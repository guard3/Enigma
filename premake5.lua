workspace "enigma"
	configurations { "Debug", "Release" }
	platforms { "Win32", "Win64" }
	location "project"
	files "src/*.*"
	includedirs "src/*.*"
	
filter "platforms:Win32"
	architecture "x32"

filter "platforms:Win64"
	architecture "x64"

project "enigma"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	targetname "enigma"
	targetdir "project/bin/%{cfg.buildcfg}"
	characterset "MBCS"
	defines "_CRT_SECURE_NO_WARNINGS"
	
	filter "configurations:Debug"
		symbols "on"
		optimize "off"

	filter "configurations:Release"
		symbols "off"
		optimize "on"