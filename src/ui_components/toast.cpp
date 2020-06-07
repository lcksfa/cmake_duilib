#include "toast.h"
#include "closure/msg_loop_thread_factory.hpp"

const LPCTSTR Toast::kClassName = L"Toast";

void Toast::Show(const std::wstring &content, int duration, HWND parent, int x, int y)
{
    Toast *toast = new Toast;
    HWND hWnd = toast->Create(parent, L"", WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX, 0);
    if (hWnd == NULL)
        return;

    toast->SetContent(content);
    toast->SetDuration(duration);
    if (x == 0 || y == 0) {
        toast->CenterWindow();
    }
    else {
        //调整toast弹窗相对主窗口的位置
        RECT rcCenter = {0};
        ::GetWindowRect(parent, &rcCenter);
        ::SetWindowPos(hWnd, NULL, rcCenter.left + x, rcCenter.top + y, -1, -1,
                       SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
    toast->ShowWindow(true);
}

ui::CDuiString Toast::GetSkinFile()
{
    return L"toast.xml";
}
LPCTSTR Toast::GetWindowClassName(void) const
{
    return kClassName;
}

void Toast::OnFinalMessage(HWND hWnd)
{
    __super::OnFinalMessage(hWnd);
    delete this;
}

void Toast::SetDuration(int duration)
{
    if (duration <= 0)
        return;

    ckbase::MessageLoopThreadManager::PostDelayedTask(
        static_cast<int>(ThreadId::UI), ToWeakCallback([this]() { this->Close(); }),
        ckbase::TimeDelta::FromMilliseconds(duration));
}

LRESULT Toast::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_KEYDOWN && wParam == VK_ESCAPE) {
        this->Close();
    }
    // 整个toast界面都在标题栏，所以要处理WM_NC消息
    else if (uMsg == WM_NCLBUTTONDBLCLK || uMsg == WM_LBUTTONDBLCLK) {
        this->Close();
    }
    // duilib在WM_MOUSELEAVE消息中会发送一个lparam为-1的WM_MOUSEMOVE消息
    else if ((uMsg == WM_NCMOUSEMOVE || uMsg == WM_MOUSEMOVE) && lParam != -1) {
        if (NULL != close_button_ && !close_button_->IsVisible())
            close_button_->SetVisible(true);
    }
    else if (uMsg == WM_NCMOUSELEAVE || uMsg == WM_MOUSELEAVE) {
        POINT pt;
        GetCursorPos(&pt);
        ScreenToClient(m_hWnd, &pt);
        ui::CDuiRect client_rect;
        ::GetClientRect(m_hWnd, &client_rect);
        // leave消息触发时，获取的鼠标坐标有可能还在client_rect范围内，会偏差1像素，这里缩减1像素
        client_rect.Deflate(ui::CDuiRect(1, 1, 1, 1));
        if (NULL != close_button_ && !client_rect.IsPointIn(pt))
            close_button_->SetVisible(false);
    }
    return __super::HandleMessage(uMsg, wParam, lParam);
}

void Toast::InitWindow()
{
    content_ = static_cast<ui::CLabelUI *>(m_pm.FindControl(L"content"));
    close_button_ = static_cast<ui::CButtonUI *>(m_pm.FindControl(L"close_btn"));
}

void Toast::SetContent(const std::wstring &str)
{
    content_->SetText(str.c_str());
}

void Toast::Notify(ui::TNotifyUI &msg)
{
    if (msg.sType == DUI_MSGTYPE_CLICK) {
        if (msg.pSender == close_button_) {
            Close();
        }
    }
}
