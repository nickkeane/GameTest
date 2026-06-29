//
// Created by Nick on 5/29/2026.
//

#include "Bitmap.hpp"

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>

#include <SDL3_image/SDL_image.h>

#include "Renderer.hpp"

namespace NK {
// Bitmap2DSDL3
Bitmap2DSDL3::Bitmap2DSDL3(RendererSDL3 const& rRenderer) : Bitmap2DSDL3(rRenderer, "Assets/test.png") {  }
Bitmap2DSDL3::Bitmap2DSDL3(RendererSDL3 const& rRenderer, std::string const& szFilename) : mpTexture(nullptr) {
  SDL_Surface* pSurface = SDL_LoadBMP(szFilename.c_str());
  if (!pSurface) {
    SDL_LogWarn(SDL_LOG_CATEGORY_VIDEO, "Could not create SDL_Surface object. Error: %s", SDL_GetError());
    return;
  }

  // Create a Texture Object
  SDL_Texture* pTexture = SDL_CreateTextureFromSurface(rRenderer.getHandle().get(), pSurface);
  if (!pTexture) {
    SDL_LogWarn(SDL_LOG_CATEGORY_VIDEO, "Could not create SDL_Texture object. Error: %s", SDL_GetError());
    SDL_DestroySurface(pSurface);
    return;
  }

  // Texture copied to GPU; Release surface
  SDL_DestroySurface(pSurface);
  mpTexture = std::shared_ptr<SDL_Texture>(pTexture, SDL_DestroyTexture);
}
Bitmap2DSDL3::Bitmap2DSDL3(Bitmap2DSDL3 const& other) : mpTexture(other.mpTexture) {  }
Bitmap2DSDL3::~Bitmap2DSDL3() {  }
Bitmap2DSDL3& Bitmap2DSDL3::operator=(Bitmap2DSDL3 const& other) {
  if (this == &other)
    return *this;
  mpTexture = other.mpTexture;
  return *this;
}

std::shared_ptr<SDL_Texture> Bitmap2DSDL3::getTexture() const {
  return mpTexture;
}

int32_t Bitmap2DSDL3::getWidth() const {
  return mpTexture->w;
}

int32_t Bitmap2DSDL3::getHeight() const {
  return mpTexture->h;
}

int32_t Bitmap2DSDL3::getFormat() const {
  return mpTexture->format;
}

// BitmapCubeSDL3

}  // namespace NK