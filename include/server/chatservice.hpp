#ifndef _CHATSERVICE_H_
#define _CHATSERVICE_H_
#include <muduo/net/TcpConnection.h>
#include <unordered_map>
#include <functional>
#include <mutex>
#include "json.hpp"
#include "usermodel.hpp"
#include "offlinemessagemodel.hpp"
#include "friendmodel.hpp"
#include "groupmodel.hpp"
using namespace std;
using namespace muduo;
using namespace muduo::net;
using json=nlohmann::json;

//定义网络请求类型对应的处理器类型，设置为包装器类型，接收各类函数的绑定
using MsgHandler=function<void(const TcpConnectionPtr&,json&,Timestamp)>;

//聊天服务器业务类，设计为单例模式即可，只需要一个对象
class ChatService{
public:
    //提供静态方法，获取唯一实例
    static ChatService* getInstance();
    //定义一系列对外提供的服务接口，并在构造器中，将服务接口注册进入map映射中，供controller层快速调用
    //处理登录业务
    void login(const TcpConnectionPtr &conn,json &js,Timestamp time);
    //处理注册业务
    void reg(const TcpConnectionPtr &conn,json &js,Timestamp time);
    //处理一对一聊天业务
    void oneChat(const TcpConnectionPtr &conn,json &js,Timestamp time);
    //添加好友业务
    void addFriend(const TcpConnectionPtr &conn,json &js,Timestamp time);
    //创建群组业务
    void createGroup(const TcpConnectionPtr &conn,json &js,Timestamp time);
    //加入群组业务
    void addGroup(const TcpConnectionPtr &conn,json &js,Timestamp time);
    //展开群聊业务
    void groupChat(const TcpConnectionPtr &conn,json &js,Timestamp time);
    //服务器异常退出以后，将业务重置逻辑
    void reset();
    //根据网络请求类型，提供消息处理器
    MsgHandler getHandler(int msgid);
    //处理客户端用户异常退出，即下线需要修改用户状态并删掉服务端保存的客户端连接
    void clientCloseException(const TcpConnectionPtr&);

private:
    ChatService();
    //定义用户id与tcp连接的映射，给用户推送接收到的消息
    unordered_map<int,TcpConnectionPtr> _userConnMap;
    //定义消息类型与对应的业务处理方法映射
    unordered_map<int,MsgHandler> _msgHandlerMap;
    //定义互斥锁，使多线程互斥访问_userConnMap,保证线程安全的添加用户tcp连接
    mutex _connMutex;
    //引用用户模型与数据层进行交互
    UserModel _userModel;
    //引用离线消息模型与数据层进行交互
    OfflineMsgModel _offlineMsgModel;
    //引用好友模型与数据层交互
    FriendModel _friendModel;
    GroupModel _groupModel;
};



#endif