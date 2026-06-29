//
// Created by Nick on 5/29/2026.
//

#ifndef CONANFILE_PY_BITMAP_HPP
#define CONANFILE_PY_BITMAP_HPP

#include <stdint.h>
#include <memory>

struct SDL_Surface;
struct SDL_Texture;

namespace NK {
class RendererSDL3;

class Bitmap2DSDL3 {
 public:
  friend class RendererSDL3;
 public:
  Bitmap2DSDL3(RendererSDL3 const& rRenderer);
  Bitmap2DSDL3(RendererSDL3 const& rRenderer, std::string const& szFilename);
  Bitmap2DSDL3(Bitmap2DSDL3 const& other);
  ~Bitmap2DSDL3();

  Bitmap2DSDL3& operator=(Bitmap2DSDL3 const& other);

public:
  [[nodiscard]] std::shared_ptr<SDL_Texture> getTexture() const;

  [[nodiscard]] int32_t getWidth() const;
  [[nodiscard]] int32_t getHeight() const;
  [[nodiscard]] int32_t getFormat() const;

private:
  std::shared_ptr<SDL_Texture> mpTexture;
};

class BitmapCubeSDL3 {
public:
  friend class Bitmap2DSDL3;
public:
  enum EFace {
    RIGHT = 0,
    LEFT,
    TOP,
    BOTTOM,
    FRONT,
    REAR,
    NUM_FACES
  };

public:
  BitmapCubeSDL3(std::array<std::string_view, 6> const& arrFiles);
  BitmapCubeSDL3(std::string_view const& szRightFile,
    std::string_view const& szLeftFile,
    std::string_view const& szTopFile,
    std::string_view const& szBottomFile,
    std::string_view const& szFrontFile,
    std::string_view const& szRearFile);
  BitmapCubeSDL3(std::array<Bitmap2DSDL3, 6> const& arrBitmaps);
  BitmapCubeSDL3(Bitmap2DSDL3 const& right,
    Bitmap2DSDL3 const& left,
    Bitmap2DSDL3 const& top,
    Bitmap2DSDL3 const& bottom,
    Bitmap2DSDL3 const& front,
    Bitmap2DSDL3 const& rear);
  BitmapCubeSDL3(BitmapCubeSDL3 const& other);
  ~BitmapCubeSDL3();

  BitmapCubeSDL3& operator=(BitmapCubeSDL3 const& other);
private:
  std::array<Bitmap2DSDL3, 6> marrCubemaps;
};
}

#endif  // CONANFILE_PY_BITMAP_HPP
