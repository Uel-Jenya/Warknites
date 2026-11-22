cmake_minimum_required(VERSION 3.10.0)
project(Warknites VERSION 0.1.0 LANGUAGES C CXX)

set(src src/Renderer/main.cpp
    src/Renderer/Game.cpp
    src/core/Base.cpp
)

# 1. Point CMake to your local SDL3 directory (must be BEFORE find_package)
list(APPEND CMAKE_PREFIX_PATH "D:/libs/SDL3")

# 2. Locate SDL3 config
find_package(SDL3 CONFIG REQUIRED)

#set(OGRE_STATIC true)
if(NOT OGRE_HOME AND NOT DEFINED ENV{OGRE_HOME})
    set(OGRE_HOME "D:/libs/ogre-next-vcc" CACHE PATH "OGRE SDK directory")
endif()

# Add OGRE CMake modules to prefix path
list(APPEND CMAKE_PREFIX_PATH "${OGRE_HOME}" "${OGRE_HOME}/CMake")
list(APPEND CMAKE_MODULE_PATH "${OGRE_HOME}/CMake")

set(OGRE_INCLUDE_DIR "D:/libs/ogre-next-vcc/include/OGRE-Next/")
# 2. Find OGRE-Next components
# Available components: OgreMain, OgreHlmsPbs, OgreHlmsUnlit, OgreOverlay, etc.
find_package(OGRE REQUIRED)


add_executable(Warknites ${src})
target_link_libraries(Warknites PRIVATE SDL3::SDL3 ${OGRE_LIBRARIES}
    ${OGRE_HlmsPbs_LIBRARIES}
    ${OGRE_HlmsUnlit_LIBRARIES} 
    )
target_include_directories(Warknites PRIVATE ${CMAKE_SOURCE_DIR}/include
                                        ${CMAKE_SOURCE_DIR}/include
                                        ${OGRE_INCLUDE_DIRS}
                                        ${OGRE_HlmsPbs_INCLUDE_DIRS}
                                        ${OGRE_HlmsUnlit_INCLUDE_DIRS}
                                        "D:/libs/ogre-next-vcc/include/OGRE-Next"
                                        "D:/libs/ogre-next-vcc/include/OGRE-Next/Hlms/Common"
                                        "D:/libs/ogre-next-vcc/include/OGRE-Next/Hlms/Pbs"
                                        "D:/libs/ogre-next-vcc/include/OGRE-Next/Hlms/Unlit"
)


# 1. Copies directly linked targets (SDL2, OgreMain)
add_custom_command(TARGET Warknites POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_RUNTIME_DLLS:Warknites>
        $<TARGET_FILE_DIR:Warknites>
    COMMAND_EXPAND_LISTS
)

# 2. Copies unlinked runtime plugins & config files from Ogre's bin folder
add_custom_command(TARGET Warknites POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "D:/libs/ogre-next-vcc/bin/$<CONFIG>"
        $<TARGET_FILE_DIR:Warknites>
)