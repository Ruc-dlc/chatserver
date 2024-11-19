#ifndef OFFLINEMESSAGEMODEL_H
#define OFFLINEMESSAGEMODEL_H
#include <vector>
#include <string>
using namespace std;


//封装离线消息表的操作接口方法
class OfflineMsgModel{
public:
    //用户登录上线后，查询离线消息,一个用户可能含有多条离线消息
    vector<string> query(int userid); 
    //加载完离线消息后，将离线消息删除
    void remove(int userid);
    //新增一条离线消息
    void insert(int userid,string msg);

};


#endif