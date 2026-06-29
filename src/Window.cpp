//
// Created by Nick on 5/28/2026.
//

#include "Window.hpp"

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_video.h>

#include <memory>
#include <limits>

namespace NK {
WindowSDL3::WindowSDL3() : WindowSDL3("TestWindow", 800, 600, true, true) {}
WindowSDL3::WindowSDL3(std::string const& szTitle, int nWidth, int nHeight, bool bIsFullscreen, bool bIsBorderless) : mpWindow(nullptr) {
  SDL_WindowFlags winFlags = 0ULL;
  if (bIsFullscreen) [[likely]]
    winFlags |= SDL_WINDOW_FULLSCREEN;
  if (bIsBorderless) [[likely]]
    winFlags |= SDL_WINDOW_BORDERLESS;
  SDL_Window* pWindow = SDL_CreateWindow(szTitle.c_str(), nWidth, nHeight, winFlags);
  if (pWindow == nullptr) [[unlikely]]
    SDL_LogWarn(SDL_LOG_CATEGORY_VIDEO, "Window could not be created: %s", SDL_GetError());
  else
    mpWindow = std::shared_ptr<SDL_Window>(pWindow, SDL_DestroyWindow);
}

WindowSDL3::WindowSDL3(WindowSDL3 const& other) : mpWindow(other.mpWindow) {
}

WindowSDL3::~WindowSDL3() {
  mpWindow = nullptr;
}

WindowSDL3& WindowSDL3::operator=(WindowSDL3 const& other) {
  if (this == &other) [[unlikely]]
    return *this;
  mpWindow = other.mpWindow;
  return *this;
}

bool WindowSDL3::exists() const {
  return (mpWindow != nullptr);
}

long WindowSDL3::usages() const {
  return mpWindow.use_count();
}

std::shared_ptr<SDL_Window> WindowSDL3::getHandle() const {
  return mpWindow;
}

int WindowSDL3::getX() const {
  int x;
  if (!SDL_GetWindowPosition(mpWindow.get(), &x, nullptr)) [[unlikely]]
    return std::numeric_limits<int>::min();
  return x;
}

int WindowSDL3::getY() const {
  int y;
  if (!SDL_GetWindowPosition(mpWindow.get(), nullptr, &y)) [[unlikely]]
    return std::numeric_limits<int>::min();
  return y;
}

int WindowSDL3::getWidth() const {
  int w;
  if (!SDL_GetWindowSize(mpWindow.get(), &w, nullptr)) [[unlikely]]
    return std::numeric_limits<int>::min();
  return w;
}

int WindowSDL3::getHeight() const {
  int h;
  if (!SDL_GetWindowSize(mpWindow.get(), nullptr, &h)) [[unlikely]]
    return std::numeric_limits<int>::min();
  return h;
}

std::pair<int, int> WindowSDL3::getPos() const {
  int x, y;
  if (!SDL_GetWindowPosition(mpWindow.get(), &x, &y)) [[unlikely]]
    return {std::numeric_limits<int>::min(), std::numeric_limits<int>::min()};
  return {x, y};
}

std::pair<int, int> WindowSDL3::getDims() const {
  int w, h;
  if (!SDL_GetWindowSize(mpWindow.get(), &w, &h)) [[unlikely]]
    return {std::numeric_limits<int>::min(), std::numeric_limits<int>::min()};
  return {w, h};
}

}  // namespace NK