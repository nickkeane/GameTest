//
// Created by Nick on 5/29/2026.
//
#include "Renderer.hpp"

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

namespace NK {
RendererSDL3::RendererSDL3(SDL_Window* pWindow) : mpRenderer(nullptr) {
  SDL_Renderer* pRenderer = SDL_CreateRenderer(pWindow, nullptr);
  if (pRenderer == nullptr) [[unlikely]]
    SDL_LogWarn(SDL_LOG_CATEGORY_VIDEO, "Renderer could not be created: %s", SDL_GetError());
  else
    mpRenderer = std::shared_ptr<SDL_Renderer>(pRenderer, SDL_DestroyRenderer);
}

RendererSDL3::RendererSDL3(RendererSDL3 const& other) : mpRenderer(other.mpRenderer) {
}

RendererSDL3::~RendererSDL3() {
  mpRenderer = nullptr;
}

RendererSDL3& RendererSDL3::operator=(RendererSDL3 const& other) {
  if (this == &other) [[unlikely]]
    return *this;
  mpRenderer = other.mpRenderer;
  return *this;
}

bool RendererSDL3::exists() const {
  return (mpRenderer != nullptr);
}

long RendererSDL3::usages() const {
  return mpRenderer.use_count();
}

std::shared_ptr<SDL_Renderer> RendererSDL3::getHandle() const {
  return mpRenderer;
}
}  // namespace NK