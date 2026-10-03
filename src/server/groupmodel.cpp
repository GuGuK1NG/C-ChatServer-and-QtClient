#include "groupmodel.hpp"
#include "mysql.h"

bool GroupModel::createGroup(Group &group)
{
    MySQL mysql;
    if (!mysql.connect())
    {
        return false;
    }
    char sql[1024] = {0};
    sprintf(sql, "insert into AllGroup(groupname,groupdesc) values('%s','%s')",
            mysql.escape(group.getName()).c_str(), mysql.escape(group.getDesc()).c_str());

    if (mysql.update(sql))
    {
        group.setId(mysql_insert_id(mysql.getConnection()));
        return true;
    }

    return false;
}
void GroupModel::addGroup(int userid, int groupid, string role)
{
    MySQL mysql;
    if (!mysql.connect())
    {
        return;
    }
    char sql[1024] = {0};
    sprintf(sql, "insert into GroupUser values(%d,%d,'%s')",
            groupid, userid, mysql.escape(role).c_str());

    mysql.update(sql);
}
vector<Group> GroupModel::queryGroups(int userid)
{
    char sql[1024] = {0};
    sprintf(sql, "select a.id,a.groupname,a.groupdesc from AllGroup a inner join\
        GroupUser b on a.id=b.groupid where b.userid=%d",
            userid);
    vector<Group> groupVec;

    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
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
    }
    for (Group &group : groupVec)
    {
        // 查该群的成员列表：从 User 表取成员 id/姓名，从 GroupUser 表取角色
        sprintf(sql, "select a.id,a.name,b.role from User a inner join\
        GroupUser b on a.id=b.userid where b.groupid=%d",
                group.getId());

        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                GroupUser user;
                user.setId(atoi(row[0]));   // a.id   -> 成员id
                user.setName(row[1]);       // a.name -> 成员名
                user.setRole(row[2]);       // b.role -> 角色
                group.getUsers().push_back(user);
            }
            mysql_free_result(res);
        }
    }
    return groupVec;
}
vector<int> GroupModel::queryGroupUsers(int userid, int groupid)
{
    char sql[1024] = {0};
    sprintf(sql, "select userid from GroupUser where groupid = %d \
        and userid!=%d",
            groupid, userid);
    vector<int> idVec;

    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                idVec.push_back(atoi(row[0]));
            }
            mysql_free_result(res);
        }
    }
    return idVec;
}
