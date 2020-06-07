#include "db/sqlite3_interface.h"

bool Sqlite3Interface::Open(const std::string &dbPath)
{
    if (m_isOpened)
        return true;

    m_cppSQLLite3DBIns.open(dbPath.c_str());

    m_isOpened = true;
    return true;
}

void Sqlite3Interface::Close()
{
    if (!m_isOpened)
        return;
    m_cppSQLLite3DBIns.close();
    m_isOpened = false;
}

bool Sqlite3Interface::SetPwd(const char *key, unsigned int nlen)
{
    if (!m_isOpened)
        return false;

    return (SQLITE_OK == m_cppSQLLite3DBIns.setKey(key, nlen));
}

int Sqlite3Interface::BeginTransaction()
{
    return m_cppSQLLite3DBIns.BeginTransaction();
}

int Sqlite3Interface::Rollback()
{
    return m_cppSQLLite3DBIns.Rollback();
}

int Sqlite3Interface::Commit()
{
    return m_cppSQLLite3DBIns.Commit();
}

bool Sqlite3Interface::BindExecuteSql(const std::string &sql,
                                      std::vector<std::string> &vecRow)
{
    if (!m_isOpened)
        return false;
    CppSQLite3Statement statement = m_cppSQLLite3DBIns.compileStatement(sql.c_str());
    for (unsigned int i = 0; i < vecRow.size(); i++) {
        statement.bind(i + 1, vecRow.at(i).c_str());
    }
    statement.execDML();
    statement.finalize();
    return true;
}

bool Sqlite3Interface::ExecuteSql(const std::string &sql)
{
    if (!m_isOpened)
        return false;

    auto ret = m_cppSQLLite3DBIns.execDML(sql.c_str());

    // m_cppSQLLite3QueryIns.finalize();
    return ret > 0;
}

bool Sqlite3Interface::GetExecuteSqlData(const char *Sql, VVerString &vecDatas)
{
    if (!m_isOpened)
        return false;

    m_cppSQLLite3QueryIns = m_cppSQLLite3DBIns.execQuery(Sql);
    if (m_cppSQLLite3QueryIns.eof() || m_cppSQLLite3QueryIns.numFields() < 1) {
        //查询数据为空
        m_cppSQLLite3QueryIns.finalize();
        return false;
    }
    vecDatas.clear();
    std::vector<std::string> vecRow;
    while (!m_cppSQLLite3QueryIns.eof()) {
        int index = 0;
        vecRow.clear();
        while (index < m_cppSQLLite3QueryIns.numFields()) {
            const char *pText = m_cppSQLLite3QueryIns.fieldValue(index);
            if (pText != NULL) {
                vecRow.emplace_back(pText);
            }
            ++index;
        }
        if (vecRow.size() != 0) {
            vecDatas.emplace_back(vecRow);
        }
        m_cppSQLLite3QueryIns.nextRow();
    }
    m_cppSQLLite3QueryIns.finalize();
    return true;
}

bool Sqlite3Interface::IsTableExist(const std::string &tableName)
{
    return m_cppSQLLite3DBIns.tableExists(tableName.c_str());
}

bool Sqlite3Interface::IsTableFieldNameExist(const std::string &strTableName,
                                             const std::string &fieldName)
{
    sqlite3_stmt *stmt = NULL;
    char sql[200] = {0x00};
    sprintf_s(sql, 200, "PRAGMA table_info([%s])", strTableName.c_str());
    int nRow = 0, nCol = 0;
    char *pErr = NULL;

    CppSQLite3Table tab = m_cppSQLLite3DBIns.getTable(sql);
    nRow = tab.numRows();   //行
    nCol = tab.numFields(); //列数

    //先查找name字段所在的列数
    int i_nameCol = -1;

    for (int i = 0; i < nCol; ++i) {
        if (strcmp(tab.fieldName(i), "name") == 0) {
            i_nameCol = i;
            break;
        }
    }
    // name字段一般在第2列，数值为1
    if (-1 == i_nameCol)
        return false;

    for (int j = 0; j < nRow; j++) {
        tab.setRow(j);
        if (strcmp(tab.fieldValue(i_nameCol), fieldName.c_str()) == 0) {
            return true;
        }
    }
    return false;
}