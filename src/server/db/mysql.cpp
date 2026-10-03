#include "mysql.h"

MySQL::MySQL()
{
    _conn = mysql_init(nullptr);
}
MySQL::~MySQL()
{
    if (_conn != nullptr)
    {
        mysql_close(_conn);
    }
}
bool MySQL::connect()
{
    MYSQL *p = mysql_real_connect(_conn, server.c_str(), user.c_str(),
                                  password.c_str(), dbname.c_str(), 3306, nullptr, 0);
    if (p != nullptr)
    {
        mysql_query(_conn, "set names gbk");
        LOG_INFO << "connect mysql success!";
    }
    else
    {
        LOG_INFO << "connect mysql fail!";
    }
    return p;
}
bool MySQL::update(string sql)
{
    if (mysql_query(_conn, sql.c_str()))
    {
        LOG_INFO << __FILE__ << ":" << __LINE__ << ":"
                 << sql << "更新失败";
        return false;
    }
    return true;
}
MYSQL_RES *MySQL::query(string sql)
{
    if (mysql_query(_conn, sql.c_str()))
    {
        LOG_INFO << __FILE__ << ":" << __LINE__ << ":"
                 << sql << "查询失败";
        return nullptr;
    }
    return mysql_use_result(_conn);
}

MYSQL *MySQL::getConnection()
{
    return _conn;
}

string MySQL::escape(const string &str)
{
        char buf[2 * str.length() + 1] = {0};
        unsigned long len = mysql_real_escape_string(_conn, buf, str.c_str(), str.length());
        return string(buf, len);
}