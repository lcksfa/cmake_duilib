/*
 * Written by Li Zhao <bw_lizhao AT whty Dot com Dot cn>.
 * On 2020年5月27日
 * This file use for solution main window，and resource init
 *
 */

#include "pch.h"
#include "MainWindow.h"
#include "resource.h"
#include "ModalWnd.hpp"
#include "win32/string_code_convert.h"
#include "MsgBox.h"
#include "app_env\app_ins.h"

using namespace ui;

const std::wstring MainWindow::kClassName = L"EAccoplus";
static const std::wstring kAppName = L"E账通";

constexpr auto TIMER_RESET_STATUSBAR_MSG = 50001;
constexpr auto TRAY_EXIT_APP = 101;
bool ResourceInitial::InitDebugModeResource()
{
    return CResourceManager::GetInstance()->LoadResource(_T("res.xml"), NULL) == TRUE;
}

bool ResourceInitial::InitReleaseModeResource()
{
    HRSRC hResource = ::FindResource(CPaintManagerUI::GetResourceDll(),
                                     MAKEINTRESOURCE(IDR_ZIPRES), _T("ZIPRES"));
    if (hResource == NULL) {
        return false;
    }
    HGLOBAL hGlobal = ::LoadResource(CPaintManagerUI::GetResourceDll(), hResource);
    if (hGlobal == NULL) {
        ::FreeResource(hResource);
        return false;
    }

    DWORD dwSize = ::SizeofResource(CPaintManagerUI::GetResourceDll(), hResource);
    if (dwSize > 0) {
        CPaintManagerUI::SetResourceZip((LPBYTE)::LockResource(hGlobal), dwSize);
        CResourceManager::GetInstance()->LoadResource(_T("res.xml"), NULL);
    }
    ::FreeResource(hResource);
    return true;
}

ResourceInitial::ResourceInitial(HINSTANCE hInstance)
{
    CPaintManagerUI::SetInstance(hInstance);

    CDuiString strResourcePath = CPaintManagerUI::GetInstancePath();
    strResourcePath += _T("Res\\");
    CPaintManagerUI::SetResourcePath(strResourcePath.GetData());
}

bool ResourceInitial::Init()
{
#if _DEBUG
    return InitDebugModeResource();
#else
    return InitReleaseModeResource();
#endif
}

CDuiString MainWindow::GetSkinFile()
{
    return L"mainwindow.xml";
}

LPCTSTR MainWindow::GetWindowClassName(void) const
{
    return kClassName.c_str();
}

LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT lRes = 0;
    BOOL bHandled = false;
    return __super::HandleMessage(uMsg, wParam, lParam);
}

LRESULT MainWindow::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam,
                                        BOOL &bHandled)
{
    LRESULT lRes = 0;
    bHandled = FALSE;
    switch (uMsg) {
    case WM_TIMER:
        lRes = OnTimer(uMsg, wParam, lParam, bHandled);
        break;
    case UIMSG_TRAYICON:
        OnTrayIcon(lParam);
        break;
    case WM_MENUCLICK: {
        MenuCmd *pMenuCmd = reinterpret_cast<MenuCmd *>(wParam);
        if (pMenuCmd == nullptr)
            return 0;
        OnClickMenu(pMenuCmd->szName);
        m_pm.DeletePtr(pMenuCmd);
    } break;
    default:
        break;
    }
    if (bHandled)
        return lRes;
    return __super::HandleCustomMessage(uMsg, wParam, lParam, bHandled);
}

void MainWindow::OnFinalMessage(HWND hWnd)
{
    __super::OnFinalMessage(hWnd);
    delete this;
}

void MainWindow::InitWindow()
{
    ::SetWindowText(m_hWnd, kAppName.data());
    SetIcon(IDC_LOGO);
    InitBackboneCtrls();

    m_loginView = std::make_unique<LoginView>(&m_pm, GetHWND());
    m_registerView = std::make_unique<RegisterView>(&m_pm, GetHWND());
    m_personalCenterView = std::make_unique<PersonalCenterView>(&m_pm, GetHWND());
    m_onekeyDeclarateView = std::make_unique<OnekeyDeclarateView>(&m_pm, GetHWND());
    m_siheAccountHelperView = std::make_unique<SiheAccountHelperView>(&m_pm, GetHWND());
    m_findPasswordView = std::make_unique<FindPasswordView>(&m_pm, GetHWND());
    CreateTray();
}

LRESULT MainWindow::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    PostQuitMessage(0L);
    return __super::OnClose(uMsg, wParam, lParam, bHandled);
}

LRESULT MainWindow::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    bHandled = FALSE;
    switch (wParam) {
    case TIMER_RESET_STATUSBAR_MSG:
        HideStatusBarTip();
        break;
    }
    return 0;
}

void MainWindow::InitBackboneCtrls()
{
    m_mainAni_tablayout =
        dynamic_cast<CAnimationTabLayoutUI *>(m_pm.FindControl(L"body_animationTab"));
    m_login_status_tab =
        dynamic_cast<CTabLayoutUI *>(m_pm.FindControl(L"login_status_tab"));
    m_header_logintitle_tab =
        dynamic_cast<CTabLayoutUI *>(m_pm.FindControl(L"header_logintitle_tab"));
    m_mainpage_tablayout =
        dynamic_cast<CTabLayoutUI *>(m_pm.FindControl(L"mainpage_tablayout"));

    m_register_tab = dynamic_cast<CTabLayoutUI *>(m_pm.FindControl(L"register_tab"));

    m_optmainpage = dynamic_cast<COptionUI *>(m_pm.FindControl(L"optmainpage"));

    BindButtonWithCallback();
}

void MainWindow::Notify(TNotifyUI &msg)
{
    auto ctrlName = msg.pSender->GetName();
    if (msg.sType == DUI_MSGTYPE_CLICK) {
        OnBtnClick(ctrlName.GetData());
    }
    else if (msg.sType == Constom_UI_log_Success) {
        if (ctrlName == L"btn_request_login") {
            //接收登录时的参数传递，保存用户信息
            auto pAccountInfo = reinterpret_cast<AccountInfo *>(msg.wParam);
            //这里使用智能指针管理 发送过来的内存
            std::shared_ptr<AccountInfo> newAccoutInfo{pAccountInfo};
            auto isAutoLogin = msg.lParam;
            //只有在用户手动更新登录选项时，才更新用户表数据
            if (!isAutoLogin) {
                Post2GlobalMisc([this, newAccoutInfo]() {
                    try {
                        m_dbHandle->InsertAccountInfo(*newAccoutInfo.get());
                    }
                    catch (const CppSQLite3Exception &e) {
                        //这里要尽力规避，sql语句要规范
                        ckbase::Warn("Fatal Error: Db InsertAccountInfo:{}",
                                     e.errorMessage());
                    }
                });
            }

            ShowMainPageViewWithLogin();
        }
    }
    else if (msg.sType == Constom_UI_reg_Success) {
        if (ctrlName == L"btn_request_reg") {
            ShowRegisterSuccessPageView();
        }
    }
    else if (msg.sType == DUI_MSGTYPE_SELECTCHANGED) {
        OnOptSelectChanged(ctrlName.GetData());
    }
    else if (msg.sType == DUI_MSGTYPE_WINDOWINIT) {
        //窗口初始化完成
        InitDB();

        auto accountPhoneList = m_dbHandle->GetAccountPhoneLists();
        if (accountPhoneList.size() == 1) {
            //目前只支持一个用户
            std::string phone = accountPhoneList[0];

            if (m_dbHandle->IsSupportAutoLogin(phone)) {
                auto pwd = m_dbHandle->GetAccountPwd(phone);
                auto unicodePhone = ckbase::win32::MBCSToUnicode(phone);
                auto unicodepwd = ckbase::win32::MBCSToUnicode(pwd);
                m_loginView->DoRequest(unicodePhone, unicodepwd, true);
            }
        }
    }
    return __super::Notify(msg);
}

void MainWindow::OnBtnClick(const std::wstring &name)
{
    auto findcallback = m_buttonsCallbackMap.find(name);
    if (findcallback != m_buttonsCallbackMap.end()) {
        findcallback->second();
    }
}

void MainWindow::ShowRegisterPageView()
{
    m_optmainpage->Selected(true, false);
    m_mainAni_tablayout->SelectItem(static_cast<int>(AppPageIndex::mainpage));
    m_mainpage_tablayout->SelectItem(static_cast<int>(MainPageIndex::registerview));
    m_register_tab->SelectItem(static_cast<int>(RegisterStatusIndex::goto_register));
}

void MainWindow::ShowRegisterSuccessPageView()
{
    m_optmainpage->Selected(true, false);
    m_mainAni_tablayout->SelectItem(static_cast<int>(AppPageIndex::mainpage));
    m_mainpage_tablayout->SelectItem(static_cast<int>(MainPageIndex::registerview));
    m_register_tab->SelectItem(static_cast<int>(RegisterStatusIndex::registerSuccess));
}

void MainWindow::ShowLoginPageView()
{
    m_loginView->ClearAllTips();
    m_optmainpage->Selected(true, false);
    m_mainAni_tablayout->SelectItem(static_cast<int>(AppPageIndex::mainpage));
    m_mainpage_tablayout->SelectItem(static_cast<int>(MainPageIndex::loginview));
}

void MainWindow::ShowFindPasswordPageView()
{
    m_optmainpage->Selected(true, false);
    m_mainAni_tablayout->SelectItem(static_cast<int>(AppPageIndex::mainpage));
    m_mainpage_tablayout->SelectItem(static_cast<int>(MainPageIndex::findpwdback));
}

void MainWindow::ShowMainPageViewWithLogout()
{
    auto makeExitTheAccount = [this](MsgBoxRet ret) {
        if (ret != MB_YES)
            return;

        m_loginView->SetLogoutStatus();
        m_optmainpage->Selected(true, false);
        m_mainAni_tablayout->SelectItem(static_cast<int>(AppPageIndex::mainpage));
        m_mainpage_tablayout->SelectItem(static_cast<int>(MainPageIndex::mainview));
        m_login_status_tab->SelectItem(static_cast<int>(LoginStatusIndex::logout));
        m_header_logintitle_tab->SelectItem(static_cast<int>(LoginStatusIndex::logout));
    };

    ShowMsgBox(m_hWnd, makeExitTheAccount, L"是否退出当前账号？");
}

void MainWindow::ShowMainPageViewWithLogin()
{
    m_optmainpage->Selected(true, false);
    m_mainAni_tablayout->SelectItem(static_cast<int>(AppPageIndex::mainpage));
    m_mainpage_tablayout->SelectItem(static_cast<int>(MainPageIndex::mainview));
    m_login_status_tab->SelectItem(static_cast<int>(LoginStatusIndex::login));
    m_header_logintitle_tab->SelectItem(static_cast<int>(LoginStatusIndex::login));

    SayHelloToUser();
    m_onekeyDeclarateView->SetUserTaxNo(m_loginView->GetCurrentUserCompanyTaxno());
    m_onekeyDeclarateView->SetUserCompanyName(m_loginView->GetCurrentUserCompanyTaxno());
    ShowUserCompanyTaxNO();
    ShowInvalidDate();
}

void MainWindow::ShowPersonCenterPageView()
{
    if (!m_loginView->IsLogInSuccess()) {

        ::KillTimer(m_hWnd, TIMER_RESET_STATUSBAR_MSG);
        HideStatusBarTip();
        ShowStatusBarTip(L"请先登录!");
        return;
    }

    m_optmainpage->Selected(false, false);

    m_mainAni_tablayout->SelectItem(static_cast<int>(AppPageIndex::personcenterpage));
    m_personalCenterView->ShowInfo(m_loginView->GetCurrentUserId(),
                                   m_loginView->GetCurrentUserCompanyTaxno());
}

void MainWindow::BindButtonWithCallback()
{
    m_buttonsCallbackMap.emplace(L"BtnRegister",
                                 std::bind(&MainWindow::ShowRegisterPageView, this));
    m_buttonsCallbackMap.emplace(L"btn_startreg",
                                 std::bind(&MainWindow::ShowRegisterPageView, this));

    m_buttonsCallbackMap.emplace(L"btn_reg_quitelogin",
                                 std::bind(&MainWindow::ShowLoginPageView, this));
    m_buttonsCallbackMap.emplace(L"BtnLogin",
                                 std::bind(&MainWindow::ShowLoginPageView, this));
    m_buttonsCallbackMap.emplace(
        L"BtnLogout", std::bind(&MainWindow::ShowMainPageViewWithLogout, this));
    m_buttonsCallbackMap.emplace(L"btn_forgetpwd",
                                 std::bind(&MainWindow::ShowFindPasswordPageView, this));
    m_buttonsCallbackMap.emplace(L"btn_back_to_login",
                                 std::bind(&MainWindow::ShowLoginPageView, this));

    m_buttonsCallbackMap.emplace(L"btn_close", [this]() { ShowWindow(false, false); });
    m_buttonsCallbackMap.emplace(L"btn_more_set",
                                 std::bind(&MainWindow::OnClickMoreSet, this));
}

void MainWindow::ShowStatusBarTip(const std::wstring &message)
{
    ::KillTimer(m_hWnd, TIMER_RESET_STATUSBAR_MSG);
    auto _warningAniTabLayout =
        dynamic_cast<CAnimationTabLayoutUI *>(m_pm.FindControl(L"warningAniTabLayout"));

    auto _labMainwarnmsg = dynamic_cast<CLabelUI *>(m_pm.FindControl(L"labMainwarnmsg"));
    _warningAniTabLayout->SelectItem(static_cast<int>(1));
    _labMainwarnmsg->SetText(message.data());
    ::SetTimer(m_hWnd, TIMER_RESET_STATUSBAR_MSG, 3000, nullptr);
}

void MainWindow::HideStatusBarTip()
{
    auto _warningAniTabLayout =
        dynamic_cast<CAnimationTabLayoutUI *>(m_pm.FindControl(L"warningAniTabLayout"));
    _warningAniTabLayout->SelectItem(static_cast<int>(0));
}

void MainWindow::OnTrayIcon(LPARAM lParam)
{
    if (lParam == WM_LBUTTONDOWN) {
        ShowWindow();
    }
    else if (lParam == WM_RBUTTONDOWN) {
        POINT pt;
        GetCursorPos(&pt);
        SetForegroundWindow(m_hWnd);
        HMENU hMenu;
        hMenu = CreatePopupMenu();
        AppendMenu(hMenu, MF_STRING, TRAY_EXIT_APP, _T("退出"));
        int cmd = TrackPopupMenu(hMenu, TPM_RETURNCMD, pt.x, pt.y, NULL, m_hWnd, NULL);
        if (cmd == TRAY_EXIT_APP) {
            PostMessage(WM_CLOSE);
        }
    }
}

void MainWindow::CreateTray()
{
    m_trayIcon = std::make_unique<CTrayIcon>();
    m_trayIcon->CreateTrayIcon(m_hWnd, IDC_LOGO, kAppName.data());
}

void MainWindow::ShowRechargePageView()
{
    ::KillTimer(m_hWnd, TIMER_RESET_STATUSBAR_MSG);
    HideStatusBarTip();
    m_mainAni_tablayout->SelectItem(static_cast<int>(AppPageIndex::rechargepage));
}

void MainWindow::ShowOnekeyDeclaratePageView()
{
    ::KillTimer(m_hWnd, TIMER_RESET_STATUSBAR_MSG);
    HideStatusBarTip();
    m_mainAni_tablayout->SelectItem(static_cast<int>(AppPageIndex::okdeclarationpage));
    auto p_declarTab = dynamic_cast<ui::CTabLayoutUI *>(m_pm.FindControl(L"declarTab"));
    assert(p_declarTab != nullptr);
    p_declarTab->SelectItem(0);
}

void MainWindow::ShowSiheAccountHelperPageView()
{
    ::KillTimer(m_hWnd, TIMER_RESET_STATUSBAR_MSG);
    HideStatusBarTip();
    // every time we enter sihe page ,we should check the status first
    if (!m_siheAccountHelperView->CheckAccountStatus(m_loginView->GetCurrentUserId())) {
        ShowStatusBarTip(L"您企业尚未缴费，请充值后使用");
        return;
    }

    m_siheAccountHelperView->SetSiheUserData(
        m_dbHandle->GetSiheAccountInfo(m_loginView->GetCurrentUserPhone()));
    m_mainAni_tablayout->SelectItem(
        static_cast<int>(AppPageIndex::siheaccounterhelperpage));
}

void MainWindow::OnOptSelectChanged(const std::wstring &name)
{
    if (name == L"optsiheAccouter" || name == L"optonekeydeclarate") {

        if (!m_loginView->IsLogInSuccess()) {

            ::KillTimer(m_hWnd, TIMER_RESET_STATUSBAR_MSG);
            HideStatusBarTip();
            ShowStatusBarTip(L"请先登录!");
            return;
        }
        else {
            if (name == L"optsiheAccouter")
                ShowSiheAccountHelperPageView();
            else if (name == L"optonekeydeclarate")
                ShowOnekeyDeclaratePageView();
        }
    }

    else if (name == L"optrecharge")
        ShowRechargePageView();
}

void MainWindow::OnClickMoreSet()
{
    RECT rect, rcPos;
    GetWindowRect(m_hWnd, &rect);
    auto p_btn_more_set = dynamic_cast<CButtonUI *>(m_pm.FindControl(L"btn_more_set"));
    assert(p_btn_more_set != nullptr);
    rcPos = p_btn_more_set->GetPos();

    POINT pt = {rect.left + rcPos.left - 70, rect.top + rcPos.bottom + 15};

    CMenuWnd::DestroyMenu();
    CMenuWnd::CreateMenu(NULL, _T("menu.xml"), pt, &m_pm);
}

void MainWindow::OnClickMenu(const std::wstring &name)
{
    if (name == L"personalcenter") {
        ShowPersonCenterPageView();
    }
    if (name == L"menu_help") {
        ShowHelpDoc();
    }
    else if (name == L"menu_about") {
        ShowAppAbout();
    }
}

void MainWindow::ShowHelpDoc()
{
}

void MainWindow::ShowAppAbout()
{
    ModalWnd::ShowAboutWnd(m_hWnd);
}

void MainWindow::SayHelloToUser()
{
    auto _lab_user_login_info =
        dynamic_cast<ui::CLabelUI *>(m_pm.FindControl(L"lab_user_login_info"));
    assert(_lab_user_login_info != nullptr);
    auto unicodeUserName =
        ckbase::win32::MBCSToUnicode(m_loginView->GetCurrentUserName());
    auto sayHelloToUser = unicodeUserName + L"，你好！";
    _lab_user_login_info->SetText(sayHelloToUser.c_str());
}

void MainWindow::ShowUserCompanyTaxNO()
{
    auto _lab_taxNo = dynamic_cast<ui::CLabelUI *>(m_pm.FindControl(L"lab_taxNo"));
    assert(_lab_taxNo != nullptr);
    auto unicodeCompanyTaxno =
        ckbase::win32::MBCSToUnicode(m_loginView->GetCurrentUserCompanyTaxno());
    _lab_taxNo->SetText(unicodeCompanyTaxno.c_str());
}

void MainWindow::ShowInvalidDate()
{
    auto _lab_siheAuthInvalidDate =
        dynamic_cast<ui::CLabelUI *>(m_pm.FindControl(L"lab_siheAuthInvalidDate"));
    auto _lab_helperInvalidDate =
        dynamic_cast<ui::CLabelUI *>(m_pm.FindControl(L"lab_helperInvalidDate"));

    if (m_loginView->IsNeedShowSiheValidDate()) {
        std::string siheDateShowLab{"有效期至（{c #40A1D1}财务版{c}）：{c #FF6670}"};
        siheDateShowLab += m_loginView->GetSiheValidDate();
        siheDateShowLab += "{c}";

        auto unicodeSiheInvalidDate = ckbase::win32::MBCSToUnicode(siheDateShowLab);
        _lab_siheAuthInvalidDate->SetText(unicodeSiheInvalidDate.c_str());
    }

    if (m_loginView->IsNeedShowHelperValidDate()) {
        std::string helpDateShowLab{"有效期至（{c #40A1D1}助手版{c}）：{c #FF6670}"};
        helpDateShowLab += m_loginView->GetHelperValidDate();
        helpDateShowLab += "{c}";

        auto unicodeHelpDateShowLab = ckbase::win32::MBCSToUnicode(helpDateShowLab);
        _lab_siheAuthInvalidDate->SetText(unicodeHelpDateShowLab.c_str());
    }
}

void MainWindow::InitDB()
{
    m_dbHandle = std::make_unique<InternalDB>();
    auto appRunningPath = AppEnvironment::GetRunningPath();
    auto dbPath = appRunningPath + L"db.db";
    // mark  初始化数据库的路径必须是Utf-8编码的
    auto ansi_path = ckbase::win32::UnicodeToUTF8(dbPath);

    if (!m_dbHandle->Init(ansi_path)) {
        ckbase::Warn("DB File Init failed!");
    }
}
