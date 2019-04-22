cmake_minimum_required(VERSION 2.8.12)
if(NOT CMAKE_VERSION VERSION_LESS 3.0)
    cmake_policy(SET CMP0048 NEW)
endif()
project(imagetool)

set(OpenCV_DIR "~/opencv/opencv/build")

# Find requirements
find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
  image_transport
  sensor_msgs
  cv_bridge
  message_generation
)
if(NOT fastcdr_FOUND)
    find_package(fastcdr REQUIRED)
endif()

if(NOT fastrtps_FOUND)
find_package(fastrtps REQUIRED)
endif()
find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED COMPONENTS
  system
  thread
)

include(CheckCXXCompilerFlag)
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG OR
        CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    check_cxx_compiler_flag(--std=c++11 SUPPORTS_CXX11)
    if(SUPPORTS_CXX11)
        add_compile_options(--std=c++11 )
    else()
        message(FATAL_ERROR "Compiler doesn't support C++11")
    endif()
endif()

add_message_files(
  FILES
  image_sensor.msg
#   Message1.msg
#   Message2.msg
 )

 generate_messages(
   DEPENDENCIES
   sensor_msgs
   std_msgs  # Or other packages containing msgs
 )


catkin_package(
INCLUDE_DIRS include
LIBRARIES imagetool
CATKIN_DEPENDS roscpp rospy std_msgs message_runtime
DEPENDS system_lib
)

include_directories(
  include
	${catkin_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
)

#add_executable(pub
#	src/ImageToolPub.cpp
#	src/DDSPublisher.cxx
#	src/MsgBufferType.cxx
#)
#add_executable(sub
#	src/ImageToolSub.cpp
#	src/DDSSubscriber.cxx
#	src/MsgBufferType.cxx
#)
  
#target_link_libraries(pub
#		fastrtps
#		fastcdr
#		${OpenCV_LIBS}
#		${Boost_LIBRARIES}
#		${Boost_THREAD_LIBRARY}
#		${Boost_SYSTEM_LIBRARY}
#		${catkin_LIBRARIES}
#)
#target_link_libraries(sub
#		fastrtps
#		fastcdr
#		${OpenCV_LIBS}
#		${Boost_LIBRARIES}
#		${Boost_THREAD_LIBRARY}
#		${Boost_SYSTEM_LIBRARY}
#		${catkin_LIBRARIES}
#)
