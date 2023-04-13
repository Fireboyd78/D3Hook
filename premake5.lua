
workspace "D3Mods"
    platforms { "x86", "DLL" }
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
            "NOMINMAX",
        }
    
    project "hook"
        language "C++"
        
        targetname "D3Hook"
        editandcontinue "Off"
        
        filter {"platforms:x86"}
          staticruntime "on"
          kind "WindowedApp"
          defines { "HOOK_EXE" }
          flags { "NoIncrementalLink" }
          linkoptions "/IGNORE:4254 /DYNAMICBASE:NO /LARGEADDRESSAWARE /LAST:.zdata"
        
        filter {"platforms:DLL"}
          staticruntime "off"
          kind "SharedLib"
          characterset "ASCII"
          defines { "HOOK_DLL" }
          linkoptions "/IGNORE:4254"
          targetname "dinput8"
          targetdir "bin/%{cfg.buildcfg}/out_dll"
         
        filter {"configurations:Debug"}
          runtime "Debug"
          defines { "_DEBUG", "DEBUG" }
        filter {"configurations:Release"}
          runtime "Release"
          defines { "_NDEBUG", "NDEBUG" }
        
        filter {}
          includedirs { 'src', 'src/imgui' }
          libdirs { 'src/dxsdk', 'src/imgui' }
          
          links { "d3dx9", "d3d9", "xinput", "kernel32", "user32", "gdi32", "winspool", "comdlg32", "advapi32", "shell32", "ole32", "oleaut32", "uuid", "odbc32", "odbccp32"  }
          
          files
          {
              "src/**.cpp",
              "src/**.h",
          }
        
        