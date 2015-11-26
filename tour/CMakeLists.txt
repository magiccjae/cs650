cmake_minimum_required(VERSION 2.8)
project( tour )
find_package( OpenCV REQUIRED )
add_executable( tour main.cpp )

target_link_libraries( tour ${OpenCV_LIBS} )


