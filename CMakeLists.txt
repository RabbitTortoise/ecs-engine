# Specify minimum supported cmake version and set c++20
cmake_minimum_required(VERSION 3.8)
set (CMAKE_CXX_STANDARD 20)


#Set the project (solution name)
project(
	"ECS_Engine"
	VERSION 0.1.0
	LANGUAGES CXX
)


#Library options, external
set(GLFW_BUILD_EXAMPLES "" OFF    CACHE    BOOL    "ASSIMP"    GLFW)
set(GLFW_BUILD_TESTS "" OFF    BOOL    "ASSIMP"    GLFW)
set(GLFW_BUILD_DOCS "" OFF    BOOL    "ASSIMP"    GLFW)
set(GLFW_INSTALL "" OFF    BOOL    "ASSIMP"    GLFW)

# Assimp options
set (ASSIMP_BUILD_ZLIB    ON    CACHE    BOOL    "ASSIMP"    FORCE)
set (ASSIMP_BUILD_TESTS    OFF    CACHE    BOOL    "ASSIMP"    FORCE)
set (ASSIMP_BUILD_ASSIMP_TOOLS    OFF    CACHE    BOOL    "ASSIMP"    FORCE)
set (ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT    OFF    CACHE    BOOL    "ASSIMP"    FORCE)
set (ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT    OFF    CACHE    BOOL    "ASSIMP"    FORCE)
set (ASSIMP_BUILD_FBX_IMPORTER    ON    CACHE    BOOL    "ASSIMP"    FORCE)
set (ASSIMP_BUILD_FBX_EXPORTER    ON    CACHE    BOOL    "ASSIMP"    FORCE)
set (ASSIMP_BUILD_OBJ_IMPORTER    ON    CACHE    BOOL    "ASSIMP"    FORCE)
set (ASSIMP_BUILD_OBJ_EXPORTER    ON    CACHE    BOOL    "ASSIMP"    FORCE)

#Subdirectories, external
add_subdirectory(Engine/External/glfw-3.3.6)
add_subdirectory(Engine/External/glm-0.9.9.8)
add_subdirectory(Engine/External/assimp-5.0.1)

#Include directories, external
include_directories("Engine/External/stb_image")
include_directories("${GLFW_SOURCE_DIR}/deps")

#Used sources, external
set(GLAD_GL "${GLFW_SOURCE_DIR}/deps/glad/gl.h" "${GLFW_SOURCE_DIR}/deps/glad_gl.c")

#Include directories, project
include_directories(".") 
include_directories("Engine/ECS")
include_directories("Engine/ECS/Components")
include_directories("Engine/ECS/Entities")
include_directories("Engine/ECS/Systems")
include_directories("Engine/Core")
include_directories("Engine/Interface")
include_directories("Engine/OpenGL")
include_directories("Engine/OpenGL/Shaders")
include_directories("Engine/OpenGL/ECS/Components")
include_directories("Engine/OpenGL/ECS/Entities")
include_directories("Engine/OpenGL/ECS/Systems")
include_directories("Game")
include_directories("Game/Entities")
include_directories("Game/Components")
include_directories("Game/Systems")
include_directories("Game/Scripts")

file(GLOB ECS_ENGINE_SRC CONFIGURE_DEPENDS
    "Engine/ECS/*.h"
    "Engine/ECS/*.cpp"
	"Engine/ECS/Components/*.h"
	"Engine/ECS/Components/*.cpp"
    "Engine/ECS/Entities/*.h"
	"Engine/ECS/Entities/*.cpp"
	"Engine/ECS/Systems/*.h"
	"Engine/ECS/Systems/*.cpp"
    "Engine/ECS/*.cpp"
	"Engine/ECS/*.h"
    "Engine/ECS/*.cpp"
	"Engine/Interface/*.h"
    "Engine/Interface/*.cpp"
	"Engine/Core/*.h"
    "Engine/Core/*.cpp"
	"Engine/OpenGL/*.h"
    "Engine/OpenGL/*.cpp"
	"Engine/OpenGL/Shaders/*.frag"
    "Engine/OpenGL/Shaders/*.vert"
	"Engine/OpenGL/ECS/Components/*.h"
    "Engine/OpenGL/ECS/Components/*.cpp"
	"Engine/OpenGL/ECS/Entities/*.h"
    "Engine/OpenGL/ECS/Entities/*.cpp"
	"Engine/OpenGL/ECS/Systems/*.h"
    "Engine/OpenGL/ECS/Systems/*.cpp"
	
	"Game/*.h"
    "Game/*.cpp"
	"Game/Entities/*.h"
    "Game/Entities/*.cpp"
	"Game/Components/*.h"
    "Game/Components/*.cpp"
	"Game/Systems/*.h"
    "Game/Systems/*.cpp"
    "Game/Scripts/*.h"
    "Game/Scripts/*.cpp"
)

add_executable(ECS_Engine ${ECS_ENGINE_SRC} ${GLAD_GL})
target_link_libraries(ECS_Engine PUBLIC glfw PUBLIC glm PUBLIC assimp)

# Create the source groups for source tree with root at CMAKE_CURRENT_SOURCE_DIR.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${ECS_ENGINE_SRC} ${GLAD_GL})
