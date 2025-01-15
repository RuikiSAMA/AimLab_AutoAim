cmake_minimum_required(VERSION 3.10)
project(OpenCV_AimLab)
 
# 设置C++标准
set(CMAKE_CXX_STANDARD 11)

# 使用 file(GLOB ...) 查找所有的 .cpp 和 .h 文件
file(GLOB SOURCE_CODE ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h )

#设置 OpenCV_DIR 为 find_packge 指定查找路径 
set(OpenCV_DIR C:/OpenCV/build)

# 查找OpenCV库
find_package(OpenCV REQUIRED) 
 
# 添加可执行文件
add_executable(OpenCV_AimLab ${SOURCE_CODE})

# 链接OpenCV库
target_link_libraries(OpenCV_AimLab Dwmapi ${OpenCV_LIBS} ${OpenCV_LIBRARIES})
