#include "chatservice.hpp"
#include "public.hpp"
#include <muduo/base/Logging.h>
#include <vector>
#include "offlinemessagemodel.hpp"
#include "friendmodel.hpp"
using namespace muduo;
// 提供静态方法，获取唯一实例
ChatService *ChatService::getInstance()
{
    // 定义静态变量类，返回地址
    static ChatService service;
    return &service;
}
// 构造函数定义
ChatService::ChatService()
{
    // 将各种业务处理函数注册进入业务类中的map映射
    // 加入登录业务处理函数
    _msgHandlerMap.insert(std::make_pair(LOGIN_MSG, bind(&ChatService::login, this, _1, _2, _3)));
    // 加入注册业务函数
    _msgHandlerMap.insert(std::make_pair(REG_MSG, bind(&ChatService::reg, this, _1, _2, _3)));
    // 加入一对一业务函数
    _msgHandlerMap.insert(std::make_pair(ONE_CHAT_MSG, bind(&ChatService::oneChat, this, _1, _2, _3)));
    // 加入添加好友业务函数
    _msgHandlerMap.insert(std::make_pair(ADD_FRIEND_MSG, bind(&ChatService::addFriend, this, _1, _2, _3)));
    // 加入创建群组函数
    _msgHandlerMap.insert(std::make_pair(CREATE_GROUP_MSG, bind(&ChatService::createGroup, this, _1, _2, _3)));
    //加入加入群组函数
    _msgHandlerMap.insert(std::make_pair(ADD_GROUP_MSG, bind(&ChatService::addGroup, this, _1, _2, _3)));
    //加入开展群聊函数
    _msgHandlerMap.insert(std::make_pair(GROUP_CHAT_MSG, bind(&ChatService::groupChat, this, _1, _2, _3)));
}
// 处理登录业务,传入用户id与用户密码
void ChatService::login(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    // 打印日志测试
    // LOG_INFO << "TEST LOGIN METHOD";
    int id = js["id"];
    string pwd = js["password"];
    // 传入用户id调用用户模型类查询接口,获取用户实体
    User user = _userModel.query(id);
    // 判断是否登录成功
    if (user.getId() == id && user.getPwd() == pwd)
    {
        if (user.getState() == "online")
        {
            // 该用户已经登录，无法重复登录
            //  返回给前端一个消息响应
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 2;
            response["errmsg"] = "账号已登录，无法重复登录";
            // 通过tcp连接发回响应
            conn->send(response.dump());
            return;
        }
        // 登录成功，设置用户在线状态，同时需要将tcp连接添加到服务类的映射中
        user.setState("online");
        {
            // 使用RAII标准的自动释放互斥锁，并且保证尽量减小互斥锁的粒度
            lock_guard<mutex> lock(_connMutex);
            _userConnMap.insert(make_pair(id, conn));
        }
        // 本地更改用户信息在线、离线状态信息以后，需要同步到数据库
        _userModel.updateState(user);
        // 返回给前端一个消息响应
        json response;
        response["msgid"] = LOGIN_MSG_ACK;
        response["errno"] = 0;
        response["id"] = user.getId(); // 注意插入成功时，将用户id回显给前台
        response["name"] = user.getName();
        // 登录成功以后，检查离线消息表，是否存在离线消息，如需要那么加载该用户的离线消息
        vector<string> msgs = _offlineMsgModel.query(id);
        if (!msgs.empty())
        {
            // 将获取到的离线消息加载进到响应中,读完需要即时删除
            response["offlinemsg"] = msgs;
            _offlineMsgModel.remove(id);
        }
        // 登录成功以后，检查好友列表，加载用户的好友信息
        vector<User> vecUsr = _friendModel.query(id);
        if (!vecUsr.empty())
        {
            vector<string> friends;
            // 遍历原始好友列表封装一个json返回
            for (User &user : vecUsr)
            {
                json js;
                js["id"] = user.getId();
                js["name"] = user.getName();
                js["state"] = user.getState();
                friends.push_back(js.dump());
            }
            response["friends"] = friends;
        }
        // 通过tcp连接发回响应
        conn->send(response.dump());
    }
    else
    {
        // 用户不存在或者密码不正确，登录失败
        json response;
        response["msgid"] = LOGIN_MSG_ACK;
        response["errno"] = 1;
        response["errmsg"] = "用户不存在或密码错误，请重试";
        conn->send(response.dump());
    }

    // 返回响应体
}
// 处理注册业务,客户端将用户注册信息(包含用户名与用户密码)以json形式传入
void ChatService::reg(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    // 打印日志测试
    // LOG_INFO<<"TEST REGISTER METHOD";
    // 调用用户模型内的插入用户方法
    // 需要通过js对象解析出用户名与密码
    string name = js["name"];
    string pwd = js["password"];
    User user;
    user.setUsername(name);
    user.setPwd(pwd);
    bool flag = _userModel.insert(user);
    if (flag)
    {
        // 注册用户成功
        // 返回给前端一个消息响应
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 0;
        response["id"] = user.getId(); // 注意插入成功时，将用户id回显给前台
        // 通过tcp连接发回响应
        conn->send(response.dump());
    }
    else
    {
        // 注册失败
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 1;
        conn->send(response.dump());
    }
}
// 根据网络请求类型，提供消息处理器
MsgHandler ChatService::getHandler(int msgid)
{
    auto it = _msgHandlerMap.find(msgid);
    if (it == _msgHandlerMap.end())
    {
        return [=](const TcpConnectionPtr &, json &, Timestamp)
        {
            LOG_ERROR << "msgid-" << msgid << "-cannot find its handler!";
        };
    }
    else
    {
        return _msgHandlerMap[msgid];
    }
}
// 处理客户端用户异常退出，即下线需要修改用户状态并删掉服务端保存的客户端连接
void ChatService::clientCloseException(const TcpConnectionPtr &conn)
{
    User user;
    {
        // 同一个连接使用同一把锁，与登录时作互斥处理
        lock_guard<mutex> lock(_connMutex);
        // 遍历用户连接映射，找到用户id
        for (auto it = _userConnMap.begin(); it != _userConnMap.end(); it++)
        {
            if (it->second == conn)
            {
                user.setId(it->first);
                // 传入迭代器，删除用户连接
                _userConnMap.erase(it);
                break;
            }
        }
    }
    // 未查询到用户时，不需要修改用户状态
    if (user.getId() != -1)
    {
        // 修改用户用户状态为offline
        user.setState("offline");
        // 传入用户id与用户期望的状态，访问数据库修改用户信息
        _userModel.updateState(user);
    }
}
// 处理一对一聊天业务
// 若对方在线则直接发给对方，若对方不在线，需要访问数据库存储离线消息，一旦用户登录上线以后，由服务器推送给用户
// json中包含，请求类型，来自用户的id,来自用户名，推送的用户id,消息内容本身
void ChatService::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int toid = js["to"].get<int>();
    {
        // 由于需要访问连接信息表，检查对方是否在线，需要上锁
        lock_guard<mutex> lock(_connMutex);
        // 尝试在服务器上寻找对方是否在线
        auto it = _userConnMap.find(toid);
        if (it != _userConnMap.end())
        {
            // 对方在线，直接将消息经服务器推送给对方
            it->second->send(js.dump());
            return;
        }
    }
    // 对方不在线，处理离线消息，将消息发送到离线消息表内
    _offlineMsgModel.insert(toid, js.dump());
}
// 服务器异常退出以后，将业务重置逻辑
void ChatService::reset()
{
    _userModel.resetState();
}
// 添加好友业务
void ChatService::addFriend(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    // 获取用户id,好友id
    int id = js["id"];
    int friendid = js["friendid"];
    // 调用数据层接口,添加好友并返回好友列表给客户端
    _friendModel.insert(id, friendid);
}
// 创建群组业务
void ChatService::createGroup(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    string name = js["groupname"];
    string desc = js["groupdesc"];
    Group group(-1, name, desc);
    if (_groupModel.createGroup(group))
    {
        // 存储群组创建人信息,即创建人为群主
        _groupModel.addGroup(userid, group.getId(), "creator");
    }
}
// 加入群组业务
void ChatService::addGroup(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int groupid = js["groupid"].get<int>();
    int userid = js["id"].get<int>();
    _groupModel.addGroup(userid, groupid, "normal");
}
// 展开群聊业务
void ChatService::groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int groupid = js["groupid"].get<int>();
    int userid = js["id"].get<int>();
    //获取群组包含的所有用户，把消息广播给除了自身以外的每一个用户
    vector<int> ids=_groupModel.queryGroupUsers(userid,groupid);
    lock_guard<mutex> lock(_connMutex);
    for(int id:ids){
        auto it=_userConnMap.find(id);
        //用户在线时
        if(it!=_userConnMap.end()){
            it->second->send(js.dump());
        }else{
        //用户离线时
            _offlineMsgModel.insert(id,js.dump());
        }
    }
}