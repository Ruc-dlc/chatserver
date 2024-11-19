#include "groupmodel.hpp"
#include "db.hpp"
// 创建群组
bool GroupModel::createGroup(Group &group)
{
    char sql[128] = {0};
    sprintf(sql, "insert into allgroup(groupname,groupdesc) values('%s','%s')", group.getName().c_str(), group.getDesc().c_str());
    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            group.setId(mysql_insert_id(mysql.getConnection()));
            return true;
        }
    }
    return false;
}
// 加入群组
void GroupModel::addGroup(int userid, int groupid, string role)
{
    char sql[128] = {0};
    sprintf(sql, "insert into groupuser values(%d,%d,'%s')", groupid, userid, role.c_str());
    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}

// 查询用户所在的所有群组信息
vector<Group> GroupModel::queryGroups(int userid)
{
    char sql[128] = {0};
    sprintf(sql, "select a.id,a.groupname,a.groupdesc from allgroup a inner join groupuser b on a.id=b.groupid where b.userid=%d", userid);
    vector<Group> groupVec;
    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                Group group;
                group.setId(atoi(row[0]));
                group.setName(row[1]);
                group.setDesc(row[2]);
                groupVec.push_back(group);
            }
            mysql_free_result(res);
        }
        // 遍历查到的用户所在的所有群组，在该群组内搜索包含的所有用户
        for (Group &group : groupVec)
        {
            sprintf(sql, "select a.id,a.name,a.state,b.grouprole from user a inner join groupuser b on b.userid=a.id where b.groupid=%d", group.getId());
            MYSQL_RES *res = mysql.query(sql);
            if (res)
            {
                MYSQL_ROW row;
                while ((row = mysql_fetch_row(res)) != nullptr)
                {
                    GroupUser user;
                    user.setId(atoi(row[0]));
                    user.setUsername(row[1]);
                    user.setState(row[2]);
                    user.setRole(row[3]);
                    group.getUsers().push_back(user);
                }
                mysql_free_result(res);
            }
        }
    }

    return groupVec;
}
// 根据groupid查询群组用户中间表，查询该组包含的所有用户,需要去除登录的用户本身
vector<int> GroupModel::queryGroupUsers(int userid, int groupid)
{
    char sql[128] = {0};
    sprintf(sql, "select userid from groupuser where groupid=%d and userid!=%d", groupid, userid);
    MySQL mysql;
    vector<int> ids;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                ids.push_back(atoi(row[0]));
            }
            mysql_free_result(res);
        }
    }
    return ids;
}