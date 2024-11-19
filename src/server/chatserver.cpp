#include "chatserver.hpp"
#include <functional>
#include "json.hpp"
#include <string>
#include <iostream>
#include "chatservice.hpp"
using namespace std;
using namespace placeholders;
using json = nlohmann::json;
// 定义构造函数
ChatServer::ChatServer(EventLoop *loop,
                       const InetAddress &listenAddr,
                       const string &nameArg) : _server(loop, listenAddr, nameArg), _loop(loop)
{
    // 注册回调函数
    _server.setConnectionCallback(bind(&ChatServer::onConnection, this, _1));
    _server.setMessageCallback(bind(&ChatServer::onMessage, this, _1, _2, _3));

    // 设置线程数量，一个负责io链接，其余负责处理io读写事件
    _server.setThreadNum(4);
}
// 服务器启动接口
void ChatServer::start()
{
    _server.start();
}
// 定义链接断开相关的回调函数逻辑
void ChatServer::onConnection(const TcpConnectionPtr &conn)
{
    // 链接断开，处理客户端用户断开前需要做到事，例如修改用户状态，释放套接字资源,
    if (!conn->connected())
    {
        ChatService::getInstance()->clientCloseException(conn);
        conn->shutdown();
    }
}

// 定义事件读写的回调函数执行逻辑
void ChatServer::onMessage(const TcpConnectionPtr &conn, // 链接指针
                           Buffer *buffer,               // 缓冲区
                           Timestamp time)               // 回调时间戳
{
    // 从网络链接中收到序列化的数据,作为string类型来接收
    string buf = buffer->retrieveAllAsString();
    // 反序列化数据得到json对象
    json js = json::parse(buf);
    // 为了解耦网络模块代码与业务模块代码
    // 通过js["msgid"]获取相应的handler,再将网络这边的参数，例如conn,js,time传给handler，进行业务处理
    //解析网络请求中的json信息，获取请求类型，转接处理方式
    //获取json对象中的msgid
    int msgid=js["msgid"].get<int>();
    //从业务实体类中的map获取对应的handler方法
    auto handler=ChatService::getInstance()->getHandler(msgid);
    //根据网络请求消息类型，去回调业务类中注册的各种对应的handler方法，从而实现网路与业务进行解耦
    handler(conn,js,time);
}