#ifndef __INCLUDE_MAINWINDOW_H__
#define __INCLUDE_MAINWINDOW_H__
#pragma once
#include <string>
#include <map>
#include <memory>
#include <functional>
#include "UIlib.h"
#include "LoginView.h"
#include "RegisterView.h"
#include "PersonalCenterView.h"
#include "OnekeyDeclarateView.h"
#include "SiheAccountHelperView.h"
#include "FindPasswordView.h"
#include "closure/weakcallbacksupport.hpp" //in base lib for weak callback support

#include "module/InternalDB.h"

enum class AppPageIndex
{
    mainpage = 0,            //逻辑首页，
    siheaccounterhelperpage, //思和助手界面
    okdeclarationpage,       //一键申报
    rechargepage,            //充值
    personcenterpage,        //个人中心
};

enum class MainPageIndex
{
    mainview = 0, //首页
    registerview, //注册
    loginview,    //登录
    findpwdback   //密码找回
};

enum class LoginStatusIndex
{
    logout = 0, //
    login = 1,  //
};

enum class RegisterStatusIndex
{
    goto_register = 0,   //
    registerSuccess = 1, //
};

using buttonWithCallback = std::function<void()>;

/**
 * @brief 这里继承SupportWeakCallback是为了让其支持弱回调，确保执行前其任务做this指针检查
 */
class MainWindow : public ui::WindowImplBase, public virtual ckbase::SupportWeakCallback
{
public:
    MainWindow() = default;
    virtual ~MainWindow() = default;
    static const std::wstring kClassName;

private:
    ui::CDuiString GetSkinFile() final;
    LPCTSTR GetWindowClassName(void) const final;
    /**
     * @brief 处理Windows系统消息
     * @param[in] 消息id ，左右值（消息附加数据）
     * @return 处理结果，一般返回0
     */
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) final;
    /**
     * @brief 处理用户自定义的Windows系统消息
     * @param[in]同上
     * @return 同上
     */
    LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam,
                                BOOL &bHandled) final;

    void OnFinalMessage(HWND hWnd) final;

    virtual void InitWindow() override;

    virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

    /**
     * @brief
     * 初始化主干控件，这里的设想是在主干类中完成所有的界面跳转，
     * 避免在view类中使用本类的指针去实现，所以在本类初始化这些控件指针是必要的
     * @param[in]无
     * @return 无
     */
    void InitBackboneCtrls();

    /**
     * @brief duilib框架的消息转发处理
     * @param[in]消息包
     * @return null
     */
    void Notify(ui::TNotifyUI &msg);

    void OnBtnClick(const std::wstring &name);

    /**
     * @brief 展示注册页面
     * @param[in]无
     * @return无
     */
    void ShowRegisterPageView();
    /**
     * @brief 展示注册成功页面
     * @param[in]无
     * @return无
     */
    void ShowRegisterSuccessPageView();

    /**
     * @brief 展示登录页面
     * @param[in] 无
     * @return 无
     */
    void ShowLoginPageView();

    void ShowFindPasswordPageView();

    /**
     * @brief 展示未登录时的界面，首次进入或者用户退出时
     * @param[in] 无
     * @return 无
     */
    void ShowMainPageViewWithLogout();

    /**
     * @brief  展示已登录时的界面
     * @param[in]无
     * @return 无
     */
    void ShowMainPageViewWithLogin();

    /**
     * @brief 将button以及其回调响应函数进行绑定
     * @param[in]无
     * @return无
     */
    void ShowPersonCenterPageView();

    void BindButtonWithCallback();
    /**
     * @brief 在左下角做错误提示
     * @param[in] 需要展示给用户的消息
     * @return  无
     */
    void ShowStatusBarTip(const std::wstring &message);
    void HideStatusBarTip();
    /**
     * @brief 托盘事件处理函数
     * @param[in] 消息附加值，指代按钮事件
     * @return 无
     */
    void OnTrayIcon(LPARAM lParam);

    void CreateTray();

    void ShowRechargePageView();

    void ShowOnekeyDeclaratePageView();

    void ShowSiheAccountHelperPageView();

    void OnOptSelectChanged(const std::wstring &name);

    /**
     * @brief 用户点击更多按钮的响应
     * @param[in] 无
     * @return 无
     */
    void OnClickMoreSet();
    /**
     * @brief 响应用户对菜单的点击事件
     * @param[in] 菜单item的名称
     * @return 无
     */
    void OnClickMenu(const std::wstring &name);

    void ShowHelpDoc();
    void ShowAppAbout();

    void SayHelloToUser();
    void ShowUserCompanyTaxNO();
    void ShowInvalidDate();

    void InitDB();

private:
    ui::CTabLayoutUI *m_mainpage_tablayout = nullptr;
    ui::CTabLayoutUI *m_login_status_tab = nullptr;
    ui::CTabLayoutUI *m_header_logintitle_tab = nullptr;
    ui::CTabLayoutUI *m_register_tab = nullptr;
    ui::CAnimationTabLayoutUI *m_mainAni_tablayout = nullptr;
    ui::COptionUI *m_optmainpage = nullptr;

    std::map<std::wstring, buttonWithCallback> m_buttonsCallbackMap;

private:
    std::unique_ptr<LoginView> m_loginView;
    std::unique_ptr<RegisterView> m_registerView;
    std::unique_ptr<PersonalCenterView> m_personalCenterView;
    std::unique_ptr<OnekeyDeclarateView> m_onekeyDeclarateView;
    std::unique_ptr<SiheAccountHelperView> m_siheAccountHelperView;
    std::unique_ptr<FindPasswordView> m_findPasswordView;
    std::unique_ptr<ui::CTrayIcon> m_trayIcon;

    std::unique_ptr<InternalDB> m_dbHandle;
};

class ResourceInitial
{
public:
    ResourceInitial(HINSTANCE hInstance);
    ~ResourceInitial() = default;
    ResourceInitial(const ResourceInitial &) = delete;
    ResourceInitial &operator=(const ResourceInitial &) = delete;

    bool Init();

private:
    bool InitDebugModeResource();
    bool InitReleaseModeResource();
};
#endif // __INCLUDE_MAINWINDOW_H__