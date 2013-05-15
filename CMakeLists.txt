########################################################################
# Prevent in-tree builds
########################################################################
IF(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    MESSAGE(FATAL_ERROR "Prevented in-tree built. This is bad practice.")
ENDIF(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})

########################################################################
# Project setup
########################################################################
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(SCL)

LIST(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
FIND_PACKAGE(YAML-CPP REQUIRED)
FIND_PACKAGE(ZeroMQ REQUIRED)

########################################################################
# Add the subdirectories
########################################################################
ADD_SUBDIRECTORY(thirdparty)
ADD_SUBDIRECTORY(src)
