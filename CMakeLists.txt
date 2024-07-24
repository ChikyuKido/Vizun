cmake_minimum_required(VERSION 3.28)
project(Vizun)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(VIZUN_BUILD_EXAMPLES "Build the Vizun example programs" ON)
option(VIZUN_ENABLE_VALIDATION_LAYER "Enables the validation layer for vulkan" ON)
set(VIZUN_BUILD_EXAMPLES ON)
set(VIZUN_ENABLE_VALIDATION_LAYER ON)

set(SPDLOG_FMT_EXTERNAL ON)
find_package(Vulkan REQUIRED)

include_directories(src)
include_directories(${Vulkan_INCLUDE_DIR})

add_subdirectory(lib/spdlog)
add_subdirectory(lib/glfw)
set(BUILD_SHARED_LIBS OFF)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)
add_subdirectory(lib/glm/glm)

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
        src/graphics/Renderer.cpp
        src/graphics/Renderer.hpp
        src/graphics/RenderTarget.cpp
        src/graphics/RenderTarget.hpp
        src/utils/Events.cpp
        src/utils/Events.hpp
        src/utils/Events.cpp
        src/utils/Events.hpp
        src/graphics/Vertex.hpp
        src/graphics/VulkanBuffer.cpp
        src/graphics/VulkanBuffer.hpp
)
add_library(Vizun ${SOURCE_FILES})
target_link_libraries(Vizun PUBLIC glfw ${Vulkan_LIBRARIES} glm-header-only spdlog stdc++)

add_custom_target(build_shaders ALL
        COMMAND "${PROJECT_SOURCE_DIR}/rsc/shaders/compile.sh"
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/rsc/shaders"
)

add_dependencies(Vizun build_shaders)

if(VIZUN_BUILD_EXAMPLES)
    message("Building vizun examples")
    add_subdirectory(example)
endif ()
if (VIZUN_ENABLE_VALIDATION_LAYER)
    add_definitions(-DVIZUN_ENABLE_VALIDATION_LAYER)
endif ()

