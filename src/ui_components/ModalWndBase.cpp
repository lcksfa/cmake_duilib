#include "ModalWndBase.h"

ModalWndBase *ModalWndBase::m_pModalWindow = nullptr;

void ModalWndBase::InitWindow()
{
}

void ModalWndBase::OnFinalMessage(HWND hWnd)
{
    __super::OnFinalMessage(hWnd);
    m_pModalWindow = nullptr;
    delete this;
}

LRESULT ModalWndBase::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    ::EnableWindow(m_parentHwnd, TRUE);
    return __super::OnClose(uMsg, wParam, lParam, bHandled);
}

void ModalWndBase::Notify(ui::TNotifyUI &msg)
{
    auto ctrlName = msg.pSender->GetName();
    if (msg.sType == DUI_MSGTYPE_CLICK) {
        if (ctrlName == L"closebtn")
            Close();
    }
}

LPCTSTR ModalWndBase::GetWindowClassName(void) const
{
    return m_wndClassName.c_str();
}

ui::CDuiString ModalWndBase::GetSkinFile()
{
    return m_xmlPath.c_str();
}

void ModalWndBase::ShowModal(HWND parent, const std::wstring &classname,
                             const std::wstring &xmlpath)
{
    auto *basemodal = new ModalWndBase;
    m_parentHwnd = parent;
    m_xmlPath = xmlpath;
    m_wndClassName = classname;

    auto modalHwnd =
        Create(parent, L"ModalWndBase", WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX, 0);
    assert(modalHwnd != nullptr);

    CenterWindow();
    ShowWindow(true);
    ::EnableWindow(parent, FALSE);
}

void ModalWndBase::Show(HWND parent, const std::wstring &className,
                        const std::wstring &xmlpath)
{
    auto *basemodal = new ModalWndBase;
    m_parentHwnd = parent;
    m_xmlPath = xmlpath;
    m_wndClassName = className;

    auto modalHwnd =
        Create(parent, L"ModalWndBase", WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX, 0);
    assert(modalHwnd != nullptr);

    CenterWindow();
    ShowWindow(true);
}

ModalWndBase *ModalWndBase::CreateInstance()
{
    if (m_pModalWindow == nullptr) {
        m_pModalWindow = new ModalWndBase;
    }
    return m_pModalWindow;
}
