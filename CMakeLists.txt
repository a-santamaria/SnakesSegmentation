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

# Find VTK
FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE})

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

## ==============================
## = Subdirs containing headers =
## ==============================

INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_BINARY_DIR}
  )

## =====================
## = Compilation rules =
## =====================

ADD_EXECUTABLE(snakesClient snakesClient.cxx point.cxx snakes.cxx)
TARGET_LINK_LIBRARIES(snakesClient ${VTK_LIBRARIES})

ADD_EXECUTABLE(mouseSnakesC snakesClientMouse.cxx point.cxx snakes.cxx)
TARGET_LINK_LIBRARIES(mouseSnakesC ${VTK_LIBRARIES})



## eof - $RCSfile$
