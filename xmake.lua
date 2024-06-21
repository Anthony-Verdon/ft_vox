set_targetdir("./")
set_languages("cxx17")

set_toolchains("clang")

add_ldflags("clang::-stdlib=libc++")
add_cxxflags("clang::-std=c++17")
add_cxxflags("clang::-Wall")
add_cxxflags("clang::-Werror")
add_cxxflags("clang::-Wextra")
add_cxxflags("clang::-gdwarf-4")
add_cxxflags("clang::-O3")

add_requires("freetype")
add_requires("glfw")
add_requires("glm")
add_requires("stb")
add_requires("glad")

rule("mode.debug")
    before_build(function (target)
        target:add("defines", "DEBUG_MODE")
    end)
rule_end()

rule("mode.generateMap")
    before_build(function (target)
        target:add("defines", "GENERATE_MAP")
    end)
rule_end()

target("ft_vox")
    set_kind("binary")
    if is_mode("debug") then
        add_rules("mode.debug")
        set_symbols("debug") -- add -g when compile
    end
    
    if is_mode("gen") then
        add_rules("mode.generateMap")
    end
        
    add_files("srcs/**.cpp")
    add_packages("freetype")
    add_packages("glfw")
    add_packages("glm")
    add_packages("stb")
    add_packages("glad")