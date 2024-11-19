#ifndef GROUPUSER_H
#define GROUPUSER_H
#include "user.hpp"



//对应群组用户中间表,主要在用户信息的基础上添加一个角色信息
class GroupUser:public User{
public:
    void setRole(string role){
        this->role=role;
    }
    string getRole(){
        return this->role;
    }
private:
    string role;
};

#endif