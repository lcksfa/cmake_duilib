#include "MsgBox.h"
#include "closure/closure.h"

void ShowMsgBox(HWND hwnd, MsgboxCallback cb, const std::wstring &content,
                const std::wstring &title, bool IsShowCancelBtn)
{
    MsgBox *msgbox = new MsgBox;
    HWND hWnd = msgbox->Create(hwnd, L"", WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX, 0);
    assert(hWnd != nullptr);
    if (hWnd == NULL)
        return;
    msgbox->SetTitle(title.c_str());
    msgbox->SetContent(content.c_str());
    msgbox->SetButton(IsShowCancelBtn);

    msgbox->Show(hwnd, cb);
}

ui::CDuiString MsgBox::GetSkinFile()
{
    return L"msgbox.xml";
}

LPCTSTR MsgBox::GetWindowClassName(void) const
{
    return L"MsgBoxClass";
}

void MsgBox::OnFinalMessage(HWND hWnd)
{
    __super::OnFinalMessage(hWnd);
    delete this;
}

void MsgBox::OnEsc(BOOL &bHandled)
{
    bHandled = TRUE;
    EndMsgBox(MB_NO);
}

void MsgBox::InitWindow()
{
}

void MsgBox::SetContent(const std::wstring &str)
{
    auto content_ = dynamic_cast<ui::CLabelUI *>(m_pm.FindControl(L"content"));
    assert(content_ != nullptr);
    content_->SetText(str.c_str());
}

void MsgBox::SetButton(bool IsShowCancelBtn)
{
    auto tabInCancelBtn_ =
        dynamic_cast<ui::CTabLayoutUI *>(m_pm.FindControl(L"tabInCancelBtn"));
    assert(tabInCancelBtn_ != nullptr);
    tabInCancelBtn_->SelectItem(IsShowCancelBtn ? 1 : 0);
}

void MsgBox::Show(HWND hwnd, MsgboxCallback cb)
{
    m_callback_ = cb;
    m_parentHwnd = hwnd;
    ::EnableWindow(hwnd, FALSE);
    CenterWindow();
    ShowWindow(true);
}

LRESULT MsgBox::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    ::EnableWindow(m_parentHwnd, TRUE);
    return __super::OnClose(uMsg, wParam, lParam, bHandled);
}

void MsgBox::EndMsgBox(MsgBoxRet ret)
{
    if (m_callback_) {
        Post2UI(ckbase::Bind(m_callback_, ret));
    }
}

void MsgBox::SetTitle(const std::wstring &str)
{
}

const LPCTSTR MsgBox::kClassName = L"MsgBox";

void MsgBox::Notify(ui::TNotifyUI &msg)
{
    if (msg.sType == DUI_MSGTYPE_CLICK) {
        if (msg.pSender->GetName() == L"MsgOk_btn" ||
            msg.pSender->GetName() == L"MsgOk_Zh_btn") {
            Close();
            EndMsgBox(MB_YES);
        }
        else if (msg.pSender->GetName() == L"MsgCancel_Zh_btn") {
            Close();
        }
    }
}