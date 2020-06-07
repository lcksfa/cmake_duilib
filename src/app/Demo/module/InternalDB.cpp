/*
 * Written by Li Zhao <bw_lizhao AT whty Dot com Dot cn>.
 * On 2020年6月3日
 * This file use for 数据库业务接口
 *
 */
#include "InternalDB.h"
#include "log/logger.h"

std::string InternalDB::SQLITE_DB_KEY = "5hkj_74Hae_2020@7";

const std::string tableAccountName{"account_info"};

InternalDB::~InternalDB()
{
    m_sqlite3Instance.Close();
}

bool InternalDB::Init(const std::string &dbFilePath)
{
    try {
        m_sqlite3Instance.Open(dbFilePath);
        // #ifndef _DEBUG
        //         m_sqlite3Instance.SetPwd(SQLITE_DB_KEY.c_str(),
        //         SQLITE_DB_KEY.length());
        // #endif
    }
    catch (CppSQLite3Exception &e) {
        ckbase::Warn("InternalDB::Init error:{}", e.errorMessage());
        return false;
    }
    return CheckTables();
}

bool InternalDB::CheckTables()
{
    bool isCheckDone = false;
    try {
        if (!m_sqlite3Instance.IsTableExist(tableAccountName)) {
            isCheckDone = CreateTable(tableAccountName);
        }
    }
    catch (CppSQLite3Exception &e) {
        ckbase::Warn("InternalDB::CheckTables:CreateTable error:{}", e.errorMessage());
        return false;
    }

    return true;
}

bool InternalDB::CreateTable(const std::string &tableName)
{
    auto creattablesql = "CREATE TABLE account_info ("
                         "phone               TEXT (30)   NOT NULL PRIMARY KEY,"
                         "name                TEXT (100)  NOT NULL,"
                         "taxno                TEXT (100)  NOT NULL,"
                         "password           TEXT (32)   NOT NULL,"
                         "taxer_type         TEXT (32)   NOT NULL,"
                         "ent_name          TEXT (256)   NOT NULL,"
                         "auto_login_flag     BOOLEAN (1) NOT NULL DEFAULT (false),"
                         "auto_login_end_date DATE,"
                         "extend1             TEXT (256),"
                         "extend2             TEXT (256) )"
                         "WITHOUT ROWID;";

    return m_sqlite3Instance.ExecuteSql(creattablesql);
}

bool InternalDB::InsertAccountInfo(const AccountInfo &info)
{
    std::string sqlformat{
        "Replace INTO  account_info values('{}','{}','{}','{}','{}','{}',{},{},'','')"};
    auto sql = fmt::format(sqlformat, info._phone, info._name, info._tax_no,
                           info._password, info._taxer_type, info._ent_name,
                           info._auto_login_flag, info._autologin_end_date);

    return m_sqlite3Instance.ExecuteSql(sql);
}

bool InternalDB::ChangeAccountPwd(const std::string &newpwd, const std::string &phone)
{
    std::string sqlformat{"update account_info set passwords={} where phone_num={}"};
    auto sql = fmt::format(sqlformat, newpwd, phone);
    return m_sqlite3Instance.ExecuteSql(sql);
}

std::string InternalDB::GetAccountPwd(std::string &phone)
{
    std::string sqlformat{"select password from account_info where phone like {}"};
    auto sql = fmt::format(sqlformat, phone);

    std::vector<std::vector<std::string>> vecRecords;
    m_sqlite3Instance.GetExecuteSqlData(sql.c_str(), vecRecords);

    if (vecRecords.size() == 1) {
        std::vector<std::string> record = vecRecords[0];

        return record[0];
    }
    return std::string("");
}

AccountInfo InternalDB::GetSiheAccountInfo(std::string &phone)
{
    std::string sqlformat{"select phone,name,taxno,taxer_type,ent_name from account_info "
                          "where phone like {}"};
    auto sql = fmt::format(sqlformat, phone);

    std::vector<std::vector<std::string>> vecRecords;
    m_sqlite3Instance.GetExecuteSqlData(sql.c_str(), vecRecords);
    AccountInfo info;
    if (vecRecords.size() == 1) {
        std::vector<std::string> record = vecRecords[0];
        info._phone = record[0];
        info._name = record[1];
        info._tax_no = record[2];
        info._taxer_type = record[3];
        info._ent_name = record[4];
    }
    return info;
}

bool InternalDB::IsSupportAutoLogin(std::string &phone)
{
    std::string sqlformat{
        "select phone from account_info where auto_login_flag=true and "
        "date('now','localtime') < auto_login_end_date and phone like '{}';"};
    auto sql = fmt::format(sqlformat, phone);

    std::vector<std::vector<std::string>> vecRecords;
    m_sqlite3Instance.GetExecuteSqlData(sql.c_str(), vecRecords);
    return vecRecords.size() == 1;
}

std::vector<std::string> InternalDB::GetAccountPhoneLists()
{
    std::string sqlformat{"select phone from account_info ORDER BY phone Desc"};

    std::vector<std::vector<std::string>> vecRecords;
    m_sqlite3Instance.GetExecuteSqlData(sqlformat.c_str(), vecRecords);
    std::vector<std::string> phoneList;
    for (const auto &record : vecRecords) {
        for (const auto &list : record) {
            phoneList.emplace_back(list);
        }
    }
    return phoneList;
}
