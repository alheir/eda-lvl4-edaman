cmake_minimum_required(VERSION 3.0)

project("EDA-Man Controller")

set (CMAKE_CXX_STANDARD 11)

# From "Working with CMake" documentation:
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin" OR ${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    # AddressSanitizer (ASan)
    add_compile_options(-fsanitize=address)
    add_link_options(-fsanitize=address)
endif()
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    # UndefinedBehaviorSanitizer (UBSan)
    add_compile_options(-fsanitize=undefined)
    add_link_options(-fsanitize=undefined)
endif()

add_executable(main 
                main.cpp 
                MQTTClient.cpp 
                GameModel.cpp 
                GameView.cpp 
                entities/Robot.cpp 
                entities/Player.cpp 
                entities/Enemy.cpp 
                entities/Red.cpp 
                entities/Pink.cpp 
                entities/Cyan.cpp 
                entities/Orange.cpp
                )

# Packages
find_package(raylib CONFIG REQUIRED)
find_package(raylib_cpp QUIET)
if (NOT raylib_cpp_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        raylib_cpp
        GIT_REPOSITORY https://github.com/RobLoach/raylib-cpp.git
        GIT_TAG v4.1.0
    )
    FetchContent_MakeAvailable(raylib_cpp)
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    find_package(mosquitto REQUIRED)
    find_package(OpenSSL REQUIRED)
    target_include_directories(main PRIVATE ${RAYLIB_INCLUDE_DIRS} ${MOSQUITTO_INCLUDE_DIRS})
    target_link_libraries(main PRIVATE ${RAYLIB_LIBRARIES} raylib_cpp ${MOSQUITTO_LIBRARIES} OpenSSL::SSL OpenSSL::Crypto)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    # From "Working with CMake" documentation:
    target_link_libraries(main PRIVATE "-framework IOKit" "-framework Cocoa" "-framework OpenGL")
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    target_link_libraries(main PRIVATE m ${CMAKE_DL_LIBS} pthread GL rt X11 raylib raylib_cpp mosquitto)
endif()