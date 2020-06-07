#ifndef __INCLUDE_SQLITE3_INTERFACE_H__
#define __INCLUDE_SQLITE3_INTERFACE_H__

#include <string>
#include <vector>
#include "db/cppsqlite3.h"

using namespace detail;

// lizhao add on 19-04-11 搞不懂之前的数据库封装怎么写的,
//重新 造个轮子好了(笑哭)
//本类 为cppSQLlite3 类的 线程安全同步类

// 2020年6月3日  lizhao 去除加锁逻辑，无锁版本，使用单线程处理db操作，无需上锁！
using VVerString = std::vector<std::vector<std::string>>;

class Sqlite3Interface
{
public:
    Sqlite3Interface() = default;
    ~Sqlite3Interface() = default;

public:
    bool Open(const std::string &dbPath);
    void Close();

    int BeginTransaction();
    int Rollback();
    int Commit();

    bool ExecuteSql(const std::string &sql);
    bool BindExecuteSql(const std::string &sql, std::vector<std::string> &vecRow);
    bool GetExecuteSqlData(const char *Sql, VVerString &vecDatas);

    bool IsTableExist(const std::string &tableName);
    bool IsTableFieldNameExist(const std::string &strTableName,
                               const std::string &fieldName);

    bool SetPwd(const char *key, unsigned int nlen);

private:
    CppSQLite3DB m_cppSQLLite3DBIns;
    CppSQLite3Query m_cppSQLLite3QueryIns;

    bool m_isOpened = false;
};
#endif // __INCLUDE_SQLITE3_INTERFACE_H__