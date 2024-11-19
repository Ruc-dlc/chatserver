#include "usermodel.hpp"
#include "db.hpp"
// 新增一名用户接口
bool UserModel::insert(User &user)
{
    // 定义交互sql语句
    char sql[128] = {0};
    // 插入一名用户
    sprintf(sql, "insert into user (name,password,state) values ('%s','%s','%s')", user.getName().c_str(), user.getPwd().c_str(), user.getState().c_str());
    MySQL mysql;
    // 连接数据库并判断连接是否成功
    if (mysql.connect())
    {
        // 连接成功，执行sql语句增加一名用户
        if (mysql.update(sql))
        {
            // 取回表格自增的用户id
            user.setId(mysql_insert_id(mysql.getConnection()));
            return true; // 插入成功
        }
    }
    // 插入失败
    return false;
}
// 查询一名用户信息接口
User UserModel::query(int id)
{
    // 组装sql语句
    char sql[128] = {0};
    sprintf(sql, "select * from user where id = %d", id);
    // 获取数据库实体
    MySQL mysql;
    if (mysql.connect())
    {
        // 连接成功,开始执行sql查询语句,默认为查询出一张表，多行多列
        MYSQL_RES *res = mysql.query(sql);
        // 已知传入的id唯一，所以只有一行结果
        if (res != nullptr)
        {
            MYSQL_ROW row = mysql_fetch_row(res);
            if (row != nullptr)
            {
                User user;
                user.setId(atoi(row[0]));
                user.setUsername(row[1]);
                user.setPwd(row[2]);
                user.setState(row[3]);
                mysql_free_result(res);
                return user;
            }
        }
    }

    // 创建连接失败，或者用户查询无结果（传入的用户id不存在）
    return User();
}
// 修改一名用户离线在线状态信息
bool UserModel::updateState(User &user)
{
    char sql[128] = {0};
    sprintf(sql, "update user set state = '%s' where id = %d", user.getState().c_str(), user.getId());
    MySQL mysql;
    if (mysql.connect())
    {
        // 连接成功执行更新语句，更该用户状态
        if (mysql.update(sql))
        {
            return true;
        }
    }
    return false;
}
// 重置用户的状态信息
void UserModel::resetState(){
    char sql[128]="update user set state='offline' where state='online'";
    MySQL mysql;
    if(mysql.connect()){
        mysql.update(sql);
    }
}