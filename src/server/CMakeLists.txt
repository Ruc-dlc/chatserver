#指定生成可执行文件需要的源文件
aux_source_directory(. SRC_LIST)
aux_source_directory(./db DB_LIST)
aux_source_directory(./model MODEL_LIST)
#指定生成可执行文件需要的源文件
add_executable(ChatServer ${SRC_LIST} ${DB_LIST} ${MODEL_LIST})


#指定编译后生成可执行文件之前需要链接的库
target_link_libraries(ChatServer muduo_net muduo_base mysqlclient pthread)