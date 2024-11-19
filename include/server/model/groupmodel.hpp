#ifndef GROUPMODEL_H
#define GROUPMODEL_H
#include "group.hpp"
#include <vector>
#include <string>
using namespace std;
//与群组表合进行交互
class GroupModel{
public:
    //创建群组
    bool createGroup(Group &group);
    //加入群组
    void addGroup(int userid,int groupid,string role);
    //查询用户所在的所有群组信息
    vector<Group> queryGroups(int userid);
    //根据groupid查询群组用户中间表，查询该组包含的所有用户,需要去除登录的用户本身
    vector<int> queryGroupUsers(int userid,int groupid);
};

#endif