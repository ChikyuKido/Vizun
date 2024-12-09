cmake_minimum_required(VERSION 3.28)
project(Vizun)


set(CMAKE_CXX_STANDARD 23)



option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(VIZUN_BUILD_EXAMPLES "Build the Vizun example programs" ON)
option(VIZUN_ENABLE_VALIDATION_LAYER "Enables the validation layer for vulkan" ON)
set(VIZUN_BUILD_EXAMPLES ON)
set(VIZUN_ENABLE_VALIDATION_LAYER OFF)
set(VIZUN_ENABLE_ASSERT OFF)

find_package(Vulkan REQUIRED)

include_directories(src)
include_directories(rsc/shaders/include)
include_directories(lib/stb_image)
include_directories(lib/stb_truetype)
include_directories(lib/stb_rect_pack)
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

add_subdirectory(lib/vma SYSTEM)



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
        src/graphics/renderer/pipeline_renderer/VulkanImagePipelineRenderer.cpp
        src/graphics/renderer/pipeline_renderer/VulkanImagePipelineRenderer.hpp
        src/graphics/renderer/pipeline_renderer/VulkanLinePipelineRenderer.cpp
        src/graphics/renderer/pipeline_renderer/VulkanLinePipelineRenderer.hpp
        src/graphics/renderer/targets/geometry/Line.cpp
        src/graphics/renderer/targets/geometry/Line.hpp
        src/data/Transform.hpp
        src/data/GeometryVertex.hpp
        src/data/Color.hpp
        src/graphics/resources/buffer/VulkanBuffer.cpp
        src/graphics/resources/buffer/VulkanBuffer.hpp
        src/graphics/resources/buffer/VertexIndexBuffer.cpp
        src/graphics/resources/buffer/VertexIndexBuffer.hpp
        src/graphics/resources/buffer/UniformBuffer.cpp
        src/graphics/resources/buffer/UniformBuffer.hpp
        src/graphics/resources/buffer/StorageBuffer.cpp
        src/graphics/resources/buffer/StorageBuffer.hpp
        src/graphics/renderer/targets/geometry/Rectangle.cpp
        src/graphics/renderer/targets/geometry/Rectangle.hpp
        src/graphics/renderer/targets/Text.cpp
        src/graphics/renderer/targets/Text.hpp
        src/graphics/resources/VulkanFont.cpp
        src/graphics/resources/VulkanFont.hpp
        src/utils/FileUtil.cpp
        src/utils/FileUtil.hpp
        src/data/FontVertex.hpp
        src/graphics/renderer/pipeline_renderer/VulkanFontPipelineRenderer.cpp
        src/graphics/renderer/pipeline_renderer/VulkanFontPipelineRenderer.hpp
        src/resource_loader/loader/VulkanFontLoader.hpp
        src/pch.h
        src/pch.cpp
        src/resource_loader/loader/VulkanFontLoader.cpp
        src/resource_loader/loader/VulkanImageLoader.cpp
        src/graphics/renderer/pipeline_renderer/VulkanTrianglePipelineRenderer.cpp
        src/graphics/renderer/pipeline_renderer/VulkanTrianglePipelineRenderer.hpp
        src/graphics/renderer/targets/geometry/Triangle.cpp
        src/graphics/renderer/targets/geometry/Triangle.hpp
        src/graphics/resources/VulkanTexture.cpp
        src/graphics/resources/VulkanTexture.hpp
)
add_library(Vizun ${SOURCE_FILES})

target_link_libraries(Vizun PUBLIC glfw ${Vulkan_LIBRARIES} spdlog stdc++ GPUOpen::VulkanMemoryAllocator)

target_precompile_headers(Vizun PRIVATE src/pch.h)

if(MSVC)
    target_compile_options(Vizun PRIVATE /Yupch.h /Fp${CMAKE_BINARY_DIR}/pch.pch)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(Vizun PRIVATE -Winvalid-pch -fpch-preprocess)
endif()

add_custom_target(build_shaders ALL
        COMMAND "${PROJECT_SOURCE_DIR}/rsc/shaders/compile.sh"
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/rsc/shaders"
)

add_definitions(-frtti)
add_definitions(-DVULKAN_HPP_NO_EXCEPTIONS -DVULKAN_HPP_NO_SMART_HANDLE -DVULKAN_HPP_NO_SETTERS -DVULKAN_HPP_NO_SPACESHIP_OPERATOR -DVULKAN_HPP_NO_TO_STRING)

add_dependencies(Vizun build_shaders)

set(CMAKE_CXX_FLAGS "-ftime-trace")
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

