#ifndef __INCLUDE_MSGBOX_H__
#define __INCLUDE_MSGBOX_H__

#pragma once
#include "UIlib.h"
#include <string>
#include <functional>

enum MsgBoxRet
{
    MB_YES,
    MB_NO
};

typedef std::function<void(MsgBoxRet)> MsgboxCallback;
/**
 * @brief 文本提示弹窗，可以设置回调函数，弹窗关闭时被调用
 * @param[in]
 * @return
 */
void ShowMsgBox(HWND hwnd, MsgboxCallback cb, const std::wstring &content = L"",
                const std::wstring &title = L"提示", bool IsShowCancelBtn = true);

class MsgBox : public ui::WindowImplBase
{
public:
    friend void ShowMsgBox(HWND hwnd, MsgboxCallback cb, const std::wstring &content,
                           const std::wstring &title, bool IsShowCancelBtn);

public:
    MsgBox() = default;
    virtual ~MsgBox() = default;

    virtual ui::CDuiString GetSkinFile() override;
    virtual LPCTSTR GetWindowClassName(void) const override;
    virtual void OnFinalMessage(HWND hWnd);
    virtual void OnEsc(BOOL &bHandled);

    virtual void InitWindow() override;

private:
    LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    void SetTitle(const std::wstring &str);
    void SetContent(const std::wstring &str);
    void SetButton(bool IsShowCancelBtn = true);
    void Show(HWND hwnd, MsgboxCallback cb);

    void EndMsgBox(MsgBoxRet ret);
    virtual void Notify(ui::TNotifyUI &msg) override;

public:
    static const LPCTSTR kClassName;

private:
    MsgboxCallback m_callback_ = nullptr;
    HWND m_parentHwnd;
};

#endif // __INCLUDE_MSGBOX_H__