####################################
# General project definition
####################################

CMAKE_MINIMUM_REQUIRED(VERSION 3.5.2 FATAL_ERROR)

# Set cmake policy by version: https://cmake.org/cmake/help/latest/manual/cmake-policies.7.html
if(${CMAKE_VERSION} VERSION_LESS 3.12)
  cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else()
  cmake_policy(VERSION 3.12)
endif()

# Define project
project(GBT_IC
  VERSION 0.0.1
  DESCRIPTION "O2 low-level GTB-IC communication"
  LANGUAGES CXX
  )

set(USE_SYSTEM_ALF FALSE)

# Add compiler flags for warnings and debug symbols
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")

# Set fPIC for all targets
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Set CMAKE_INSTALL_LIBDIR explicitly to lib (to avoid lib64 on CC7)
set(CMAKE_INSTALL_LIBDIR lib)

# Set the default build type to "RelWithDebInfo"
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE "RelWithDebInfo"
    CACHE
    STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel Coverage."
    FORCE
  )
endif()


####################################
# Dependencies
####################################

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

find_package(Common REQUIRED)
find_package(InfoLogger REQUIRED)
find_package(ReadoutCard REQUIRED)
find_package(LLA REQUIRED)
if(USE_SYSTEM_ALF)
  find_package(ALF REQUIRED)
else()
  set(ALF_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/ALF/include)
endif()

####################################
# Executables
####################################

set(LIB_SRCS
  src/Sca/hdlc_core.cpp
  src/Sca/hdlc_alf.cpp
  src/Sca/cern/hdlc_cern_me.cpp
  src/Sca/gbt_sca_i2c.cpp
  src/Sca/sampa.cpp
  src/Sca/fec_revision.cpp
  src/Sca/ds_fec.cpp
  src/decode/decode.cpp
)
if(NOT USE_SYSTEM_ALF)
  list(APPEND LIB_SRCS
    ALF/src/ScBase.cxx
    ALF/src/Ic.cxx
    ALF/src/Sca.cxx
    ALF/src/Lla.cxx
  )
endif()


set(EXE_SRCS
  gbt-config.cpp
  ds-config.cpp
  cru-decode.cpp
  cru-decode-ul.cpp
)

set(EXE_NAMES
  gbt-config
  ds-config
  cru-decode
  cru-decode-ul
)

set(EXE_LIBS
  AliceO2::Common
  AliceO2::InfoLogger
  AliceO2::ReadoutCard
  o2::LLA
)
if(USE_SYSTEM_ALF)
  list(APPEND EXE_LIBS o2::ALF)
endif()

list(LENGTH EXE_SRCS count)
math(EXPR count "${count}-1")
foreach(i RANGE ${count})
  list(GET EXE_SRCS ${i} src)
  list(GET EXE_NAMES ${i} name)
  add_executable(${name} apps/${src} ${LIB_SRCS})
  target_include_directories(${name}
    PUBLIC
      ${Common_INCLUDE_DIRS}
      ${InfoLogger_INCLUDE_DIRS}
      ${ReadoutCard_INCLUDE_DIRS}
      ${ALF_INCLUDE_DIRS}
    PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/src
  )
  target_link_libraries(${name}
    PUBLIC
      ${EXE_LIBS}
  )
  # Use C++17
  target_compile_features(${name} PUBLIC cxx_std_17)
endforeach()


####################################
# Install
####################################
