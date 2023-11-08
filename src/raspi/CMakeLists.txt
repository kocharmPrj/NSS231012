cmake_minimum_required(VERSION 3.0.2)
project(raspi LANGUAGES C CXX)
 
find_package(catkin REQUIRED COMPONENTS message_generation roscpp               geometry_msgs)
find_package(Threads) 
set(CMAKE_C_COMPILER "/usr/bin/gcc-9")
set(CMAKE_CXX_COMPILER "/usr/bin/g++-9")

include_directories(include ${catkin_INCLUDE_DIRS})

add_message_files(FILES Cmd_vel_msg.msg)
generate_messages(DEPENDENCIES geometry_msgs)
 
catkin_package(
  CATKIN_DEPENDS roscpp geometry_msgs
)
#include_directories(${catkin_INCLUDE_DIRS} )

add_executable(cmd_vel ${CMAKE_CURRENT_SOURCE_DIR}/src/raspi.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/socket_pi.cpp) 
add_dependencies(cmd_vel ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(cmd_vel ${catkin_LIBRARIES} pthread)
target_include_directories(cmd_vel PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/hdr)
target_compile_options(cmd_vel PRIVATE -g -Wall) 

  

