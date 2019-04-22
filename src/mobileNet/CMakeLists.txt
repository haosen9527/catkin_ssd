cmake_minimum_required(VERSION 2.8.3)
project(mobileNet)

## Compile as C++11, supported in ROS Kinetic and newer
add_compile_options(-std=c++11 -O2)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
)
catkin_package(
  INCLUDE_DIRS include
  LIBRARIES mobileNet
#  CATKIN_DEPENDS roscpp rospy
#  DEPENDS system_lib
)

###########
## Build ##
###########

## Specify additional locations of header files
## Your package locations should be listed before other locations
include_directories(
  "include"
  ${catkin_INCLUDE_DIRS}
  "/usr/local/include/eigen3"
  "~/tensorflow_cxx/tensorflow_source/tensorflow"
  "~/tensorflow_cxx/tensorflow_source/tensorflow/bazel-genfiles"
  "~/tensorflow_cxx/tensorflow_source/tensorflow/tensorflow/contrib/makefile/gen/protobuf/include"
)
link_directories("~/tensorflow_cxx/tensorflow_source/tensorflow/bazel-bin/tensorflow")

## Declare a C++ library
#add_library(${PROJECT_NAME}
#  src/mobileNet.cpp
#)

#target_link_libraries(${PROJECT_NAME}
#  tensorflow_cc
#  tensorflow_framework
#  ${catkin_LIBRARIES}
#)

