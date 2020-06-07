#include "ImageBox.h"

void ShowImageBox(HWND parentHwnd, const std::wstring &bk_imagePath)
{
    ImageBox *imageBox = new ImageBox;
    HWND hWnd =
        imageBox->Create(parentHwnd, L"", WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX, 0);
    assert(hWnd != nullptr);
    if (hWnd == NULL)
        return;
    imageBox->SetBk(bk_imagePath);
    imageBox->Show(parentHwnd);
}

void ImageBox::SetBk(const std::wstring &bk_imagePath)
{
    auto _labImg = m_pm.FindControl(L"labImg");
    assert(_labImg != nullptr);
    _labImg->SetBkImage(bk_imagePath.c_str());
}

LRESULT ImageBox::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    ::EnableWindow(m_parentHwnd, TRUE);
    return __super::OnClose(uMsg, wParam, lParam, bHandled);
}

void ImageBox::Show(HWND hwnd)
{
    m_parentHwnd = hwnd;
    ::EnableWindow(hwnd, FALSE);
    CenterWindow();
    ShowWindow(true);
}

void ImageBox::Notify(ui::TNotifyUI &msg)
{
    if (msg.sType == DUI_MSGTYPE_CLICK) {
        if (msg.pSender->GetName() == L"imageOk_btn") {
            Close();
        }
    }
}

ui::CDuiString ImageBox::GetSkinFile()
{
    return L"imagebox.xml";
}

LPCTSTR ImageBox::GetWindowClassName(void) const
{
    return L"ImageBoxClass";
}

void ImageBox::OnFinalMessage(HWND hWnd)
{
    __super::OnFinalMessage(hWnd);
    delete this;
}

void ImageBox::OnEsc(BOOL &bHandled)
{
    bHandled = TRUE;
    Close();
}

void ImageBox::InitWindow()
{
}
