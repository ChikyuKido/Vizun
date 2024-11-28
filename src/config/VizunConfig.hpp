#ifndef VIZUNCONFIG_HPP
#define VIZUNCONFIG_HPP

#ifndef FRAMES_IN_FLIGHT
#define FRAMES_IN_FLIGHT 2
#endif

#ifndef MAX_IMAGES_IN_SHADER
#define MAX_IMAGES_IN_SHADER 16
#endif
#ifndef MAX_TEXTURE_PACK_WIDTH
#define MAX_TEXTURE_PACK_WIDTH 4096
#endif

#ifndef TRANSFORM_BUFFER_SIZE
#define TRANSFORM_BUFFER_SIZE 16000
#endif

#define FRAMES(type) \
    std::array<type,FRAMES_IN_FLIGHT>

#ifndef VIZUN_ENABLE_ASSERT
    #define VZ_ASSERT(condition, message) ((void)0);
#else
    #define VZ_ASSERT(condition, message) \
    if (!(condition)) { \
    const std::string errorMsg = std::string(__FILE__) + ":" + std::to_string(__LINE__) + " - " + message; \
    VZ_LOG_CRITICAL(errorMsg);\
    } \

#endif

#endif
