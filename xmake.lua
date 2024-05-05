set_targetdir("./")
set_languages("cxx17")

add_cxxflags("clang::-stdlib=libc++")
add_cxxflags("clang::-std=c++17")
add_cxxflags("clang::-Wall")
add_cxxflags("clang::-Werror")
add_cxxflags("clang::-Wextra")

add_requires("freetype")
add_requires("glfw")
add_requires("glm")
add_requires("stb")
add_requires("glad")

target("ft_vox")
    set_toolset("clang")
    set_kind("binary")
    add_files("srcs/**.cpp")
    add_packages("freetype")
    add_packages("glfw")
    add_packages("glm")
    add_packages("stb")
    add_packages("glad")
    set_symbols("debug") -- add -g when compile