add_executable(example main.cpp)
target_link_libraries(example PRIVATE Vizun)

set(CMAKE_CXX_FLAGS "-O3 -march=native -flto -DNDEBUG -mavx2 -mfma -ffast-math -funroll-loops")
