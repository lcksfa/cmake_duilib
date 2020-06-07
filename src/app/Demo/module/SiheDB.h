#pragma once
#include "db/sqlite3_interface.h"
#include <string>
#include "windows.h"
#include "InternalDB.h"
// struct PublicAccountInfo {
//     std::string _phone_num;   //手机号
//     std::string _nsrsbh;      //纳税人识别号
//     std::string _nsrlb;       //纳税人类别
//     std::string _nsr_comName; //纳税人单位名称
//     std::string _nsr_lxr;     //联系人
//     std::string _nsr_lxdh;    //联系电话
//     std::string _nsr_lxdz;    //联系地址
// };

struct FPInfo {
    std::string fpid;
    std::string phone;  //授权手机号
    std::string nsrsbh; //
    std::string fpzt;   //发票状态
    std::string djh;    //单据号
    std::string fphm;   //
    std::string fpzl;   //发票种类
    std::string fpdm;
    std::string kprq;    //
    std::string gf_sh;   //购方税号
    std::string gf_qymc; //购方企业名称
    std::string gf_yhzh; //购方银行账号
    std::string gf_dzdh; //购方地址电话

    std::string xf_sh;   //
    std::string xf_qymc; //
    std::string xf_yhzh; //
    std::string xg_dzdh; //

    std::string hjje;
    std::string hjse;
    std::string bz;   //
    std::string kpr;  //
    std::string fhr;  //
    std::string skr;  //
    std::string sczt; //
    std::string skm;  //
    std::string qrztdm;
    std::string qrsj; //
};

struct FPFL_Info {
    std::string fpid;
    std::string xh;   //序号
    std::string spbm; //
    std::string spmc; //
    std::string spfl;
    std::string ggxh; //
    std::string jldw; //计量单位
    std::string dj;
    std::string sl;
    std::string je;
    std::string slv; //税率
    std::string se;
};

using pf_sqlite3_init = int(_stdcall *)(char *pName);
using pf_sqlite3_uninit = void(_stdcall *)();
using pf_sqlite3_close = int (*)(sqlite3 *);
using pf_sqlite3_open = int (*)(const char *filename, /* Database filename (UTF-8) */
                                sqlite3 **ppDb        /* OUT: SQLite db handle */
);

using pf_sqlite3_key = int (*)(sqlite3 *db,               /* Database to be rekeyed */
                               const void *pKey, int nKey /* The key */
);

using pf_sqlite3_exec = int (*)(sqlite3 *,       /* An open database */
                                const char *sql, /* SQL to be evaluated */
                                int (*callback)(void *, int, char **,
                                                char **), /* Callback function */
                                void *,                   /* 1st argument to callback */
                                char **errmsg             /* Error msg written here */
);

using pf_sqlite3_prepare_v2 = int (*)(
    sqlite3 *db,           /* Database handle */
    const char *zSql,      /* SQL statement, UTF-8 encoded */
    int nByte,             /* Maximum length of zSql in bytes. */
    sqlite3_stmt **ppStmt, /* OUT: Statement handle */
    const char **pzTail    /* OUT: Pointer to unused portion of zSql */
);

using pf_sqlite3_step = int (*)(sqlite3_stmt *db);
using pf_sqlite3_finalize = int (*)(sqlite3_stmt *db);
using pf_sqlite3_free = void (*)(void *mem);

class SiheDataBase
{
public:
    SiheDataBase();
    ~SiheDataBase();

public:
    bool Init(const std::string &dbPath);
    void Close();

    bool InsertPublicInfo(const AccountInfo &Info);
    bool InsertInvoiceInfo(const FPInfo &Info);
    bool InsertInvoice_FL_Info(const FPFL_Info &info);

private:
    bool ExecuteSql(const std::string &strSql, std::string &errMsg);

private:
    static const std::string SQLITE_DB_KEY;
    HMODULE dllHModule_;
    pf_sqlite3_init dbInitFunc_;
    pf_sqlite3_uninit dbUninitFunc_;
    pf_sqlite3_close dbCloseFunc_;
    pf_sqlite3_open dbOpenFunc_;
    pf_sqlite3_key dbKeyFunc_;
    pf_sqlite3_exec dbExecFunc_;

    pf_sqlite3_prepare_v2 dbPrepareV2_;
    pf_sqlite3_step dbStep_;
    pf_sqlite3_finalize dbFinalize_;
    pf_sqlite3_free dbfree_;

    std::string dbPath_;
};