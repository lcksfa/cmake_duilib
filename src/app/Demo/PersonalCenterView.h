#ifndef __INCLUDE_PERSONALCENTERVIEW_H__
#define __INCLUDE_PERSONALCENTERVIEW_H__

#pragma once
#include <memory>
#include "UIlib.h"
#include "module/PersonalCenter.h"

class PersonalCenterView : public ui::INotifyUI
{
public:
    PersonalCenterView(ui::CPaintManagerUI *uiManager, HWND hwnd);
    ~PersonalCenterView() = default;
    /**
     * @brief 展示用户信息
     * @param[in]
     * @return
     */
    void ShowInfo(const std::string &userId, const std::string &taxno);

private:
    void Notify(ui::TNotifyUI &msg);

    void RequestUserBasicInfo(const std::string &usrId);
    void RequestOrderList(const std::string &taxno);
    /**
     * @brief 展示用户基本信息
     * @param[in] respRet 向后台获取的数据
     * @return 无
     */
    void ShowUserBasicInfo(const UnorderedStringMap &respRet);

    /**
     * @brief 展示用户订单列表
     * @param[in]
     * @return
     */
    void ShowOrderList(const UnorderedStringMap &respRet);

    void AddComboItem(ui::CComboUI *comboPtr, const std::wstring &itemText);
    void AddAllProvinceItem();
    void AddAllCitiesByProvinceName(const std::wstring &provinceName);
    /**
     * @brief 修改用户的省市信息
     * @param[in]
     * @return
     */
    void ModifyUserAreaInfo();
    void AddListItem(const OrderListInternal &orderItem);
    void OnBtnClick(const std::wstring &btnName);
    /**
     * @brief 将用户的营业执照放大显示
     * @param[in]
     * @return
     */
    void EnlargeBusinessImage();
    void ChangeBusinessLincenseImage();
    /**
     * @brief 查看审核原因，弹窗提示
     * @param[in]
     * @return
     */
    void CheckForAuditReason();

    void OnSelectBusinessLicensePath(BOOL IsRetSuccess, const std::wstring &filePath);

private:
    HWND m_ownerHwnd;
    std::string m_userId;
    std::string m_licenseUrl;
    ui::CPaintManagerUI *m_uiManager;
    std::unique_ptr<PersonalCenter> m_personalCenter_Logic;

    ui::CLabelUI *m_labPCName = nullptr;
    ui::CLabelUI *m_labPCPhoneNo = nullptr;
    ui::CLabelUI *m_labPCTaxNo = nullptr;
    ui::CLabelUI *m_labPCTaxerType = nullptr;
    ui::CLabelUI *m_labPCCheckStatue = nullptr;
    ui::CLabelUI *m_labPCCompanyName = nullptr;
    ui::CComboUI *m_comboPCProvince = nullptr;
    ui::CComboUI *m_comboPCCity = nullptr;
    ui::CListUI *m_orderPCList = nullptr;
};
#endif // __INCLUDE_PERSONALCENTERVIEW_H__