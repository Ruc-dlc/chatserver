
#指定头文件搜索路径
#include_directories()
#指定库文件搜索路径
#link_directories()

#将指定路径下的源文件名称全部添加到变量中
aux_source_directory(. SRC_LIST)

#传入源文件，生成可执行文件
add_executable(server ${SRC_LIST})
#指定源文件生成可执行文件前需要链接的库
target_link_libraries(server muduo_net muduo_base pthread)
