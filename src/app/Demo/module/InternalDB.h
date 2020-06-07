#ifndef __INCLUDE_INTERNAL_DB_H__
#define __INCLUDE_INTERNAL_DB_H__

#pragma once
#include <string>
#include <vector>
#include "db/sqlite3_interface.h"

struct AccountInfo {
    std::string _phone;
    std::string _name;
    std::string _tax_no;
    std::string _password;
    bool _auto_login_flag = false;
    std::string _autologin_end_date;
    std::string _taxer_type;
    std::string _ent_name;
};

class InternalDB
{
public:
    InternalDB() = default;
    ~InternalDB();

    /**
     * @brief 数据库的初始化，包括打开数据库文件，进行数据库加密密钥设置
     * @param[in]
     * @return
     */
    bool Init(const std::string &dbFilePath);

    bool InsertAccountInfo(const AccountInfo &Info);

    bool ChangeAccountPwd(const std::string &newpwd, const std::string &phone);
    std::string GetAccountPwd(std::string &phone);
    AccountInfo GetSiheAccountInfo(std::string &phone);
    bool IsSupportAutoLogin(std::string &phone);
    /**
     * @brief 获取记录的登录账号列表
     * @param[in]
     * @return
     */
    std::vector<std::string> GetAccountPhoneLists();

private:
    bool CheckTables();
    bool CreateTable(const std::string &tableName);

private:
    static std::string SQLITE_DB_KEY;

    Sqlite3Interface m_sqlite3Instance;
};
#endif // __INCLUDE_INTERNAL_DB_H__