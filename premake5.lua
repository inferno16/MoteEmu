workspace "MoteEmu"
    configurations
    {
        "Debug32",
        "Debug64",
        "Release32",
        "Release64"
    }

    filter "configurations:*32"
        architecture "x86"

    filter "configurations:*64"
        architecture "x64"
        
    filter "system:windows"
        cppdialect "C++17"
        systemversion "latest"

outputdir = "%{cfg.buildcfg}-%{cfg.system}"
inputdir = "%{prj.name}/src"

project "MoteEmu"
    location "MoteEmu"
    kind "WindowedApp"
    language "C++"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("obj/" .. outputdir .. "/%{prj.name}")

    files
    {
        inputdir .. "/*.h",
        inputdir .. "/*.cpp"
    }

    vpaths 
    {
        ["Header Files"] = { "**.hpp", "**.h" },
        ["Source Files"] = { "**.cpp", "**.cc", "**.c" }
    }

    includedirs { "vendor/SDL2/include" }
    links { "SDL2", "SDL2main" }

    filter "configurations:*32"
        libdirs { "vendor/SDL2/lib/x86" }

    filter "configurations:*64"
        libdirs { "vendor/SDL2/lib/x64" }
    

        