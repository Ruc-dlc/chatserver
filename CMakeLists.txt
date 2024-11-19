#根目录下的CMakeLists.txt一般存放公有的部分，主导总体项目构建规则
cmake_minimum_required(VERSION 3.0)
project(chat)

#设置编译选项，通过set命令
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)

#设置可执行文件存放路径在工程根目录下面的bin文件夹中
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#设置头文件搜索路径
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/server)
include_directories(${PROJECT_SOURCE_DIR}/include/server/db)
include_directories(${PROJECT_SOURCE_DIR}/include/server/model)
include_directories(${PROJECT_SOURCE_DIR}/include/client)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty)

#添加子目录，递归寻找CMakeLists.txt构建项目
add_subdirectory(src)