#ifndef __INCLUDE_IMAGEBOX_H__
#define __INCLUDE_IMAGEBOX_H__

#pragma once
#include "UIlib.h"
#include <string>

void ShowImageBox(HWND hwnd, const std::wstring &bk_imagePath);

/**
 * @brief 显示单个背景图片的模态对话框，可以被继承
 * @param[in]
 * @return
 */
class ImageBox : public ui::WindowImplBase
{
public:
    friend void ShowImageBox(HWND hwnd, const std::wstring &bk_imagePath);
    ImageBox() = default;
    virtual ~ImageBox() = default;
    void SetBk(const std::wstring &bk_imagePath);

private:
    virtual ui::CDuiString GetSkinFile() override;
    virtual LPCTSTR GetWindowClassName(void) const override;
    virtual void OnFinalMessage(HWND hWnd);
    virtual void OnEsc(BOOL &bHandled);

    virtual void InitWindow() override;

private:
    LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

    void Show(HWND hwnd);

    virtual void Notify(ui::TNotifyUI &msg) override;

public:
    static const LPCTSTR kClassName;

private:
    HWND m_parentHwnd;
};
#endif // __INCLUDE_IMAGEBOX_H__