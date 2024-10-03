cmake_minimum_required(VERSION 3.28)
project(Vizun)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)
#set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(VIZUN_BUILD_EXAMPLES "Build the Vizun example programs" ON)
option(VIZUN_ENABLE_VALIDATION_LAYER "Enables the validation layer for vulkan" ON)
set(VIZUN_BUILD_EXAMPLES ON)
set(VIZUN_ENABLE_VALIDATION_LAYER OFF)

set(SPDLOG_FMT_EXTERNAL ON)
find_package(Vulkan REQUIRED)

include_directories(src)
include_directories(lib/stb_image)
include_directories(lib/glm)
include_directories(${Vulkan_INCLUDE_DIR})

add_subdirectory(lib/spdlog)
add_subdirectory(lib/glfw)
set(BUILD_SHARED_LIBS OFF)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)
set(GLFW_BUILD_X11 ON)
set(GLFW_BUILD_WAYLAND OFF)


set(SOURCE_FILES
        src/graphics/RenderWindow.cpp
        src/graphics/RenderWindow.hpp
        src/utils/Logger.hpp
        src/graphics/VulkanBase.cpp
        src/graphics/VulkanBase.hpp
        src/utils/VulkanConfig.hpp
        src/utils/VulkanUtils.hpp
        src/utils/VulkanUtils.cpp
        src/graphics/VulkanSwapchain.cpp
        src/graphics/VulkanSwapchain.hpp
        src/graphics/VulkanGraphicsPipeline.cpp
        src/graphics/VulkanGraphicsPipeline.hpp
        src/graphics/VulkanGraphicsPipeline.cpp
        src/graphics/VulkanGraphicsPipeline.hpp
        src/graphics/VulkanRenderPass.cpp
        src/graphics/VulkanRenderPass.hpp
        src/graphics/RenderTarget.hpp
        src/utils/Events.cpp
        src/utils/Events.hpp
        src/utils/Events.cpp
        src/utils/Events.hpp
        src/graphics/Vertex.hpp
        src/graphics/VulkanBuffer.cpp
        src/graphics/VulkanBuffer.hpp
        src/graphics/VulkanImage.cpp
        src/graphics/VulkanImage.hpp
        src/graphics/VulkanRenderer.cpp
        src/graphics/VulkanRenderer.hpp
        src/graphics/VulkanGraphicsPipelineDescriptor.cpp
        src/graphics/VulkanGraphicsPipelineDescriptor.hpp
        src/config/VizunConfig.hpp
        src/graphics/Image.cpp
        src/graphics/Image.cpp
        src/graphics/Image.hpp
        src/VizunEngine.cpp
        src/VizunEngine.hpp
)
add_library(Vizun ${SOURCE_FILES})

target_link_libraries(Vizun PUBLIC glfw ${Vulkan_LIBRARIES} spdlog stdc++)

add_custom_target(build_shaders ALL
        COMMAND "${PROJECT_SOURCE_DIR}/rsc/shaders/compile.sh"
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/rsc/shaders"
)

add_definitions(-DVULKAN_HPP_NO_EXCEPTIONS -DVULKAN_HPP_NO_SMART_HANDLE -DVULKAN_HPP_NO_SETTERS -DVULKAN_HPP_NO_SPACESHIP_OPERATOR -DVULKAN_HPP_NO_TO_STRING)

add_dependencies(Vizun build_shaders)

if(VIZUN_BUILD_EXAMPLES)
    message("Building vizun examples")
    add_subdirectory(example)
endif ()
if (VIZUN_ENABLE_VALIDATION_LAYER)
    add_definitions(-DVIZUN_ENABLE_VALIDATION_LAYER)
endif ()

