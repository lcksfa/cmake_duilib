#include "pch.h"
#include <functional>
#include "PersonalCenterView.h"
#include "ModalWnd.hpp"
#include "toast.h"
#include "closure/closure.h"
#include "win32/string_code_convert.h"
#include "module/Register.h"
#include "MsgBox.h"
#include "async_modal/AsyncFileDialog.h"
#include "win32/file_utils.h"
#include "ImageBox.h"
#include "module/GlobalSetting.h"

// I don't know why when I use function callable object ,
// VS2015 complie this warning ,so
// disable it first
#pragma warning(disable : 4503)
// #ifdef _DEBUG
// constexpr auto url_image_upload = "http://192.168.5.208:8180/bzt/bztUpload";
// #else
// constexpr auto url_image_upload = "http://bzt.580kp.com/bzt/bztUpload";

// #endif //

PersonalCenterView::PersonalCenterView(ui::CPaintManagerUI *uiManager, HWND hwnd)
    : m_uiManager(uiManager)
    , m_ownerHwnd(hwnd)
{
    m_uiManager->AddNotifier(this);

    auto DFindAndCheckComboPtr = [this](const std::wstring &comboName) -> ui::CComboUI * {
        auto *comboPtr =
            dynamic_cast<ui::CComboUI *>(m_uiManager->FindControl(comboName.c_str()));
        assert(comboPtr);
        return comboPtr;
    };

    auto DFindAndCheckLabelPtr = [=](const std::wstring &labelName) -> ui::CLabelUI * {
        auto *labelptr =
            dynamic_cast<ui::CLabelUI *>(m_uiManager->FindControl(labelName.c_str()));
        assert(labelptr);
        return labelptr;
    };

    m_labPCName = DFindAndCheckLabelPtr(L"labPCName");
    m_labPCPhoneNo = DFindAndCheckLabelPtr(L"labPCPhoneNo");
    m_labPCTaxerType = DFindAndCheckLabelPtr(L"labPCTaxerType");
    m_labPCCheckStatue = DFindAndCheckLabelPtr(L"labPCCheckStatue");
    m_labPCCompanyName = DFindAndCheckLabelPtr(L"labPCCompanyName");
    m_labPCTaxNo = DFindAndCheckLabelPtr(L"labPCTaxNo");

    m_comboPCProvince = DFindAndCheckComboPtr(L"comboPCProvince");
    m_comboPCCity = DFindAndCheckComboPtr(L"comboPCCity");

    m_orderPCList = dynamic_cast<ui::CListUI *>(m_uiManager->FindControl(L"orderPCList"));
    assert(m_orderPCList != nullptr);
}

void PersonalCenterView::RequestUserBasicInfo(const std::string &usrId)
{
    std::function<UnorderedStringMap(void)> reqBasicInfoTask =
        [=]() -> UnorderedStringMap {
        return m_personalCenter_Logic->DoRequestUserBasicInfo(usrId);
    };

    HttpResponseFuncType reqReply = [this](const UnorderedStringMap &respRet) {
        if (respRet.find("success")->second == "true")
            ShowUserBasicInfo(respRet);
        else {
            Toast::Show(L"获取用户信息失败,请稍后重试", 3000, m_ownerHwnd);
        }
    };
    Await(ThreadId::Misc, reqBasicInfoTask, reqReply);
}

void PersonalCenterView::RequestOrderList(const std::string &taxno)
{
    HttpRequestFuncType reqOrderTask = [=]() -> UnorderedStringMap {
        return m_personalCenter_Logic->DoRequestUserOrders(taxno);
    };

    HttpResponseFuncType reqReply = [this](const UnorderedStringMap &respRet) {
        ModalWnd::CloseModal();
        if (respRet.find("success")->second == "true")
            ShowOrderList(respRet);
        else {
            Toast::Show(L"获取订单信息失败,请稍后重试", 3000, m_ownerHwnd);
        }
    };
    Await(ThreadId::Misc, reqOrderTask, reqReply);
}

void PersonalCenterView::ShowUserBasicInfo(const UnorderedStringMap &respRet)
{
    auto SetLabelText = [this](ui::CLabelUI *labelPtr, const std::string &infoItem) {
        auto unicodeMsg = ckbase::win32::MBCSToUnicode(infoItem);
        labelPtr->SetText(unicodeMsg.c_str());
    };

    auto GetMapValue = [respRet](const std::string &key) -> std::string {
        return respRet.find(key)->second;
    };

    m_licenseUrl = GlobalSetting::GetImageUploadUrl() + GetMapValue("licensePath");
    SetLabelText(m_labPCName, GetMapValue("username"));
    SetLabelText(m_labPCPhoneNo, GetMapValue("phone"));
    auto taxType = GetMapValue("taxType");
    std::string taxTypeExplanation;
    if (taxType == "01")
        taxTypeExplanation = "小规模纳税人";
    else if (taxType == "02")
        taxTypeExplanation = "一般纳税人";
    else
        taxTypeExplanation = "未知类别";

    auto auditStatus = GetMapValue("auditStatus");
    std::string auditStatusExplanation;
    if (auditStatus == "01")
        auditStatusExplanation = "待审核";
    else if (auditStatus == "02")
        auditStatusExplanation = "通过";
    else if (auditStatus == "03")
        auditStatusExplanation = "不通过";
    else
        auditStatusExplanation = "未知状态";

    SetLabelText(m_labPCTaxerType, taxTypeExplanation);
    SetLabelText(m_labPCCheckStatue, auditStatusExplanation);
    SetLabelText(m_labPCCompanyName, GetMapValue("entName"));
    SetLabelText(m_labPCTaxNo, GetMapValue("taxNo"));

    Register controller;
    AddComboItem(m_comboPCCity, controller.GetCityNameByCode(GetMapValue("city")));
    AddComboItem(m_comboPCProvince,
                 controller.GetProvinceNameByCode(GetMapValue("province")));

    m_comboPCCity->SelectItem(0);
    m_comboPCProvince->SelectItem(0);
}

void PersonalCenterView::AddListItem(const OrderListInternal &orderItem)
{
    ui::CListTextElementUI *pListElement = new ui::CListTextElementUI;
    pListElement->SetFixedHeight(50);
    m_orderPCList->Add(pListElement);
    auto textConverter = [](const std::string &text) {
        return ckbase::win32::MBCSToUnicode(text).c_str();
    };
    pListElement->SetText(0, textConverter(orderItem.orderNo));
    pListElement->SetText(1, textConverter(orderItem.phone));
    pListElement->SetText(2, textConverter(orderItem.amount));
    pListElement->SetText(3, textConverter(orderItem.createTime));
    pListElement->SetText(4, textConverter(orderItem.startDate));
    pListElement->SetText(5, textConverter(orderItem.endDate));
}

void PersonalCenterView::ShowOrderList(const UnorderedStringMap &respRet)
{
    auto orderList = m_personalCenter_Logic->ParseOrderList(respRet);
    for (auto &&item : orderList) {
        AddListItem(item);
    }
}

void PersonalCenterView::OnBtnClick(const std::wstring &btnName)
{
    if (btnName == L"btnPCSaveReginCode") {
        ModifyUserAreaInfo();
    }

    else if (btnName == L"BtnPCChangeBusinessll") {
        ChangeBusinessLincenseImage();
    }
    else if (btnName == L"btnPCShowReason") {
        CheckForAuditReason();
    }
}

void PersonalCenterView::EnlargeBusinessImage()
{
    ModalWnd::ShowWaiting(m_ownerHwnd);
    std::function<std::string(void)> businessimageDownloadTask = [this]() {
        ckbase::Info("Downloading image in [{}]", m_licenseUrl);
        return m_personalCenter_Logic->DownloadBussinessImage(m_licenseUrl);
    };

    std::function<void(std::string)> downloadRetReply =
        [this](const std::string &imagePath) {
            ModalWnd::CloseModal();
            if (!imagePath.empty()) {
                auto unicodeImagePath = ckbase::win32::MBCSToUnicode(imagePath);
                ShowImageBox(m_ownerHwnd, unicodeImagePath);
            }
            else {
                Toast::Show(L"营业执照图片获取失败", 2000, m_ownerHwnd);
            }
        };

    Await(ThreadId::Misc, businessimageDownloadTask, downloadRetReply);
}

void PersonalCenterView::OnSelectBusinessLicensePath(BOOL IsRetSuccess,
                                                     const std::wstring &filePath)
{
    if (!IsRetSuccess)
        return;

    auto sizeOfFile = ckbase::GetFileSize(filePath);
    if (sizeOfFile > 5 * 1024 * 1024) {
        Toast::Show(L"文件大小不能超过5M", 3000, m_ownerHwnd);
        return;
    }

    std::function<UnorderedStringMap(void)> changeImageRequestTask =
        [=]() -> UnorderedStringMap {
        auto uft8_filePath = ckbase::win32::UnicodeToUTF8(filePath);
        return m_personalCenter_Logic->DoRequestModifyBussinessLincenseImage(
            m_userId, uft8_filePath);
    };

    std::function<void(UnorderedStringMap)> respReply =
        [this](const UnorderedStringMap &ret) {
            if (ret.find("success")->second == "true") {
                Toast::Show(L"修改成功！", 1000, m_ownerHwnd);

                auto _btn_select_businessll = dynamic_cast<ui::CButtonUI *>(
                    m_uiManager->FindControl(L"BtnPCChangeBusinessll"));
                _btn_select_businessll->SetNormalImage(
                    L"file='public\\button\\ul_del_normal.png'");
                _btn_select_businessll->SetHotImage(
                    L"file='public\\button\\ul_del_hover.png'");
                _btn_select_businessll->SetPushedImage(
                    L"file='public\\button\\ul_del_pushed.png'");
            }
            else {
                Toast::Show(L"修改失败！", 1000, m_ownerHwnd);
            }
        };

    Await(ThreadId::Misc, changeImageRequestTask, respReply);
}

void PersonalCenterView::ChangeBusinessLincenseImage()
{
    //当我们使用异步文件对话框时，只能在堆上分配内存 ！
    auto *file_dialog = new AsyncFileDialog;

    std::map<LPCTSTR, LPCTSTR> filters;
    filters[L"图像文件(*.jpg)"] = L"*.jpg";
    filters[L"图像文件(*.jpeg)"] = L"*.jpeg";
    filters[L"图像文件(*.png)"] = L"*.png";
    filters[L"图像文件(*.bmp)"] = L"*.bmp";
    file_dialog->SetFilter(filters);
    file_dialog->SetParentWnd(m_ownerHwnd);

    AsyncFileDialog::FileDialogCallback callback =
        std::bind(&PersonalCenterView::OnSelectBusinessLicensePath, this,
                  std::placeholders::_1, std::placeholders::_2);

    file_dialog->AsyncShowOpenFileDlg(callback);
}

void PersonalCenterView::CheckForAuditReason()
{
    HttpRequestFuncType auditRemarkRequestTask = [this]() -> UnorderedStringMap {
        return m_personalCenter_Logic->DoRequestAuditReasons(m_userId);
    };

    HttpResponseFuncType auditRemarkReply = [this](const UnorderedStringMap &ret) {
        if (ret.find("success")->second == "true") {
            auto auditRemark = ret.find("auditRemark")->second;
            auto unicodeAuditRemark = ckbase::win32::MBCSToUnicode(auditRemark);
            ShowMsgBox(m_ownerHwnd, MsgboxCallback(), unicodeAuditRemark);
        }
        else {
            Toast::Show(L"获取失败！", 3000, m_ownerHwnd);
        }
    };

    Await(ThreadId::Misc, auditRemarkRequestTask, auditRemarkReply);
}

void PersonalCenterView::Notify(ui::TNotifyUI &msg)
{
    auto ctrlName = msg.pSender->GetName();

    if (msg.sType == DUI_MSGTYPE_CLICK) {
        OnBtnClick(ctrlName.GetData());
    }
    else if (msg.sType == DUI_MSGTYPE_DBCLICK) {
        if (ctrlName == L"BtnPCEnlargeBusinessll") {
            EnlargeBusinessImage();
        }
    }
    else if (msg.sType == DUI_MSGTYPE_PREDROPDOWN) {
        if (ctrlName == L"comboPCProvince") {
            m_comboPCProvince->RemoveAll();
            AddAllProvinceItem();
            m_comboPCProvince->SelectItem(0);

            m_comboPCCity->RemoveAll();
            AddComboItem(m_comboPCCity, L"请选择市");
            m_comboPCCity->SelectItem(0);
        }
        else if (ctrlName == L"comboPCCity") {
            m_comboPCCity->RemoveAll();
            AddAllCitiesByProvinceName(m_comboPCProvince->GetText().GetData());
            m_comboPCCity->SelectItem(0);
        }
    }
}

void PersonalCenterView::AddComboItem(ui::CComboUI *comboPtr,
                                      const std::wstring &itemText)
{
    ui::CListLabelElementUI *elem = new ui::CListLabelElementUI();
    elem->SetText(itemText.c_str());
    comboPtr->Add(elem);
}

void PersonalCenterView::AddAllProvinceItem()
{
    Register register_logic;
    auto provinceList = register_logic.GetProviceList();
    for (auto proviceItem : provinceList) {
        AddComboItem(m_comboPCProvince, proviceItem);
    }
}

void PersonalCenterView::AddAllCitiesByProvinceName(const std::wstring &provinceName)
{
    Register register_logic;

    auto citiesList = register_logic.GetCityListOf(provinceName);
    for (auto cityItem : citiesList) {
        AddComboItem(m_comboPCCity, cityItem);
    }
}

void PersonalCenterView::ModifyUserAreaInfo()
{
    ModalWnd::ShowWaiting(m_ownerHwnd);
    auto unicodeProvince = m_comboPCProvince->GetText().GetData();
    auto unicodeCity = m_comboPCCity->GetText().GetData();

    Register controller;
    auto province_code =
        controller.GetProvinceCodeByName(ckbase::win32::UnicodeToMBCS(unicodeProvince));
    auto city_code =
        controller.GetCityCodeByName(ckbase::win32::UnicodeToMBCS(unicodeCity));

    HttpRequestFuncType reqOrderTask = [=]() -> UnorderedStringMap {
        return m_personalCenter_Logic->DoRequestModifyUserAreaInfo(
            m_userId, province_code, city_code);
    };

    HttpResponseFuncType respReply = [this](const UnorderedStringMap &ret) {
        ModalWnd::CloseModal();
        if (ret.find("success")->second == "true") {
            Toast::Show(L"保存成功！", 1000, m_ownerHwnd);
        }
        else {
            Toast::Show(L"保存失败！", 1000, m_ownerHwnd);
        }
    };
    Await(ThreadId::Misc, reqOrderTask, respReply);
}

void PersonalCenterView::ShowInfo(const std::string &userId, const std::string &taxno)
{
    ModalWnd::ShowWaiting(m_ownerHwnd);
    m_userId = userId;
    RequestUserBasicInfo(userId);
    RequestOrderList(taxno);
}
