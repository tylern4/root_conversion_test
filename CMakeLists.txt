cmake_minimum_required(VERSION 3.5)
project(validate C CXX Fortran Java)
set(VERSION 1.0.0)

option(BUILD_FPIC "Build with -fPIC" ON)
option(BUILD_JAVA "Build with JAVA" OFF)

if(BUILD_FPIC)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

# Add modules to find find packages
set(CMAKE_MODULE_PATH
   ${CMAKE_MODULE_PATH}
   ${PROJECT_SOURCE_DIR}/cmake)

# Add root support
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
find_package(ROOT REQUIRED COMPONENTS RIO Net)
include(${ROOT_USE_FILE})
set(CMAKE_C_FLAGS "-O3")
set(CMAKE_CXX_FLAGS "-O3 ${ROOT_CXX_FLAGS}")

# Find the lz4 package
# Hopefully in the future if not found download and build
add_subdirectory(lz4/contrib/cmake_unofficial)
set(LZ4_LIBRARY lz4_static)
set(LZ4_INCLUDE_DIR lz4/lib)
include_directories(${LZ4_INCLUDE_DIR})
add_definitions(-D__LZ4__)

# Add directory for hipo4 library
include_directories(hipo/hipo4)
set(hipo4_srcs
  hipo/hipo4/bank.cpp
  hipo/hipo4/dictionary.cpp
  hipo/hipo4/event.cpp
  hipo/hipo4/reader.cpp
  hipo/hipo4/record.cpp
  hipo/hipo4/recordbuilder.cpp
  hipo/hipo4/utils.cpp
  hipo/hipo4/wrapper.cpp
  hipo/hipo4/writer.cpp
  )

add_library(hipo4_objlib OBJECT ${hipo4_srcs})
set_property(TARGET hipo4_objlib PROPERTY POSITION_INDEPENDENT_CODE ON)
add_library(hipocpp4 STATIC $<TARGET_OBJECTS:hipo4_objlib>)
target_link_libraries(hipocpp4 PUBLIC ${LZ4_LIBRARY})

include_directories(src)

file(GLOB EXAMPLES src/*.cxx )
foreach(exe ${EXAMPLES} )
    get_filename_component(exename ${exe} NAME)
    string( REPLACE ".cxx" "" exename ${exename} )
    add_executable( ${exename} ${exe} )
    target_link_libraries(${exename}
      PRIVATE ${libname}
      PUBLIC hipocpp4
      PUBLIC ${ROOT_LIBRARIES})
    add_dependencies(${exename} hipocpp4)
endforeach( exe ${EXAMPLES} )


if(BUILD_JAVA)
  find_package(Java)
  if(${Java_FOUND})
    include(UseJava)
    set(CMAKE_JAVA_COMPILE_FLAGS "-source" "1.8" "-target" "1.8")
    set(CMAKE_JAVA_INCLUDE_PATH $ENV{COATJAVA_LIB})
    add_jar(validate validate/validate.java)
  endif()
endif()