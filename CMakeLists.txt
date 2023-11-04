cmake_minimum_required(VERSION 3.2.0)
project(NSS231012 LANGUAGES C CXX)


set(CMAKE_C_COMPILER "/usr/bin/gcc-11")
set(CMAKE_CXX_COMPILER "/usr/bin/g++-11")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output)
set(src ${CMAKE_SOURCE_DIR}/src)

#find_package(OpenCV REQUIRED)

add_executable(NSS231012
                ${src}/main.cpp ${src}/socket_self.cpp)
# Add .cpp files like "(NSS231012 ${src}/main.cpp ${src}/etc.cpp})


target_compile_options(NSS231012 PRIVATE -g -Wall -std=c++11)
# target_compile_option(NSS231012 PRIVATE -O3 -Wall -std=c++11)
# the line above is for RELEASE VER


target_include_directories(NSS231012 PUBLIC ${CMAKE_SOURCE_DIR}/src/hdr)
#target_include_directories(NSS231012 PUBLIC ${CMAKE_SOURCE_DIR}/lib/opencv/build/include ${CMAKE_SOURCE_DIR}/src/hdr)


#target_link_libraries(vscode_cmake PUBLIC ${CMAKE_SOURCE_DIR}/lib/opencv/build/x64/vc16/lib/opencv_world480.lib)
# MUST CHANGE ABOUVE CODE FOR opencv470
# (TODO) Download and unpack sources because of small capacity of git remote to upload OPENCV LIB
# (LINK) https://github.com/opencv/opencv/tree/4.7.0
# git clone https://github.com/opencv/opencv.git // in lib folder
# cd opencv
# git checkout 4.7.0
# && in opencv root folder, mkdir build, cd build, cmake .. and make -j


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)


