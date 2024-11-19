#ifndef GROUP_H
#define GROUP_H
#include <string>
using namespace std;
#include <vector>
#include "groupuser.hpp"

//对应群组表实体类
class Group{
public:
    Group(int id=-1,string name="",string desc=""){
        this->id=id;
        this->name=name;
        this->desc=desc;
    }

    void setId(int id){
        this->id=id;
    }
    void setName(string name){
        this->name=name;
    }
    void setDesc(string desc){
        this->desc=desc;
    }

    int getId(){
        return this->id;
    }
    string getName(){
        return this->name;
    }
    string getDesc(){
        return this->desc;
    }
    vector<GroupUser> &getUsers(){
        return this->users;
    }

private:
    int id;
    string name;
    string desc;
    vector<GroupUser> users;//群组包含的所有的用户


};

#endif