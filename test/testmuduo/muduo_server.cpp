//muduo网络库提供了两个主要的类
//tcpserver,tcpclient
//底层是epoll+线程池，有效的把网络IO与业务代码进行区分

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <iostream>
#include <functional>
using namespace std;
using namespace muduo;
using namespace muduo::net;
using namespace placeholders;
/*基于muduo网络库开发服务器程序
    1.组合tcpserver对象
    2.创建事件循环eventloop指针
    3.明确初始化tcpserver构造需要的参数，进一步写出服务器类的构造参数
    4.在当前服务器类的构造函数中注册必要的回调函数，通常是需要处理连接创建与断开的回调和处理读写事件的回调
    5.设置合适的线程数目，muduo自适应分配io线程与worker线程，前者负责监听连接，后者负责读写事件
*/
class ChatServer{
public:
    ChatServer(EventLoop* loop, //事件循环，相当于reactor反应堆
            const InetAddress& listenAddr, //包含服务器ip与port信息
            const string& nameArg)//服务器名称
            :_server(loop,listenAddr,nameArg),_loop(loop){
                //针对Tcpserver对象进行两个回调函数的注册
                //给服务器注册用于---用户连接的创建与断开---的回调函数
                _server.setConnectionCallback(std::bind(&ChatServer::onConnection,this,_1));
                //给服务器注册用于---用户读写事件处理---的回调函数
                _server.setMessageCallback(std::bind(&ChatServer::onMessage,this,_1,_2,_3));
                //设置服务器端的线程数量，包含两个工作，一个负责用户的连接，一个负责用户的读写
                _server.setThreadNum(4);//一个线程负责连接，三个负责用户的读写
            }
            //开启事件循环
    void start(){
        _server.start();
    }
private:
    //自定义连接建立与断开时的回调函数
    void onConnection(const TcpConnectionPtr& conn){
        if(conn->connected()){
            //客户端建立连接成功,打印远端ip与端口号连接到本地
            cout<<conn->peerAddress().toIpPort()<<"->"<<conn->localAddress().toIpPort()<<"start online"<<endl;
        }else{
            //客户端断开连接，打印远端ip与端口号连接到本地
            cout<<conn->peerAddress().toIpPort()<<"->"<<conn->localAddress().toIpPort()<<"finish offline"<<endl;
            conn->shutdown();//本链接断开，系统回收文件套接字
        }
    }
    //自定义用户读写事件处理的回调函数
    void onMessage(const TcpConnectionPtr& conn, //指向连接的指针
                            Buffer* buffer,        //缓冲区
                            Timestamp time){     //接收到数据的时间
        //使用buffer接收数据，并echo回显
        string buf=buffer->retrieveAllAsString();
        //打印服务端接收到的数据
        cout<<"recv data:"<<buf<<"time"<<time.toString()<<endl;
        //返回给客户端相同的数据
        conn->send(buf);
    }
    //定义server对象，eventloop指针
    TcpServer _server;
    EventLoop *_loop;
};

int main(){
    EventLoop loop;
    InetAddress addr("127.0.0.1",6000);
    ChatServer server(&loop,addr,"mychatserver");
    //服务器开启监听，事件循环开始循环
    server.start();
    loop.loop();//以阻塞方式调用epoll_wait，等待新用户连接与读写事件
    return 0;
}