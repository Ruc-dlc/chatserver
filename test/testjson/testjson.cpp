#include "json.hpp"   //第三方德国作者的json序列化库导入头文件，即可使用
using json=nlohmann::json;

#include <iostream>
#include <vector>
#include <map>

//json序列化方式
/*
    总体而言，json序列化库支持多种value类型，可以是stl容器，也可以是json对象本身进行嵌套
    在网络上进行传输时，需要先序列化为json字符串才能进行传输
*/
//json测试用例1：可以像使用stl一样使用json序列化库
void func1(){
    //类似无序哈希map，形成键值对数组
    json js;
    js["msg-type"]=2;
    js["from"]="zhang san";
    js["to"]="li si";
    js["msg"]="hello json serialize";
    //直接打印json对象，由于重载了运算符，可以按内容打印数组
    std::cout<<js<<std::endl;
    //转为json字符串以后，可以在网络上进行传输
    std::string buf=js.dump();
    std::cout<<buf.c_str()<<std::endl;
}
//json测试用例2：value可以是更复杂的数据类型，甚至是json对象类型
void func2(){
    //类似无序哈希map，形成键值对数组
    json js;
    js["id"]={1,2,3,4,5};
    js["from"]="zhang san";
    js["msg"]["zhangsan"]="hello json serialize";
    js["msg"]["lisi"]="hello protobuf serialize";
    //上面两句等同于
    js["msg"]={{"zhangsan", "hello"},{"lisi","bye"}};
    //直接打印json对象，由于重载了运算符，可以按内容打印数组
    std::cout<<js<<std::endl;
    //转为json字符串以后，可以在网络上进行传输
    std::string buf=js.dump();
    std::cout<<buf.c_str()<<std::endl;
}
//json测试用例3：直接给json对象添加stl容器
void func3(){
    json js;
    //直接给js对象添加一个数组容器
    std::vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    js["lsit"]=vec;
    //给js对象添加一个map容器
    std::map<int,std::string> m;
    m.insert(std::make_pair(1,"huashan"));
    m.insert(std::make_pair(2,"taishan"));
    m.insert(std::make_pair(3,"huangshan"));
    js["path"]=m;
    //直接打印json对象，由于重载了运算符，可以按内容打印数组
    std::cout<<js<<std::endl;
    //转为json字符串以后，可以在网络上进行传输
    std::string buf=js.dump();
    std::cout<<buf.c_str()<<std::endl;
}

//json反序列化方式
std::string func4(){
    //json对象先序列化为json字符串以后发送到网络上，然后模拟接收网络上的json字符串，然后再反序列化得到json对象
    json js;
    js["msg-type"]=2;
    js["from"]="zhang san";
    js["to"]="li si";
    js["msg"]="hello json serialize";
    return js.dump(); 
}
void func5(){
    //使用json::parse进行反序列化
    //接收网络上发来的json字符串
    std::string recv=func4();
    //反序列化得到json对象
    auto jsbuf=json::parse(recv);
    //尝试打印结果
    std::cout<<jsbuf["msg"]<<std::endl;
    std::cout<<jsbuf["from"]<<std::endl;
    std::cout<<jsbuf["to"]<<std::endl;
}
int main(){
    //func1();
    //func2();
    //func3();
    func5();
    return 0;
}