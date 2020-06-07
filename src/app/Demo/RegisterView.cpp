/*
 * Written by Li Zhao <bw_lizhao AT whty Dot com Dot cn>.
 * On 2020年5月29日
 * This file use for 注册页面对应的类
 *
 */

#include "pch.h"
#include <vector>
#include "RegisterView.h"
#include "win32\string_code_convert.h"
#include "ModalWnd.hpp"
#include "async_modal/AsyncFileDialog.h"
#include "win32\file_utils.h"
#include "toast.h"

// I don't know why when I use function callable object ,
// VS2015 complie this warning ,so
// disable it first
#pragma warning(disable : 4503)

RegisterView::RegisterView(ui::CPaintManagerUI *uiManager, HWND hwnd)
    : m_uiManager(uiManager)
    , m_ownerHwnd(hwnd)
    , m_register_logic(std::make_unique<Register>())
{
    m_labcompanyfillerror = FetchLabelPtr(L"labcompanyfillerror");
    m_labtaxnofillinerror = FetchLabelPtr(L"labtaxnofillinerror");
    m_labtaxerTypeformaterror = FetchLabelPtr(L"labtaxerTypeformaterror");
    m_labareaformaterror = FetchLabelPtr(L"labareaformaterror");
    m_labnameformaterror = FetchLabelPtr(L"labnameformaterror");
    m_labphoneformaterror = FetchLabelPtr(L"labregphoneformaterror");
    m_labverificationcodeerror = FetchLabelPtr(L"labverificationcodeerror");
    m_labpwdformaterror = FetchLabelPtr(L"labregpwdformaterror");
    m_labphoneregederror = FetchLabelPtr(L"labphoneregederror");

    m_comboRegisterTaxerType = FetchComboPtr(L"comboRegisterTaxerType");
    m_comboRegisterProvince = FetchComboPtr(L"comboRegisterProvince");
    m_comboRegisterCity = FetchComboPtr(L"comboRegisterCity");

    ResetRegisterView();
    m_uiManager->AddNotifier(this);
}

RegisterView::~RegisterView()
{
}

void RegisterView::Notify(ui::TNotifyUI &msg)
{
    auto ctrlName = msg.pSender->GetName();

    if (msg.sType == DUI_MSGTYPE_CLICK) {
        if (ctrlName == L"btn_request_reg")
            OnClickRegister();
        else if (ctrlName == L"btn_fetch_vercode")
            OnClickFetchVerCode();
        else if (ctrlName == L"btn_select_businessll")
            OnClickSelectBusinessLicense();
        else if (ctrlName == L"btn_reg_user_service_protocol") {
            m_register_logic->OpenUserServiceProtocol();
        }
    }
    else if (msg.sType == DUI_MSGTYPE_PREDROPDOWN) {
        if (ctrlName == L"comboRegisterTaxerType") {
            m_comboRegisterTaxerType->RemoveAll();
            AddComboItem(m_comboRegisterTaxerType, L"小规模纳税人");
            AddComboItem(m_comboRegisterTaxerType, L"一般纳税人");
            m_comboRegisterTaxerType->SelectItem(0);
        }
        else if (ctrlName == L"comboRegisterProvince") {
            m_comboRegisterProvince->RemoveAll();
            AddAllProvinceItem();
            m_comboRegisterProvince->SelectItem(0);

            m_comboRegisterCity->RemoveAll();
            AddComboItem(m_comboRegisterCity, L"请选择市");
            m_comboRegisterCity->SelectItem(0);
        }
        else if (ctrlName == L"comboRegisterCity") {
            m_comboRegisterCity->RemoveAll();
            AddAllCitiesByProvinceName(m_comboRegisterProvince->GetText().GetData());
            m_comboRegisterCity->SelectItem(0);
        }
    }
}

inline ui::CLabelUI *RegisterView::FetchLabelPtr(const std::wstring &labelName)
{
    return dynamic_cast<ui::CLabelUI *>(m_uiManager->FindControl(labelName.data()));
}

inline ui::CEditUI *RegisterView::FetchEditPtr(const std::wstring &editname)
{
    return dynamic_cast<ui::CEditUI *>(m_uiManager->FindControl(editname.data()));
}

inline ui::CComboUI *RegisterView::FetchComboPtr(const std::wstring &comboname)
{
    return dynamic_cast<ui::CComboUI *>(m_uiManager->FindControl(comboname.data()));
}

bool RegisterView::CheckUserFillin()
{
    bool isCheckValid = true;
    isCheckValid = IsFillUpValidCompanyName();
    isCheckValid = IsFillUpValidTaxNO();
    isCheckValid = IsFillUpValidNickName();

    isCheckValid = IsFillUpValidTaxerType();
    isCheckValid = IsFillUpValidArea();
    isCheckValid = IsFillUpValidPhone();
    isCheckValid = IsFillUpValidVerificationcode();

    isCheckValid = IsFillUpValidPassWord();

    return isCheckValid;
}

void RegisterView::ShowErrorTip(regErrorTipType type, const std::wstring &message)
{
    switch (type) {

    case regErrorTipType::company_name:
        m_labcompanyfillerror->SetText(message.data());
        m_labcompanyfillerror->SetVisible(true);
        break;
    case regErrorTipType::name:
        m_labnameformaterror->SetText(message.data());
        m_labnameformaterror->SetVisible(true);
        break;
    case regErrorTipType::phone:
        m_labphoneformaterror->SetText(message.data());
        m_labphoneformaterror->SetVisible(true);
        break;
    case regErrorTipType::taxno:
        m_labtaxnofillinerror->SetText(message.data());
        m_labtaxnofillinerror->SetVisible(true);
        break;
    case regErrorTipType::verificationcode:
        m_labverificationcodeerror->SetText(message.data());
        m_labverificationcodeerror->SetVisible(true);
        break;
    case regErrorTipType::password:
        m_labpwdformaterror->SetText(message.data());
        m_labpwdformaterror->SetVisible(true);
        break;
    case regErrorTipType::area:
        m_labareaformaterror->SetText(message.data());
        m_labareaformaterror->SetVisible(true);
        break;
    case regErrorTipType::taxerType:
        m_labtaxerTypeformaterror->SetText(message.data());
        m_labtaxerTypeformaterror->SetVisible(true);
        break;
    default:
        break;
    }
}

bool RegisterView::IsFillUpValidCompanyName()
{
    bool isCheckValid = true;
    auto p_edit_company_name = FetchEditPtr(L"edit_compony_name");
    if (p_edit_company_name->GetText().IsEmpty()) {
        isCheckValid = false;
        ShowErrorTip(regErrorTipType::company_name, L"企业名称不能为空");
    }
    else {
        m_labcompanyfillerror->SetVisible(false);
    }
    return isCheckValid;
}

bool RegisterView::IsFillUpValidTaxNO()
{
    auto p_edit_taxno = FetchEditPtr(L"edit_taxno");
    if (p_edit_taxno->GetText().IsEmpty()) {
        ShowErrorTip(regErrorTipType::taxno, L"企业税号不能为空");
        return false;
    }
    bool isCheckValid = true;

    if (!m_register_logic->IsValidTaxNO(p_edit_taxno->GetText().GetData())) {
        ShowErrorTip(regErrorTipType::taxno, L"企业税号格式不正确");
        isCheckValid = false;
    }
    else {
        m_labtaxnofillinerror->SetVisible(false);
    }
    return isCheckValid;
}

bool RegisterView::IsFillUpValidTaxerType()
{
    if (m_comboRegisterTaxerType->GetText() == L"请选择纳税人类别") {
        ShowErrorTip(regErrorTipType::taxerType, L"请选择纳税人类别");
        return false;
    }
    else {
        m_labtaxerTypeformaterror->SetVisible(false);
    }
    return true;
}

bool RegisterView::IsFillUpValidArea()
{
    if (m_comboRegisterProvince->GetText() == L"请选择省" ||
        m_comboRegisterCity->GetText() == L"请选择市") {

        ShowErrorTip(regErrorTipType::area, L"地区不能为空");
        return false;
    }
    else {
        m_labareaformaterror->SetVisible(false);
    }

    return true;
}

bool RegisterView::IsFillUpValidNickName()
{
    auto p_edit_name = FetchEditPtr(L"edit_name");
    if (p_edit_name->GetText().IsEmpty()) {
        ShowErrorTip(regErrorTipType::name, L"姓名不能为空");
        return false;
    }
    bool isCheckValid = true;

    if (!m_register_logic->IsValidName(p_edit_name->GetText().GetData())) {
        ShowErrorTip(regErrorTipType::name, L"姓名格式不正确");
        isCheckValid = false;
    }
    else {
        m_labnameformaterror->SetVisible(false);
    }
    return isCheckValid;
}

bool RegisterView::IsFillUpValidVerificationcode()
{
    bool isCheckValid = true;
    auto p_edit_verificationcode = FetchEditPtr(L"edit_vercode");
    auto code = p_edit_verificationcode->GetText().GetData();
    if (!m_register_logic->IsValidVerificationcode(code)) {
        ShowErrorTip(regErrorTipType::verificationcode, L"验证码不能为空");
        isCheckValid = false;
    }
    else {
        m_labverificationcodeerror->SetVisible(false);
    }
    return isCheckValid;
}

bool RegisterView::IsFillUpValidPassWord()
{
    bool isCheckValid = true;
    auto p_edit_pwd = FetchEditPtr(L"edit_pwd");
    auto pwd = std::wstring{p_edit_pwd->GetText().GetData()};
    if (pwd.empty()) {
        ShowErrorTip(regErrorTipType::password, L"密码不能为空");
        return false;
    }

    if (!m_register_logic->IsValidPassword(pwd)) {
        ShowErrorTip(regErrorTipType::password, L"密码格式不正确");
        isCheckValid = false;
    }
    else {
        m_labpwdformaterror->SetVisible(false);
    }
    return isCheckValid;
}

bool RegisterView::OnClickRegister()
{
    if (!CheckUserFillin())
        return false;

    ModalWnd::ShowWaiting(m_ownerHwnd);

    HttpRequestFuncType registerRequestTask = [this]() -> UnorderedStringMap {
        auto parms = GetRegisterRequestParms();

        return m_register_logic->DoRequest(parms);
    };

    HttpResponseFuncType registerRetReply = [this](const UnorderedStringMap &ret) {
        ModalWnd::CloseModal();
        if (ret.at("success") == "true") {
            m_userID = ret.find("userId")->second;
            auto uft8_businessLicensePath =
                ckbase::win32::UnicodeToUTF8(m_businessLicensePath);
            m_register_logic->UploadBusinessLicense(uft8_businessLicensePath, m_userID);

            ui::TNotifyUI notify_one;
            notify_one.sType = Constom_UI_reg_Success;
            notify_one.pSender = m_uiManager->FindControl(L"btn_request_reg");

            m_uiManager->SendNotify(notify_one, true);
        }
        else {
            auto errorMsg = ret.at("msg");
            auto unicodeMsg = ckbase::win32::MBCSToUnicode(errorMsg);
            ShowErrorTip(regErrorTipType::phonereged, unicodeMsg);
            Toast::Show(L"注册失败，请稍候重试", 3000, m_ownerHwnd);
        }
    };

    Await(ThreadId::Misc, registerRequestTask, registerRetReply);
    return true;
}

void RegisterView::OnClickFetchVerCode()
{
    auto p_edit_phone_number = FetchEditPtr(L"edit_phoneno");
    std::wstring phone{p_edit_phone_number->GetText().GetData()};
    if (phone.empty()) {
        ShowErrorTip(regErrorTipType::phone, L"电话号码不能为空");
        return;
    }
    if (!m_register_logic->IsValidPhone(phone)) {
        ShowErrorTip(regErrorTipType::phone, L"手机号格式不正确");
        return;
    }
    auto p_btn_fetch_vercode =
        dynamic_cast<ui::CButtonUI *>(m_uiManager->FindControl(L"btn_fetch_vercode"));
    p_btn_fetch_vercode->SetEnabled(false);
    p_btn_fetch_vercode->SetBkColor(0xFFC4C4C4);

    auto countdownrepeattask = [=]() {
        static int countdownsecond = 60;
        std::wstring secondLeft = fmt::format(L"{}秒后重新获取", --countdownsecond);
        p_btn_fetch_vercode->SetText(secondLeft.c_str());
        if (countdownsecond == 0) {
            countdownsecond = 60;
            p_btn_fetch_vercode->SetEnabled(true);
            p_btn_fetch_vercode->SetText(L"获取短信验证码");
            p_btn_fetch_vercode->SetBkColor(0xFFFF6673);
        }
    };
    // 60秒倒计时
    PostRepeatedTask2UI(ToWeakCallback(countdownrepeattask),
                        ckbase::TimeDelta::FromMilliseconds(1000), 60);

    auto ansi_phone = ckbase::win32::UnicodeToMBCS(phone);

    HttpRequestFuncType reqVerifiCodeTask = [=]() -> UnorderedStringMap {
        return m_register_logic->RequestFetchVerCode(ansi_phone);
    };

    HttpResponseFuncType responseVerifiCodeTask =
        [=](const UnorderedStringMap &response) {
            if (response.at("success") == "true") {
                Toast::Show(L"验证码已发送", 2000, m_ownerHwnd);
            }
            else {
                auto unicodeMsg = ckbase::win32::MBCSToUnicode(response.at("msg"));
                Toast::Show(unicodeMsg, 2000, m_ownerHwnd);
            }
        };

    Await(ThreadId::Misc, reqVerifiCodeTask, responseVerifiCodeTask);
}

void RegisterView::OnSelectBusinessLicensePath(BOOL IsRetSuccess,
                                               const std::wstring &filePath)
{
    if (!IsRetSuccess)
        return;

    auto sizeOfFile = ckbase::GetFileSize(filePath);
    if (sizeOfFile > 5 * 1024 * 1024) {
        Toast::Show(L"文件大小不能超过5M", 3000, m_ownerHwnd);
        return;
    }
    m_businessLicensePath = filePath;

    auto _btn_select_businessll =
        dynamic_cast<ui::CButtonUI *>(m_uiManager->FindControl(L"btn_select_businessll"));
    ckbase::Info(L"Get business license path: {}", m_businessLicensePath);
    _btn_select_businessll->SetNormalImage(L"file='public\\button\\ul_del_normal.png'");
    _btn_select_businessll->SetHotImage(L"file='public\\button\\ul_del_hover.png'");
    _btn_select_businessll->SetPushedImage(L"file='public\\button\\ul_del_pushed.png'");
}

bool RegisterView::OnClickSelectBusinessLicense()
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
        std::bind(&RegisterView::OnSelectBusinessLicensePath, this, std::placeholders::_1,
                  std::placeholders::_2);

    file_dialog->AsyncShowOpenFileDlg(callback);
    return true;
}

void RegisterView::OnClickUserServiceProtocol()
{
    m_register_logic->OpenUserServiceProtocol();
}

void RegisterView::AddComboItem(ui::CComboUI *comboPtr, const std::wstring &itemText)
{
    ui::CListLabelElementUI *elem = new ui::CListLabelElementUI();
    elem->SetText(itemText.c_str());
    comboPtr->Add(elem);
}

void RegisterView::AddAllProvinceItem()
{
    auto provinceList = m_register_logic->GetProviceList();
    for (auto proviceItem : provinceList) {
        AddComboItem(m_comboRegisterProvince, proviceItem);
    }
}

void RegisterView::AddAllCitiesByProvinceName(const std::wstring &provinceName)
{
    auto citiesList = m_register_logic->GetCityListOf(provinceName);
    if (citiesList.size() == 0) {
        AddComboItem(m_comboRegisterCity, L"请选择市");
        return;
    }
    for (auto cityItem : citiesList) {
        AddComboItem(m_comboRegisterCity, cityItem);
    }
}

RegisterRequestParms RegisterView::GetRegisterRequestParms()
{
    auto p_edit_company_name = FetchEditPtr(L"edit_compony_name");
    auto p_edit_taxno = FetchEditPtr(L"edit_taxno");
    auto p_edit_name = FetchEditPtr(L"edit_name");
    auto p_edit_verificationcode = FetchEditPtr(L"edit_vercode");
    auto p_edit_pwd = FetchEditPtr(L"edit_pwd");
    auto p_edit_phone_number = FetchEditPtr(L"edit_phoneno");

    std::wstring company_name{p_edit_company_name->GetText().GetData()};
    std::wstring taxno{p_edit_taxno->GetText().GetData()};
    std::wstring name{p_edit_name->GetText().GetData()};
    std::wstring verificationcode{p_edit_verificationcode->GetText().GetData()};
    std::wstring password{p_edit_pwd->GetText().GetData()};
    std::wstring phone{p_edit_phone_number->GetText().GetData()};

    std::string prefixOfType{"0"};
    std::string taxerType{prefixOfType +
                          std::to_string(m_comboRegisterTaxerType->GetCurSel() + 1)};

    std::wstring provinceName{m_comboRegisterProvince->GetText().GetData()};
    std::wstring city{m_comboRegisterCity->GetText().GetData()};

    RegisterRequestParms parm;
    parm.entName = ckbase::win32::UnicodeToMBCS(company_name);
    parm.taxNo = ckbase::win32::UnicodeToMBCS(taxno);
    parm.username = ckbase::win32::UnicodeToMBCS(name);
    parm.identifyingCode = ckbase::win32::UnicodeToMBCS(verificationcode);
    parm.password = ckbase::win32::UnicodeToMBCS(password);
    parm.phone = ckbase::win32::UnicodeToMBCS(phone);
    parm.taxType = taxerType;
    parm.province = ckbase::win32::UnicodeToMBCS(provinceName);
    parm.city = ckbase::win32::UnicodeToMBCS(city);

    return parm;
}

void RegisterView::ResetRegisterView()
{
    m_labcompanyfillerror->SetVisible(false);
    m_labtaxnofillinerror->SetVisible(false);
    m_labtaxerTypeformaterror->SetVisible(false);
    m_labareaformaterror->SetVisible(false);
    m_labnameformaterror->SetVisible(false);
    m_labphoneformaterror->SetVisible(false);
    m_labverificationcodeerror->SetVisible(false);
    m_labpwdformaterror->SetVisible(false);
    m_labphoneregederror->SetVisible(false);

    m_comboRegisterTaxerType->RemoveAll();
    AddComboItem(m_comboRegisterTaxerType, L"请选择纳税人类别");
    m_comboRegisterTaxerType->SelectItem(0);

    m_comboRegisterProvince->RemoveAll();
    AddComboItem(m_comboRegisterProvince, L"请选择省");
    m_comboRegisterProvince->SelectItem(0);

    m_comboRegisterCity->RemoveAll();
    AddComboItem(m_comboRegisterCity, L"请选择市");
    m_comboRegisterCity->SelectItem(0);
}

bool RegisterView::IsFillUpValidPhone()
{
    bool isCheckValid = true;
    auto p_edit_phone_number = FetchEditPtr(L"edit_phoneno");

    auto phone = std::wstring{p_edit_phone_number->GetText().GetData()};
    if (phone.empty()) {
        ShowErrorTip(regErrorTipType::phone, L"手机号不能为空");
        return false;
    }

    if (!m_register_logic->IsValidPhone(phone)) {
        ShowErrorTip(regErrorTipType::phone, L"手机号格式不正确");
        isCheckValid = false;
    }
    else {
        m_labphoneformaterror->SetVisible(false);
    }
    return isCheckValid;
}
