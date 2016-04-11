CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

## ============
## = Policies =
## ============

SET(
  policies
  CMP0015
  CMP0020
  CMP0042
  )
FOREACH(policy ${policies})
  IF(POLICY ${policy})
    CMAKE_POLICY(SET ${policy} NEW)
  ENDIF(POLICY ${policy})
ENDFOREACH(policy)

## ================
## = Project name =
## ================

PROJECT(snakes)

## ========================
## = Packages and options =
## ========================
IF(NOT MSVC)
  INCLUDE(CheckCXXCompilerFlag)
  CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
  IF(COMPILER_SUPPORTS_CXX11)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  ELSE(COMPILER_SUPPORTS_CXX11)
    CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
    IF(COMPILER_SUPPORTS_CXX0X)
      SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
    ELSE(COMPILER_SUPPORTS_CXX0X)
      MESSAGE(
        FATAL_ERROR
        "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support."
        )
    ENDIF(COMPILER_SUPPORTS_CXX0X)
  ENDIF(COMPILER_SUPPORTS_CXX11)
ENDIF(NOT MSVC)

## ================================================
## = Do not allow to build inside the source tree =
## ================================================

IF(PROJECT_BINARY_DIR STREQUAL ${PROJECT_SOURCE_DIR})
  MESSAGE(FATAL_ERROR "Building in the source tree is not allowed.")
ENDIF(PROJECT_BINARY_DIR STREQUAL ${PROJECT_SOURCE_DIR})

## =====================================
## = Where to put executables and libs =
## =====================================

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR})
MARK_AS_ADVANCED(
  CMAKE_BACKWARDS_COMPATIBILITY
  EXECUTABLE_OUTPUT_PATH
  LIBRARY_OUTPUT_PATH
  )

# ============
# = Find VTK =
# ============
FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE})

## =======================
## = Include directories =
## =======================

INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_BINARY_DIR}
  )

## =====================
## = Compilation rules =
## =====================

#ADD_EXECUTABLE(snakesClient snakesClient.cxx point.cxx snakeFilter.cxx)
#TARGET_LINK_LIBRARIES(snakesClient ${VTK_LIBRARIES})

#ADD_EXECUTABLE(mouseSnakesC snakesClientMouse.cxx point.cxx snakeFilter.cxx)
#TARGET_LINK_LIBRARIES(mouseSnakesC ${VTK_LIBRARIES})

ADD_EXECUTABLE(snakeClientMouse snakesClientMouse2.cxx point.cxx snakeFilter.cxx)
TARGET_LINK_LIBRARIES(snakeClientMouse ${VTK_LIBRARIES})



## eof - $RCSfile$
