#include <iostream>
#include <signal.h>
#include "chatserver.hpp"
#include "chatservice.hpp"
using namespace std;

//处理服务器ctrl+c结束后，用户状态信息重置
void resetHandler(int){
    ChatService::getInstance()->reset();
    exit(EXIT_SUCCESS);
}

int main(){
    //捕获用户异常退出时，调用重置user状态函数
    signal(SIGINT,resetHandler);
    //事件循环
    EventLoop loop;
    //服务器配置信息，端口号与ip地址
    InetAddress addr("127.0.0.1",6000);
    //创建服务器容器，传入服务器配置与事件循环
    ChatServer server(&loop,addr,"charserver");
    //分别启动容器与事件循环
    server.start();
    loop.loop();
    return 0;
}