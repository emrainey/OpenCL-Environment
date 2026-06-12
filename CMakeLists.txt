cmake_minimum_required(VERSION 3.10)
project(opencl-environment
    HOMEPAGE_URL "https://github.com/emrainey/OpenCL-Environment"
    DESCRIPTION "A CMake project for setting up an OpenCL Environment"
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(OpenCL REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLFW3 REQUIRED)
find_package(OpenGL REQUIRED)

set(CL_USER_DEVICE_COUNT 1)
set(CL_USER_DEVICE_TYPE all)


add_library(cl-interface INTERFACE)
target_include_directories(cl-interface INTERFACE include)
target_compile_definitions(cl-interface INTERFACE CL_USER_DEVICE_COUNT=${CL_USER_DEVICE_COUNT} CL_USER_DEVICE_TYPE=${CL_USER_DEVICE_TYPE} CL_DEBUG=1)
# Query the device at configure time, or just key off platform
if(APPLE AND CMAKE_SYSTEM_PROCESSOR MATCHES "arm64")
    target_compile_definitions(cl-interface INTERFACE GPGPU_NO_DOUBLE_SUPPORT)
endif()
target_link_libraries(cl-interface INTERFACE OpenCL::OpenCL)

add_library(clquery ${CMAKE_CURRENT_SOURCE_DIR}/source/clquery.c)
target_include_directories(clquery PUBLIC include)
target_link_libraries(clquery INTERFACE cl-interface)

add_library(clenvironment ${CMAKE_CURRENT_SOURCE_DIR}/source/clenvironment.c)
target_include_directories(clenvironment PUBLIC include)
target_link_libraries(clenvironment INTERFACE cl-interface)

add_library(clmath ${CMAKE_CURRENT_SOURCE_DIR}/source/clmath.c)
target_include_directories(clmath PUBLIC include)
target_link_libraries(clmath INTERFACE cl-interface)

add_executable(clcompiler ${CMAKE_CURRENT_SOURCE_DIR}/source/clcompiler.c)
target_link_libraries(clcompiler INTERFACE cl-interface)
target_link_libraries(clcompiler PRIVATE OpenCL::OpenCL clenvironment clmath clquery)

add_executable(clDeviceQuery ${CMAKE_CURRENT_SOURCE_DIR}/source/cldevicequery.c)
target_include_directories(clDeviceQuery PUBLIC include)
target_link_libraries(clDeviceQuery PRIVATE OpenCL::OpenCL clquery)

# Using clcompiler, we'll build the .cl file into an .a file!
function(add_kernel_library)
    set(options "")
    set(singles TARGET KERNEL_SOURCE CL_USER_DEVICE_COUNT CL_USER_DEVICE_TYPE)
    set(multiples SOURCES INCLUDES DEFINES LIBRARIES KFLAGS)
    cmake_parse_arguments(
        ARG
        "${options}"
        "${singles}"
        "${multiples}"
        ${ARGN})

    # the .cl file is compiled by the clcompiler to become a header which is then included in the source file for the library
    # Substitute the .cl extension with .h for the generated header file
    string(REPLACE ".cl" ".h" header_file "${ARG_KERNEL_SOURCE}")
    # Replace the source directory with the binary directory for the generated header file
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/source" "${CMAKE_BINARY_DIR}/generated" header_file "${header_file}")
    # If there's KFLAGS, join them into a single string
    if(ARG_KFLAGS)
        set(kflags_str -W "\"${ARG_KFLAGS}\"")
    else()
        unset(kflags_str)
    endif()

    add_custom_command(
        OUTPUT ${header_file}
        COMMAND mkdir -p ${CMAKE_BINARY_DIR}/generated
        COMMAND clcompiler -n -f ${ARG_KERNEL_SOURCE} -t ${ARG_CL_USER_DEVICE_TYPE} -d ${ARG_CL_USER_DEVICE_COUNT} -h ${header_file} ${kflags_str}
        DEPENDS ${ARG_KERNEL_SOURCE} clcompiler
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Compiling ${ARG_KERNEL_SOURCE} to ${header_file}"
    )

    add_executable(${ARG_TARGET} ${ARG_SOURCES})
    target_include_directories(${ARG_TARGET} PUBLIC include ${CMAKE_CURRENT_BINARY_DIR}/generated ${CMAKE_CURRENT_SOURCE_DIR} ${ARG_INCLUDES})
    # Force the include of the generated header file in the library's sources
    target_sources(${ARG_TARGET} PRIVATE ${header_file})
    target_compile_definitions(${ARG_TARGET} PRIVATE ${ARG_DEFINES} CL_USER_DEVICE_COUNT=${ARG_CL_USER_DEVICE_COUNT} CL_USER_DEVICE_TYPE=${ARG_CL_USER_DEVICE_TYPE})
    target_link_libraries(${ARG_TARGET} PRIVATE ${ARG_LIBRARIES})
endfunction()

# Define the OpenCL binaries
add_kernel_library(
    TARGET kernel_spacetime
    SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/source/spacetime.c
    KERNEL_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/source/kernel_spacetime.cl
    CL_USER_DEVICE_COUNT ${CL_USER_DEVICE_COUNT}
    CL_USER_DEVICE_TYPE ${CL_USER_DEVICE_TYPE}
    LIBRARIES OpenCL::OpenCL clenvironment clmath clquery OpenGL::GL GLUT::GLUT
    DEFINES GPGPU_NO_DOUBLE_SUPPORT
    KFLAGS -I${CMAKE_CURRENT_SOURCE_DIR}/include -DGPGPU_NO_DOUBLE_SUPPORT
)

add_kernel_library(
    TARGET kernel_yuv
    SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/source/yuv.c
    KERNEL_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/source/kernel_yuv.cl
    CL_USER_DEVICE_COUNT ${CL_USER_DEVICE_COUNT}
    CL_USER_DEVICE_TYPE ${CL_USER_DEVICE_TYPE}
    LIBRARIES OpenCL::OpenCL clenvironment clmath clquery OpenGL::GL GLUT::GLUT
    DEFINES GPGPU_NO_DOUBLE_SUPPORT
    KFLAGS -I${CMAKE_CURRENT_SOURCE_DIR}/include -DGPGPU_NO_DOUBLE_SUPPORT
)

add_kernel_library(
    TARGET kernel_pid
    SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/source/pid.c
    KERNEL_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/source/kernel_pid.cl
    CL_USER_DEVICE_COUNT ${CL_USER_DEVICE_COUNT}
    CL_USER_DEVICE_TYPE ${CL_USER_DEVICE_TYPE}
    LIBRARIES OpenCL::OpenCL clenvironment clmath clquery OpenGL::GL GLUT::GLUT
    DEFINES GPGPU_NO_DOUBLE_SUPPORT
    KFLAGS -I${CMAKE_CURRENT_SOURCE_DIR}/include -DGPGPU_NO_DOUBLE_SUPPORT
)

# add_executable(monolith ${CMAKE_CURRENT_SOURCE_DIR}/source/monolith.c)
# target_link_libraries(monolith PRIVATE OpenCL::OpenCL)

add_kernel_library(
    TARGET kernel_nbody
    SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/source/nbody.c
    KERNEL_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/source/kernel_nbody.cl
    CL_USER_DEVICE_COUNT ${CL_USER_DEVICE_COUNT}
    CL_USER_DEVICE_TYPE ${CL_USER_DEVICE_TYPE}
    LIBRARIES OpenCL::OpenCL clenvironment clmath clquery OpenGL::GL GLUT::GLUT
    DEFINES GPGPU_NO_DOUBLE_SUPPORT
    KFLAGS -I${CMAKE_CURRENT_SOURCE_DIR}/include -DGPGPU_NO_DOUBLE_SUPPORT
)
