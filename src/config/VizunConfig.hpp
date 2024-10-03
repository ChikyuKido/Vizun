#ifndef VIZUNCONFIG_HPP
#define VIZUNCONFIG_HPP

#ifndef FRAMES_IN_FLIGHT
#define FRAMES_IN_FLIGHT 2
#endif

#ifndef MAX_IMAGES_IN_SHADER
#define MAX_IMAGES_IN_SHADER 16
#endif

#define FRAMES(type) \
    std::array<type,FRAMES_IN_FLIGHT>



#endif
