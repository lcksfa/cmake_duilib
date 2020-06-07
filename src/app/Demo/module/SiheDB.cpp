#include "SiheDB.h"
#include "log/logger.h"
#include "app_env/app_ins.h"
#include "win32/string_code_convert.h"

const std::string SiheDataBase::SQLITE_DB_KEY = "Banner2014";

bool SiheDataBase::InsertPublicInfo(const AccountInfo &Info)
{
    auto sqlOfClean = "delete from PI_PublicInfo";
    std::string err;
    if (!ExecuteSql(sqlOfClean, err))
        return false;

    std::string sql =
        "insert into PI_PublicInfo values('%s','%s','%s','%s','%s','%s','%s','%s')";
    std::wstring strRunPath(AppEnvironment::GetRunningPath());
    std::wstring strEZTPath = strRunPath + L"EAccoplus.exe";
    auto utfEZTPath = ckbase::win32::UnicodeToUTF8(strEZTPath);

    char szSql[2048] = {0x00};
    sprintf_s(szSql, sizeof(szSql), sql.c_str(), Info._phone.c_str(),
              Info._tax_no.c_str(), Info._taxer_type.c_str(), Info._ent_name.c_str(), "",
              "", "", utfEZTPath.c_str());
    err.clear();
    return ExecuteSql(szSql, err);
}

bool SiheDataBase::InsertInvoice_FL_Info(const FPFL_Info &info)
{
    std::string err;

    std::string sql =
        "insert into PI_FP_FLInfo "
        "values('%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s')";
    char szSql[2048] = {0x00};
    sprintf_s(szSql, sizeof(szSql), sql.c_str(), info.fpid.c_str(), info.xh.c_str(),
              info.spbm.c_str(), info.spmc.c_str(), info.spfl.c_str(), info.ggxh.c_str(),
              info.jldw.c_str(), info.dj.c_str(), info.sl.c_str(), info.je.c_str(),
              info.slv.c_str(), info.se.c_str());
    return ExecuteSql(szSql, err);
}

bool SiheDataBase::InsertInvoiceInfo(const FPInfo &Info)
{
    std::string err;
    //先试图删除已经存在的发票，防止重复插入
    std::string sqlOfdeleteItem = "delete from PI_FPInfo where FPHM='%s' and FPDM='%s'";
    char szSqlOfDelete[2048] = {0x00};
    sprintf_s(szSqlOfDelete, sizeof(szSqlOfDelete), sqlOfdeleteItem.c_str(),
              Info.fphm.c_str(), Info.fpdm.c_str());

    if (!ExecuteSql(szSqlOfDelete, err))
        return false;

    std::string sql =
        "insert into PI_FPInfo "
        "values(NULL,'%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s'"
        ",'%s','%s','%s','%s','%s','%s','%s',"
        "'%s','%s','%s','%s','%s','%s','%s','%s')";
    char szSql[2048] = {0x00};
    sprintf_s(szSql, sizeof(szSql), sql.c_str(), Info.fpid.c_str(), Info.phone.c_str(),
              Info.nsrsbh.c_str(), Info.fpzt.c_str(), Info.djh.c_str(), Info.fphm.c_str(),
              Info.fpzl.c_str(), Info.fpdm.c_str(), Info.kprq.c_str(), Info.gf_sh.c_str(),
              Info.gf_qymc.c_str(), Info.gf_yhzh.c_str(), Info.gf_dzdh.c_str(),
              Info.xf_sh.c_str(), Info.xf_qymc.c_str(), Info.xf_yhzh.c_str(),
              Info.xg_dzdh.c_str(), Info.hjje.c_str(), Info.hjse.c_str(), Info.bz.c_str(),
              Info.kpr.c_str(), Info.fhr.c_str(), Info.skr.c_str(), Info.sczt.c_str(),
              Info.skm.c_str(), Info.qrztdm.c_str(), Info.qrsj.c_str());
    return ExecuteSql(szSql, err);
}

SiheDataBase::SiheDataBase()
    : dllHModule_(nullptr)
{
}

SiheDataBase::~SiheDataBase()
{
    if (nullptr != dllHModule_) {
        FreeLibrary(dllHModule_);
        dllHModule_ = nullptr;
    }
}
bool SiheDataBase::Init(const std::string &dllPath)
{
    if (NULL != dllHModule_) {

        FreeLibrary(dllHModule_);
        dllHModule_ = NULL;
    }

    if (NULL == dllHModule_) {
        auto unicodeDllPath = ckbase::win32::MBCSToUnicode(dllPath);
        std::wstring strDllPath = unicodeDllPath + L"\\SQLite.Interop.dll";
        dllHModule_ = LoadLibrary(strDllPath.c_str());
        DWORD err = GetLastError();
        if (err == 126) {
            ckbase::Warn(L"Sihe:SQLite.Interop.dll不存在或所依赖的文件不存在!");
            return false;
        }
    }
    dbPath_ = dllPath + "\\DB\\eTaxDB.db";
    dbInitFunc_ = (pf_sqlite3_init) GetProcAddress(dllHModule_, "sqlite3_initialize");
    dbOpenFunc_ = (pf_sqlite3_open) GetProcAddress(dllHModule_, "sqlite3_open");
    dbKeyFunc_ = (pf_sqlite3_key) GetProcAddress(dllHModule_, "sqlite3_key");
    dbExecFunc_ = (pf_sqlite3_exec) GetProcAddress(dllHModule_, "sqlite3_exec");
    dbCloseFunc_ = (pf_sqlite3_close) GetProcAddress(dllHModule_, "sqlite3_close");
    dbPrepareV2_ =
        (pf_sqlite3_prepare_v2) GetProcAddress(dllHModule_, "sqlite3_prepare_v2");
    dbStep_ = (pf_sqlite3_step) GetProcAddress(dllHModule_, "sqlite3_step");
    dbFinalize_ = (pf_sqlite3_finalize) GetProcAddress(dllHModule_, "sqlite3_finalize");

    dbfree_ = (pf_sqlite3_free) GetProcAddress(dllHModule_, "sqlite3_free");
    return dllHModule_ != NULL;
}

bool SiheDataBase::ExecuteSql(const std::string &strSql, std::string &errMsg)
{
    if (dllHModule_ == nullptr)
        return false;
    bool ret = false;
    sqlite3 *pSQLite = NULL;
    auto uft8_path = ckbase::win32::MBCSToUTF8(dbPath_);
    dbOpenFunc_(uft8_path.c_str(), &pSQLite);

    if (NULL == pSQLite) {
        ckbase::Warn(L"思和db：文件打开失败！");
        return false;
    }

    auto nRet = dbKeyFunc_(pSQLite, SQLITE_DB_KEY.c_str(), SQLITE_DB_KEY.length());
    sqlite3_stmt *stmt = NULL;

    nRet = dbPrepareV2_(pSQLite, "PRAGMA case_sensitive_like = 1", -1, &stmt, NULL);
    if (NULL == stmt) {
        ckbase::Warn(L"思和db：sqlite3_prepare_v2 执行失败！");
        goto SQLEND;
    }

    nRet = dbStep_(stmt);
    nRet = dbFinalize_(stmt);

    char *pszErrMsg = NULL;
    //因为思和的数据库只有写操作，没有读操作，所以没有使用回调读取函数，后面如有变更可以
    //参看百赋通里的相关代码进项修改
    nRet = dbExecFunc_(pSQLite, strSql.c_str(), NULL, NULL, &pszErrMsg);
    if (0 == nRet) {
        ret = true;
    }
    else {
        if (pszErrMsg != NULL) {
            errMsg = pszErrMsg;
            dbfree_(pszErrMsg);
        }
    }
SQLEND:
    if (NULL != pSQLite) {
        dbCloseFunc_(pSQLite);
    }
    return ret;
}