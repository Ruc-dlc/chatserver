#ifndef USER_H
#define USER_H
#include <string>
using namespace std;

//定义数据库用户表格对应的实体类
class User{
public:
    User(int id=-1,string name="",string pwd="",string state="offline"){
        this->id=id;
        this->username=name;
        this->password=pwd;
        this->state=state;
    }
    void setId(int id){
        this->id=id;
    }
    void setUsername(string name){
        this->username=name;
    }
    void setPwd(string pwd){
        this->password=pwd;
    }
    void setState(string state){
        this->state=state;
    }
    int getId(){
        return this->id;
    }
    string getName(){
        return this->username;
    }
    string getPwd(){
        return this->password;
    }
    string getState(){
        return this->state;
    }
private:
    int id;
    string username;
    string password;
    string state;//用户状态,离线与在线状态
};


#endif