#include "SiheAccountHelperView.h"
#include "toast.h"
#include "ModalWnd.hpp"
#include <functional>
#include <thread>
#include <chrono>
#include "closure/closure.h"
#include "win32/process_operator.h"
#include "app_env/app_ins.h"
#include "win32/string_code_convert.h"
#include "module/SiheAccountHelper.h"
#include <future>

static const std::wstring siheExcuteName = L"Hsoft.eTax.Shell.exe";
static const std::wstring siheExcuteTitleName = L"百账通思和财务";
static const std::wstring siheSoftPath = L"AccountantHelper";

#pragma warning(disable : 4503)

SiheAccountHelperView::SiheAccountHelperView(ui::CPaintManagerUI *uiManager, HWND hwnd)
    : m_uiManager(uiManager)
    , m_ownerHwnd(hwnd)
    , m_siheDB_handle(std::make_unique<SiheDataBase>())
{
    m_uiManager->AddNotifier(this);
    auto unicodeRunningPath = AppEnvironment::GetRunningPath();
    auto siheApplicationPath = unicodeRunningPath + siheSoftPath;
    auto mbcsApplicationPath = ckbase::win32::UnicodeToMBCS(siheApplicationPath);
    m_siheDB_handle->Init(mbcsApplicationPath);
}

void SiheAccountHelperView::SetSiheUserData(const AccountInfo &info)
{
    Post2GlobalMisc([=]() {
        // 先将用户信息写入
        AccountInfo publicInfo = info;

        //思和对纳税人的类别有自己的定义
        if (info._taxer_type == "02")
            publicInfo._taxer_type = "1";
        if (info._taxer_type == "01")
            publicInfo._taxer_type = "2";
        publicInfo._ent_name = ckbase::win32::MBCSToUTF8(info._ent_name);

        m_siheDB_handle->InsertPublicInfo(publicInfo);
    });
}

bool SiheAccountHelperView::CheckAccountStatus(const std::string &userId)
{
    ModalWnd::ShowWaiting(m_ownerHwnd);

    std::future<UnorderedStringMap> reqStatusTask = std::async(std::launch::async, [=]() {
        SiheAccountHelper helper;
        return helper.DoRequestSiheAccountStatus(userId);
    });

    reqStatusTask.wait();

    bool accountValid = false;

    auto response = reqStatusTask.get();

    ModalWnd::CloseModal();
    if (response.at("success") == "true") {
        if (response.at("key") == "01") {
            accountValid = true;
        }
    }

    return accountValid;
}

void SiheAccountHelperView::Notify(ui::TNotifyUI &msg)
{
    if (msg.sType != DUI_MSGTYPE_CLICK)
        return;
    auto btnName = msg.pSender->GetName();
    if (btnName == L"BtnInvoiceDataImport") {
        ModalWnd::ShowWaiting(m_ownerHwnd);
        std::function<void(void)> task = [this]() {
            // FIXME
            std::this_thread::sleep_for(std::chrono::seconds(2));
        };
        std::function<void(void)> respReply = [this]() {
            ModalWnd::CloseModal();
            Toast::Show(L"导入成功", 2000, m_ownerHwnd);
        };
        Await(ThreadId::Misc, task, respReply);
    }
    else if (btnName == L"BtnStartAccountantHelper") {
        StartSiheSoftware();
    }
}

void SiheAccountHelperView::StartSiheSoftware()
{
    ModalWnd::ShowWaiting(m_ownerHwnd);
    auto unicodeRunningPath = AppEnvironment::GetRunningPath();
    auto siheApplicationPath = unicodeRunningPath + siheSoftPath;
    auto exePath = siheApplicationPath + L"\\" + siheExcuteName;
    if (ckbase::win32::IsProcessRun(siheExcuteName)) {
        HWND hWnd = FindWindowEx(NULL, 0, NULL, siheExcuteTitleName.c_str());
        if (IsIconic(hWnd))
            ShowWindow(hWnd, SW_RESTORE);
        else
            BringWindowToTop(hWnd);

        return;
    }
    std::function<bool(void)> task = [=]() {
        return ckbase::win32::BlockExecProcess(exePath);
    };

    std::function<void(bool)> Reply = [this](bool isSuccess) {
        ModalWnd::CloseModal();

        if (!isSuccess)
            Toast::Show(L"启动失败", 2000, m_ownerHwnd);
    };

    Await(ThreadId::Misc, task, Reply);
}
