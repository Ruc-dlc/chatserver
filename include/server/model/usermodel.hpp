#ifndef USERMODEL_H
#define USERMODEL_H
#include "user.hpp"
// 与数据库进行交互模型类
class UserModel
{
public:
    // 新增一名用户接口
    bool insert(User &user);
    // 查询一名用户信息接口
    User query(int id);
    // 修改一名用户离线在线状态信息
    bool updateState(User &user);
    //重置用户的状态信息
    void resetState();
private:
};

#endif