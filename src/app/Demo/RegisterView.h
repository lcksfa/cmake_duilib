#ifndef __INCLUDE_REGISTERVIEW_H__
#define __INCLUDE_REGISTERVIEW_H__

#pragma once
#include <string>
#include "UIlib.h"
#include "module\Register.h"
#include "closure\closure.h"

enum class regErrorTipType
{
    company_name,
    taxno,
    taxerType,
    area,
    name,
    phone,
    verificationcode,
    password,
    phonereged, //号码注册返回时的提示
};

constexpr auto Constom_UI_reg_Success = L"user_reg_success_notify";

class RegisterView : public ui::INotifyUI, virtual public ckbase::SupportWeakCallback
{
private:
    /* data */
    ui::CPaintManagerUI *m_uiManager;
    ui::CLabelUI *m_labcompanyfillerror = nullptr;
    ui::CLabelUI *m_labtaxnofillinerror = nullptr;
    ui::CLabelUI *m_labtaxerTypeformaterror = nullptr;
    ui::CLabelUI *m_labareaformaterror = nullptr;
    ui::CLabelUI *m_labnameformaterror = nullptr;
    ui::CLabelUI *m_labphoneformaterror = nullptr;
    ui::CLabelUI *m_labverificationcodeerror = nullptr;
    ui::CLabelUI *m_labpwdformaterror = nullptr;
    ui::CLabelUI *m_labphoneregederror = nullptr;

    ui::CComboUI *m_comboRegisterTaxerType = nullptr;
    ui::CComboUI *m_comboRegisterProvince = nullptr;
    ui::CComboUI *m_comboRegisterCity = nullptr;

    std::unique_ptr<Register> m_register_logic = nullptr;

    HWND m_ownerHwnd;

    std::string m_userID;
    std::wstring m_businessLicensePath;

public:
    RegisterView(ui::CPaintManagerUI *uiManager, HWND hwnd);
    ~RegisterView();
    void ResetRegisterView();

private:
    void Notify(ui::TNotifyUI &msg) final;
    inline ui::CLabelUI *FetchLabelPtr(const std::wstring &labelName);
    inline ui::CEditUI *FetchEditPtr(const std::wstring &editname);
    inline ui::CComboUI *FetchComboPtr(const std::wstring &comboname);
    /**
     * @brief 对用户的输入做检查
     * @param[in]无
     * @return true 说明无误 ，false 说明有问题，将在页面予以提示
     */
    bool CheckUserFillin();

    void ShowErrorTip(regErrorTipType type, const std::wstring &messages);

    /**
     * @brief 对用户输入的各项信息进行检查，一组子函数
     * @param[in] 无
     * @return 同
     */
    bool IsFillUpValidCompanyName();
    bool IsFillUpValidTaxNO();
    bool IsFillUpValidTaxerType();
    bool IsFillUpValidArea();
    bool IsFillUpValidNickName();
    bool IsFillUpValidPhone();
    bool IsFillUpValidVerificationcode();
    bool IsFillUpValidPassWord();

    /**
     * @brief 用户点击注册按钮时的响应函数
     * @param[in]
     * @return 是否注册成功
     */
    bool OnClickRegister();
    /**
     * @brief 用户点击验证码的响应函数
     * @param[in]
     * @return 是否发送验证码
     */
    void OnClickFetchVerCode();

    bool OnClickSelectBusinessLicense();

    void OnClickUserServiceProtocol();

    void AddComboItem(ui::CComboUI *comboPtr, const std::wstring &itemText);

    /**
     * @brief 添加国内所有省到combo中
     * @param[in]
     * @return
     */
    void AddAllProvinceItem();

    /**
     * @brief 将对应省所有的市名添加到combo中
     * @param[in] 省名
     * @return 无
     */
    void AddAllCitiesByProvinceName(const std::wstring &provinceName);

    RegisterRequestParms GetRegisterRequestParms();

    void OnSelectBusinessLicensePath(BOOL IsRetSuccess, const std::wstring &filePath);
};

#endif // __INCLUDE_REGISTERVIEW_H__