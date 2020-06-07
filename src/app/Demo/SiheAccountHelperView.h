#ifndef __INCLUDE_SIHE_ACCOUNTHELPERVIEW_H__
#define __INCLUDE_SIHE_ACCOUNTHELPERVIEW_H__

#pragma once

#include "UIlib.h"
#include "module/SiheDB.h"
#include <memory>
#include "module/InternalDB.h"
class SiheAccountHelperView : public ui::INotifyUI
{
public:
    SiheAccountHelperView(ui::CPaintManagerUI *uiManager, HWND hwnd);
    ~SiheAccountHelperView() = default;

    /**
     * @brief 设置用户信息，用于思和软件的读取，这里会把数据写入思和的数据库中
     * @param[in]
     * @return
     */
    void SetSiheUserData(const AccountInfo &info);

    bool CheckAccountStatus(const std::string &userId);

private:
    void Notify(ui::TNotifyUI &msg) final;
    void StartSiheSoftware();

private:
    ui::CPaintManagerUI *m_uiManager;
    HWND m_ownerHwnd;
    std::unique_ptr<SiheDataBase> m_siheDB_handle;
};
#endif // __INCLUDE_SIHE_ACCOUNTHELPERVIEW_H__