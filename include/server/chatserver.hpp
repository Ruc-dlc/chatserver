#ifndef _CHATSERVER_H_
#define _CHATSERVER_H_

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <functional>
#include <iostream>
using namespace muduo;
using namespace muduo::net;

//定义服务器聚类
class ChatServer{
public:
    ChatServer(EventLoop* loop,
            const InetAddress& listenAddr,
            const string& nameArg);
    //服务器启动接口
    void start();

private:
    //声明链接断开相关的回调函数逻辑
    void onConnection(const TcpConnectionPtr&);

    //声明事件读写的回调函数执行逻辑
    void onMessage(const TcpConnectionPtr&,//链接指针
                            Buffer*,//缓冲区
                            Timestamp);//回调时间戳
    //server容器与事件循环指针
    TcpServer _server;
    EventLoop *_loop;
};


#endif