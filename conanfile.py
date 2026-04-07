from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, cmake_layout


class ConanApplication(ConanFile):
    name = "MyGameProject"
    version = "1.0.0"
    package_type = "application"
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps"
    tool_requires = "cmake/[>=4.0]"

    def requirements(self) -> None:
        if self.requires is not None:
            self.requires("fmt/12.1.0")
            self.requires("spdlog/1.17.0")
            self.requires("simdjson/4.2.4")
            self.requires("yaml-cpp/0.9.0")
            self.requires("tomlplusplus/3.4.0")
            self.requires("catch2/3.13.0")
            self.requires("boost/1.90.0")
            self.requires("entt/3.16.0")
            self.requires("glm/1.0.1")

    # TODO: Port over requirements
    # def requirements(self) -> None:
    #     self.requires("assimp/6.0.2")
    #     #self.requires("sdl/3.2.20")
    #     #self.requires("glfw/3.4")
    #     #self.requires("sfml/3.0.2")
    #     #self.requires("miniaudio/0.11.22")
    #     #self.requires("imgui/1.92.5")
    #     #self.requires("magic_enum/0.9.7")
    #     #self.requires("zlib/1.3.1")
    #     #self.requires("nameof/0.10.4")
    #     #self.requires("lz4/1.10.0")
    #     #self.requires("box2d/3.1.1")

    def build_requirements(self) -> None:
        pass

    def configure(self) -> None:
        if self.options is not None:
            # Boost Components
            self.options["boost"].without_asio = False
            self.options["boost"].without_atomic = False
            self.options["boost"].without_beast = False
            self.options["boost"].without_charconv = True
            self.options["boost"].without_chrono = True
            # Disable Boost.Cobalt for boost_cobalt_io_ssl packaging issue
            self.options["boost"].without_cobalt = True
            self.options["boost"].without_context = False
            self.options["boost"].without_contract = True
            self.options["boost"].without_coroutine = True
            self.options["boost"].without_coroutine2 = False
            self.options["boost"].without_date_time = False
            self.options["boost"].without_exception = True
            self.options["boost"].without_fiber = True
            self.options["boost"].without_filesystem = False
            self.options["boost"].without_graph = False
            self.options["boost"].without_interprocess = False
            self.options["boost"].without_iostreams = True
            self.options["boost"].without_json = False
            self.options["boost"].without_locale = True
            self.options["boost"].without_log = True
            self.options["boost"].without_math = False
            self.options["boost"].without_multiprecision = False
            self.options["boost"].without_nowide = True
            self.options["boost"].without_pfr = False
            self.options["boost"].without_process = False
            self.options["boost"].without_random = False
            self.options["boost"].without_regex = False
            self.options["boost"].without_serialization = False
            self.options["boost"].without_signals2 = False
            self.options["boost"].without_spirit = False
            self.options["boost"].without_stacktrace = True
            self.options["boost"].without_system = False
            self.options["boost"].without_test = True
            self.options["boost"].without_thread = True
            self.options["boost"].without_timer = True
            self.options["boost"].without_type_erasure = True
            self.options["boost"].without_url = False
            self.options["boost"].without_wave = True
            self.options["boost"].without_xpressive = False

    def layout(self) -> None:
        cmake_layout(self)

    def generate(self) -> None:
        tc = CMakeToolchain(self)
        tc.user_presets_path = 'False'
        tc.generate()

    def build(self) -> None:
        cmake = CMake(self)
        cmake.configure()
        cmake.build()