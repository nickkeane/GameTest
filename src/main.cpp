//
// Created by Nick on 5/27/2026.
//

#include <cstdlib>
#include <limits>

// Ensure we have the expected floating-point support and that fast math is forced
static_assert(std::numeric_limits<float>::is_iec559,
              "IEEE 754 float support required");
static_assert(sizeof(float) == 4, "32-bit float support required");

static_assert(std::numeric_limits<double>::is_iec559,
              "IEEE 754 double support required");
static_assert(sizeof(double) == 8, "64-bit double support required");

// Fast FP Check
#ifdef _MSC_VER  // MSBuild
# ifndef _M_FP_FAST
#   warning "FP Fast is disabled"
# endif
#else  // Not MSBuild
# ifndef __FAST_MATH__
#   warning "FP Fast is disabled"
# endif
#endif

// Win32 API
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
//#include <windows.h>
//#include <winsock2.h>
//#include <wrl/client.h>
//using Microsoft::WRL::ComPtr;
//#include <directx/d3d12.h>
#endif

// spdlog
#include <spdlog/spdlog.h> // spdlog::
#include <spdlog/sinks/stdout_color_sinks.h> // spdlog::sinks::
#include <spdlog/fmt/fmt.h> // fmt::, fmt::format()

// SDL3
#ifndef SDL_MAIN_USE_CALLBACKS
#define SDL_MAIN_USE_CALLBACKS 1
#endif
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
//#include <SDL3_image/SDL_image.h>
//#include <SDL3_mixer/SDL_mixer.h>
//#include <SDL3_ttf/SDL_ttf.h>

// SDL3pp?

// EnTT
#include <entt/entt.hpp>

#include "Window.hpp"
#include "Renderer.hpp"

//#include "AssetManager.hpp"
#include "Bitmap.hpp"

// SDL Logger Callback (Using spdlog)
void SDLLogToSpdlog(void* userData, int category, SDL_LogPriority priority, const char* message) {
  (void)userData;

  spdlog::level::level_enum spdLogLevel;
  static constexpr std::array<spdlog::level::level_enum, 7> SArrLogLevels = {
    spdlog::level::trace, // SDL_LOG_PRIORITY_TRACE
    spdlog::level::debug, // SDL_LOG_PRIORITY_VERBOSE
    spdlog::level::debug, // SDL_LOG_PRIORITY_DEBUG
    spdlog::level::info, // SDL_LOG_PRIORITY_INFO
    spdlog::level::warn, // SDL_LOG_PRIORITY_WARN
    spdlog::level::err, // SDL_LOG_PRIORITY_ERROR
    spdlog::level::critical, //SDL_LOG_PRIORITY_CRITICAL
  };

  const size_t priorityIdx = static_cast<size_t>(priority) - 1;
  if (priorityIdx < SArrLogLevels.size()) [[likely]] {
    spdLogLevel = SArrLogLevels[priorityIdx];
  } else {
    spdLogLevel = spdlog::level::info;
  }

  // Skip Log Processing if Log Level is not active
  if (!spdlog::should_log(spdLogLevel)) [[unlikely]]
    return;

  std::string_view szvCategoryName;
  static constexpr std::array<std::string_view, 10> SArrCategoryNames = {
    "App", "Error", "Assert", "System", "Audio", "Video", "Render", "Input", "Test", "GPU"
  };
  if (static_cast<size_t>(category) < SArrCategoryNames.size()) [[likely]] {
    szvCategoryName = SArrCategoryNames[category];
  } else {
    szvCategoryName = (category >= SDL_LOG_CATEGORY_CUSTOM) ? "Custom" : "Unknown";
  }

  spdlog::log(spdLogLevel, "[SDL3::{}] {}", szvCategoryName, message);
}

// SDL Application State
class AppState {
public:
  enum WindowMode {
    FULL_BORDERLESS,
    FULL_EXCLUSIVE,
    WINDOWED,
  };
public:
  AppState() : mbIsRunning(false), mnDisplayIndex(0), meWindowMode(WindowMode::FULL_BORDERLESS), mpWindow(nullptr), mpRenderer(nullptr) {
    memset(&mFullscreenDisplayMode, 0, sizeof(SDL_DisplayMode));
  }
  ~AppState() {
    if (mpWindow != nullptr) [[likely]]
      SDL_DestroyWindow(mpWindow);
    mpWindow = nullptr;

    if (mpRenderer != nullptr) [[likely]]
      SDL_DestroyRenderer(mpRenderer);
    mpRenderer = nullptr;
  };
  AppState(AppState const& copy) = delete;

public:
  bool getIsRunning() const { return mbIsRunning; }
  void setIsRunning(bool bIsRunning) { mbIsRunning = bIsRunning; }

  WindowMode getWindowMode() const { return meWindowMode; }

  bool makeWindowFullscreenExclusive() {
    return makeWindowFullscreenExclusive(nullptr);
  }

  bool makeWindowFullscreenExclusive(SDL_DisplayMode const* pMode) {
    SDL_DisplayMode newMode;
    if (pMode == nullptr) {
      // Attempt - Use Closest Mode to Default
      SDL_DisplayMode closestMode;
      // TODO: A Display Index of 0 is invalid. See SDL_GetDisplays.
      // TODO: Make sure the invalid case is handled and SDL_LogWarn is actually working
      if (!SDL_GetClosestFullscreenDisplayMode(mnDisplayIndex, 2560, 1440, 0.0f, true, &closestMode)) {
        SDL_LogWarn(SDL_LOG_CATEGORY_VIDEO, "Failed to get closest fullscreen display mode. Error: %s", SDL_GetError());
        return false;
      }
      newMode = closestMode;
    } else {
      newMode = *pMode;
    }

    // Attempt Exclusive Fullscreen
    if (!SDL_SetWindowFullscreenMode(mpWindow, &newMode)) {
      SDL_LogWarn(SDL_LOG_CATEGORY_VIDEO, "Failed to set exclusive fullscreen mode. Error: %s", SDL_GetError());
      return false;
    }
    if (!SDL_SetWindowFullscreen(mpWindow, true)) {
      SDL_LogWarn(SDL_LOG_CATEGORY_VIDEO, "Failed to change to exclusive fullscreen. Error: %s", SDL_GetError());
      return false;
    }

    SDL_SyncWindow(mpWindow);
    meWindowMode = WindowMode::FULL_EXCLUSIVE;
    return true;
  }

  bool makeWindowFullscreenBorderless() {
    SDL_SetWindowFullscreenMode(mpWindow, nullptr);
    SDL_SetWindowFullscreen(mpWindow, true);
    SDL_SyncWindow(mpWindow);
    meWindowMode = WindowMode::FULL_BORDERLESS;
    return true;
  }

  bool makeWindowWindowed() {
    SDL_SetWindowFullscreenMode(mpWindow, nullptr);
    SDL_SetWindowFullscreen(mpWindow, false);
    SDL_SyncWindow(mpWindow);
    meWindowMode = WindowMode::WINDOWED;
    return true;
  }

  SDL_Window* getWindow() { return mpWindow; }
  void setWindow(SDL_Window* pWindow) { mpWindow = pWindow; }

  SDL_Renderer* getRenderer() { return mpRenderer; }
  void setRenderer(SDL_Renderer* pRenderer) { mpRenderer = pRenderer; }

private:
  bool mbIsRunning; // Is App Running?
  int mnDisplayIndex; // Selected Display for App

  // TODO: Don't track mFullscreenDisplayMode, instead use SDL_GetWindowFullscreenMode, SDL_GetCurrentDisplayMode, SDL_GetDesktopDisplayMode, and SDL_GetWindowFlags
  SDL_DisplayMode mFullscreenDisplayMode;

  // TODO: Same with meWindowMode, provide a function that generates the enum from SDL_* functions instead
  WindowMode meWindowMode;

  SDL_Window* mpWindow;
  SDL_Renderer* mpRenderer;
};

// SDL Application Callbacks
SDL_AppResult SDL_AppInit   (void** appstate, int argc, char* argv[]);
SDL_AppResult SDL_AppEvent  (void* appstate, SDL_Event* event);
SDL_AppResult SDL_AppUpdate (void* appstate);
void          SDL_AppQuit   (void* appstate, SDL_AppResult result);

// Application State
//bool gbAppIsRunning= false;

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
  (void)argc;
  (void)argv;

  AppState* pAppState = new AppState();
  *appstate = pAppState;

  // Initialize Logger
  spdlog::set_pattern("[%Y-%m-%d %I:%M:%S.%e %p] [%^%l%$] %v");
#if _DEBUG
  spdlog::set_level(spdlog::level::trace);
  //spdlog::set_level(spdlog::level::debug);
#else
  spdlog::set_level(spdlog::level::info);
#endif

  SDL_SetAppMetadata("MyGameProject", "1.0", "com.nk.mygameproject");
  SDL_SetLogOutputFunction(&SDLLogToSpdlog, nullptr);

  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Starting Game...");

  // Initialize App
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD | SDL_INIT_JOYSTICK)) {
    SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Failed to initialize SDL: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  SDL_Window* pWindow = nullptr;
  SDL_Renderer* pRenderer = nullptr;
  // SDL_WINDOW_FULLSCREEN
  // SDL_WINDOW_BORDERLESS
  // SDL_WINDOW_RESIZABLE
  // SDL_WINDOW_MINIMIZED
  // SDL_WINDOW_MAXIMIZED

  // SDL_WINDOW_OPENGL
  // SDL_WINDOW_VULKAN
  // SDL_WINDOW_METAL
  // SDL_WINDOW_INPUT_FOCUS
  // SDL_WINDOW_MOUSE_FOCUS
  if (!SDL_CreateWindowAndRenderer("MyGameProject", 2560, 1440, SDL_WINDOW_FULLSCREEN | SDL_WINDOW_BORDERLESS, &pWindow, &pRenderer)) {
    SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, "Failed to create Window and Renderer: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }
  SDL_SetRenderLogicalPresentation(pRenderer, SDL_LOGICAL_PRESENTATION_LETTERBOX);
  pAppState->setWindow(pWindow);
  pAppState->setRenderer(pRenderer);

  // TODO: Attempt to put Borderless and/or Windowed Window into Fullscreen Exclusive Mode
  //pAppState->makeWindowFullscreenExclusive(nullptr);

  // TODO: Attempt to swap fullscreen exclusive mode with a different Fullscreen Exclusive Mode
  // Start App Running
  pAppState->setIsRunning(true);
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
  AppState& rAppState = *static_cast<AppState*>(appstate);

  // Render Frame
  SDL_Renderer* pRenderer = rAppState.getRenderer();
  SDL_SetRenderDrawColor(pRenderer, 255, 0, 0, 255);
  SDL_RenderClear(pRenderer);
  SDL_RenderPresent(pRenderer);

  // Quit w/ Success
  if (!rAppState.getIsRunning()) {
    return SDL_APP_SUCCESS;
  }

  // Continue Running
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* pEvent) {
  AppState& rAppState = *static_cast<AppState*>(appstate);
  // Short-circuit Event Handler if App has been shutdown
  if (!rAppState.getIsRunning()) {
    return SDL_APP_SUCCESS;
  }

  if (pEvent != nullptr) {
    switch (pEvent->type) {
      case SDL_EVENT_QUIT:
        rAppState.setIsRunning(false);
        return SDL_APP_SUCCESS;
      case SDL_EVENT_KEY_UP:
        if (pEvent->key.key == SDLK_ESCAPE) {
          rAppState.setIsRunning(false);
        }
        break;
      default:
        break;
    }
  }

  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
  (void)result;

  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Stopping Game...");

  // Shutdown App
  delete static_cast<AppState*>(appstate);

  // Quit Program
  SDL_Quit();
}