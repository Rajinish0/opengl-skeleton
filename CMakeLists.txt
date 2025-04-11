cmake_minimum_required(VERSION 3.15)
project(MyApp LANGUAGES C CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# List glad.c explicitly to ensure it's treated as C
set(SOURCES
    src/glad.c
)

# Add C++ source files
file(GLOB CPP_SOURCES CONFIGURE_DEPENDS src/*.cpp)
list(APPEND SOURCES ${CPP_SOURCES})

# Make sure glad.c is explicitly treated as C
set_source_files_properties(src/glad.c PROPERTIES LANGUAGE C)

# Include directories
include_directories(
    include
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Find required packages
find_package(glfw3 REQUIRED)
find_package(OpenAL REQUIRED)
find_package(assimp REQUIRED)
find_package(Freetype REQUIRED)
find_package(PNG REQUIRED)
find_package(BZip2 REQUIRED)
find_package(ZLIB REQUIRED)
find_package(SndFile REQUIRED)
find_package(glm REQUIRED)

# Link libraries
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        glfw
        OpenAL::OpenAL
        assimp::assimp
        Freetype::Freetype
        PNG::PNG
        BZip2::BZip2
        ZLIB::ZLIB
        SndFile::sndfile
	glm::glm
        gdi32
        opengl32
)

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

set(RESOURCE_DIRS
    shaders
    models
    fonts
)

foreach(DIR ${RESOURCE_DIRS})
    if(EXISTS ${CMAKE_SOURCE_DIR}/${DIR})
        add_custom_command(
            TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/${DIR}
		${CMAKE_BINARY_DIR}/${DIR}
            COMMENT "Copying ${DIR} folder to bin directory"
        )
    endif()
endforeach()
