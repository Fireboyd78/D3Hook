
workspace "D3Mods"
    platforms "x86"
    characterset "Unicode"

    configurations { "Debug", "Release" }
    
    location "build"
    os.mkdir "build/symbols"
    targetdir 'bin/%{cfg.buildcfg}'
    
    debugformat "c7" -- oldstyle PDB
    
    filter {"action:vs*", "configurations:Debug"}
        buildoptions "/GF" -- remove duplicate strings
    
     filter {"system:windows", "kind:not StaticLib"}
        linkoptions { "/PDB:\"$(SolutionDir)\\symbols\\$(ProjectName)_%{cfg.buildcfg}.pdb\"" }
         
     filter "action:vs*"
        defines
        {
            "NOMINMAX"
        }
         
    project "hook"
        language "C++"
        kind "WindowedApp"
        targetname "D3Hook"
        editandcontinue "Off"
        
        flags { "NoIncrementalLink" }
        linkoptions "/IGNORE:4254 /DYNAMICBASE:NO /LARGEADDRESSAWARE /LAST:.zdata"
        
        includedirs { 'src' }
        libdirs { 'src/dxsdk' }
        
        links { "d3dx9", "d3d9" }
        
        files 
        {
            "src/**.cpp",
            "src/**.h"
        }
        
        