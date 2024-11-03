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
set(VIZUN_ENABLE_ASSERT ON)

find_package(Vulkan REQUIRED)

include_directories(src)
include_directories(lib/stb_image)
include_directories(lib/glm)
include_directories(${Vulkan_INCLUDE_DIR})

set(SPDLOG_FMT_EXTERNAL OFF)
add_subdirectory(lib/spdlog)

set(BUILD_SHARED_LIBS OFF)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)
set(GLFW_BUILD_X11 ON)
set(GLFW_BUILD_WAYLAND OFF)
add_subdirectory(lib/glfw)

add_subdirectory(lib/vma)



set(SOURCE_FILES
        src/graphics/window/RenderWindow.cpp
        src/graphics/window/RenderWindow.hpp
        src/utils/Logger.hpp
        src/graphics/base/VulkanBase.cpp
        src/graphics/base/VulkanBase.hpp
        src/config/VulkanRenderWindowConfig.hpp
        src/utils/VulkanUtils.hpp
        src/utils/VulkanUtils.cpp
        src/graphics/base/VulkanSwapchain.cpp
        src/graphics/base/VulkanSwapchain.hpp
        src/graphics/pipeline/VulkanGraphicsPipeline.cpp
        src/graphics/pipeline/VulkanGraphicsPipeline.hpp
        src/graphics/pipeline/VulkanGraphicsPipeline.cpp
        src/graphics/pipeline/VulkanGraphicsPipeline.hpp
        src/graphics/renderpass/VulkanRenderPass.cpp
        src/graphics/renderpass/VulkanRenderPass.hpp
        src/graphics/renderer/targets/RenderTarget.hpp
        src/utils/Events.cpp
        src/utils/Events.hpp
        src/utils/Events.cpp
        src/utils/Events.hpp
        src/data/ImageVertex.hpp
        src/graphics/resources/VulkanBuffer.cpp
        src/graphics/resources/VulkanBuffer.hpp
        src/graphics/resources/VulkanImage.cpp
        src/graphics/resources/VulkanImage.hpp
        src/graphics/renderer/VulkanRenderer.cpp
        src/graphics/renderer/VulkanRenderer.hpp
        src/graphics/pipeline/VulkanGraphicsPipelineDescriptor.cpp
        src/graphics/pipeline/VulkanGraphicsPipelineDescriptor.hpp
        src/config/VizunConfig.hpp
        src/graphics/renderer/targets/Image.cpp
        src/graphics/renderer/targets/Image.cpp
        src/graphics/renderer/targets/Image.hpp
        src/core/VizunEngine.cpp
        src/core/VizunEngine.hpp
        src/resource_loader/ResourceLoader.hpp
        src/resource_loader/loader/Loader.hpp
        src/resource_loader/loader/VulkanImageLoader.hpp
        src/resource_loader/ResourceLoader.cpp
        src/graphics/renderer/Camera.cpp
        src/graphics/renderer/Camera.hpp
        src/graphics/renderer/VulkanGraphicsPipelineRenderer.hpp
        src/graphics/renderer/VulkanImagePipelineRenderer.cpp
        src/graphics/renderer/VulkanImagePipelineRenderer.hpp
        src/graphics/renderer/VulkanLinePipelineRenderer.cpp
        src/graphics/renderer/VulkanLinePipelineRenderer.hpp
        src/graphics/renderer/targets/Line.cpp
        src/graphics/renderer/targets/Line.hpp
        src/graphics/renderer/targets/Transform.hpp
        src/data/Vertex.hpp
        src/data/Color.hpp
)
add_library(Vizun ${SOURCE_FILES})

target_link_libraries(Vizun PUBLIC glfw ${Vulkan_LIBRARIES} spdlog stdc++ GPUOpen::VulkanMemoryAllocator)

add_custom_target(build_shaders ALL
        COMMAND "${PROJECT_SOURCE_DIR}/rsc/shaders/compile.sh"
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/rsc/shaders"
)

add_definitions(-frtti)
add_definitions(-DVULKAN_HPP_NO_EXCEPTIONS -DVULKAN_HPP_NO_SMART_HANDLE -DVULKAN_HPP_NO_SETTERS -DVULKAN_HPP_NO_SPACESHIP_OPERATOR -DVULKAN_HPP_NO_TO_STRING)

add_dependencies(Vizun build_shaders)


#set(CMAKE_CXX_FLAGS "-O3 -march=native -flto -DNDEBUG -mavx2 -mfma -ffast-math -funroll-loops")


if(VIZUN_BUILD_EXAMPLES)
    message("Building vizun examples")
    add_subdirectory(example)
endif ()
if (VIZUN_ENABLE_VALIDATION_LAYER)
    add_definitions(-DVIZUN_ENABLE_VALIDATION_LAYER)
endif ()
if (VIZUN_ENABLE_ASSERT)
    add_definitions(-DVIZUN_ENABLE_ASSERT)
endif ()

