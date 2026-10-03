#include "usermodel.hpp"
#include "mysql.h"

#include <iostream>
using namespace std;
bool UserModel::insert(User &user)
{
    MySQL mysql;
    if(!mysql.connect())
    {
        return false;
    }
    // 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into User(name,password,state) values('%s','%s','%s')",
            mysql.escape(user.getName()).c_str(),
            mysql.escape(user.getPwd()).c_str(),
            mysql.escape(user.getState()).c_str());
    

    if (mysql.update(sql))
    {
        // 获取插入成功的用户数据生成的主键id
        user.setId(mysql_insert_id(mysql.getConnection()));
        return true;
    }

    return false;
}

User UserModel::query(string &name)
{
    MySQL mysql;
    if (!mysql.connect())
    {
        return User();
    }
    char sql[1024] = {0};
    sprintf(sql, "select * from User where name = '%s'", mysql.escape(name).c_str());

    MYSQL_RES *res = mysql.query(sql);
    if (res != nullptr)
    {
        MYSQL_ROW row = mysql_fetch_row(res);
        if (row != nullptr)
        {
            User user;
            user.setId(atoi(row[0]));
            user.setName(row[1]);
            user.setPwd(row[2]);
            user.setState(row[3]);

            mysql_free_result(res);
            return user;
        }
    }

    return User();
}
User UserModel::query(int &id)
{
    char sql[1024] = {0};
    sprintf(sql, "select * from User where id = '%d'", id);
    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row = mysql_fetch_row(res);
            if (row != nullptr)
            {
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setPwd(row[2]);
                user.setState(row[3]);

                mysql_free_result(res);
                return user;
            }
        }
    }
    return User();
}

bool UserModel::updateState(User &user)
{
    MySQL mysql;
    if (!mysql.connect())
    {
        return false;
    }
    char sql[1024] = {0};

    sprintf(sql, "update User set state = '%s' where id = %d", mysql.escape(user.getState()).c_str(), user.getId());

    if (mysql.update(sql))
    {
        return true;
    }

    return false;
}

void UserModel::resetState()
{
    char sql[1024] = {0};

    sprintf(sql, "update User set state = 'offline' where state='online'");
    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}