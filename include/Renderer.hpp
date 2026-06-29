//
// Created by Nick on 5/29/2026.
//

#ifndef CONANFILE_PY_RENDERER_HPP
#define CONANFILE_PY_RENDERER_HPP

#include <memory>

#include "../build/ucrt64/Release/_deps/sdl-src/include/SDL3/SDL_video.h"
#include "Bitmap.hpp"

struct SDL_Renderer;
struct SDL_Window;

namespace NK {
  // class IRenderer {
  //
  // };

  class RendererSDL3 {
  public:
    RendererSDL3(SDL_Window* pWindow);
    RendererSDL3(RendererSDL3 const& other);
    ~RendererSDL3();

    RendererSDL3& operator=(RendererSDL3 const& other);

  public:
    [[nodiscard]] bool exists() const;
    [[nodiscard]] long usages() const;
    [[nodiscard]] std::shared_ptr<SDL_Renderer> getHandle() const;

  public:
    std::shared_ptr<Bitmap2DSDL3> createBitmap2D(std::string const& szPath);

  private:
    std::shared_ptr<SDL_Renderer> mpRenderer;
  };
}

#endif  // CONANFILE_PY_RENDERER_HPP
