add_requires("freetype")
add_requires("glfw")
add_requires("glm")

target("ft_vox")
    set_kind("binary")
    add_files("srcs/**.cpp")
    add_files("libs/glad/src/glad.cpp")
    add_packages("freetype")
    add_packages("glfw")
    add_packages("glm")
    set_symbols("debug") -- add -g when compile

target("ft_vox_debug")
    add_defines ("DEBUG_MODE")
    add_deps("ft_vox")
