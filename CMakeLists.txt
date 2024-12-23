cmake_minimum_required(VERSION 3.10)

# Project name
project(extmap VERSION 1.0 LANGUAGES C)

# Set C standard
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Enable all warnings and pedantic errors
if (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
elseif (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(/W4 /WX)
endif()

# Add the executable
add_executable(extmap main.c)

# Set optimized build flags for Release configuration
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2")

# Installation setup
install(TARGETS extmap RUNTIME DESTINATION bin)

# Packaging setup
set(CPACK_PACKAGE_NAME "extmap")
set(CPACK_PACKAGE_VERSION "1.0")
set(CPACK_PACKAGE_CONTACT "Your Name <your.email@example.com>")

include(CPack)
