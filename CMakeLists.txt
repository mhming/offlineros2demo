cmake_minimum_required(VERSION 3.5)
project(offlineros2demo)

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)

# 设置源文件根目录，并设置别名为SRC_DIR
set(SRC_DIR ./src)

# 声明三方库位置路径
link_directories(./libs/x64)

set(LIBRARIES msc rt dl pthread asound stdc++ offline_record_lib cjson)

include_directories(./include)

set(COM_SRC
        ${SRC_DIR}/asr_offline_record_sample.c
        ${SRC_DIR}/com_test.c
        ${SRC_DIR}/linuxrec.c
        ${SRC_DIR}/record.c
        ${SRC_DIR}/speech_recognizer.c
        ${SRC_DIR}/main.c)

# 生成可执行文件
add_executable(${PROJECT_NAME} ${COM_SRC})
# 链接库
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})

install(TARGETS  ${PROJECT_NAME} DESTINATION lib/${PROJECT_NAME})

ament_package()

# rm -fr build/ install/ log/
# colcon build --packages-select offlineros2demo