//
// Created by Nick on 5/28/2026.
//

#ifndef CONANFILE_PY_WINDOW_HPP
#define CONANFILE_PY_WINDOW_HPP

#include <memory>
#include <utility>

struct SDL_Window;
struct SDL_Renderer;

namespace NK {
  class ISharedClass {
  public:
    virtual ~ISharedClass() = default;

  public:
    [[nodiscard]] virtual bool exists() const = 0;
    [[nodiscard]] virtual long usages() const = 0;
  };

  // class IWindow {
  // public:
  //   virtual ~IWindow();
  // };

  class WindowSDL3 : public ISharedClass { // : public IWindow {
  public:
    WindowSDL3();
    WindowSDL3(std::string const& szTitle, int nWidth, int nHeight, bool bIsFullscreen, bool bIsBorderless);
    WindowSDL3(WindowSDL3 const& other);
    ~WindowSDL3() override;

  public:
    WindowSDL3& operator=(WindowSDL3 const& other);

  public:
    [[nodiscard]] bool exists() const override;
    [[nodiscard]] long usages() const override;
    [[nodiscard]] std::shared_ptr<SDL_Window> getHandle() const;

  public:
    [[nodiscard]] int getX() const;
    [[nodiscard]] int getY() const;
    [[nodiscard]] int getWidth() const;
    [[nodiscard]] int getHeight() const;

    [[nodiscard]] std::pair<int, int> getPos() const;
    [[nodiscard]] std::pair<int, int> getDims() const;

  private:
    std::shared_ptr<SDL_Window> mpWindow;
  };
} // namespace NK
#endif  // CONANFILE_PY_WINDOW_HPP
