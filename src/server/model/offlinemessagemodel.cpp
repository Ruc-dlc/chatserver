#include "offlinemessagemodel.hpp"
#include "db.hpp"



// 用户登录上线后，查询离线消息,一个用户可能含有多条离线消息
vector<string> OfflineMsgModel::query(int userid){
     //装填sql语句
    char sql[128]={0};
    sprintf(sql,"select message from offlinemessage where userid=%d",userid);
    vector<string> msgs;
    //获取连接，开始执行sql语句
    MySQL mysql;
    if(mysql.connect()){
        //连接建立成功
        MYSQL_RES *rows=mysql.query(sql);//查询结果默认为多行多列
        if(rows){
            //采取逐行获取记录的方式,按照0，1，2，3的排列描述字段
            MYSQL_ROW row;
            //全局方法获取查询结果表的每一行
            while((row=mysql_fetch_row(rows))!=nullptr){
                msgs.push_back(row[0]);
            }
            //释放查询结果表
            mysql_free_result(rows);
        }
    }
    return msgs;
}
// 加载完离线消息后，将离线消息删除
void OfflineMsgModel::remove(int userid){
    //装填sql语句
    char sql[128]={0};
    sprintf(sql,"delete from offlinemessage where userid=%d",userid);
    //获取连接，开始执行sql语句
    MySQL mysql;
    if(mysql.connect()){
        //连接建立成功
        mysql.update(sql);
    }
}
// 新增一条离线消息
void OfflineMsgModel::insert(int userid, string msg){
    //装填sql语句
    char sql[128]={0};
    sprintf(sql,"insert into offlinemessage values(%d,'%s')",userid,msg.c_str());
    //获取连接，开始执行sql语句
    MySQL mysql;
    if(mysql.connect()){
        //连接建立成功
        mysql.update(sql);
    }
}