#ifndef __INCLUDE_MODALWNDBASE_H__
#define __INCLUDE_MODALWNDBASE_H__

#pragma once
#include <string>
#include "UIlib.h"

class ModalWndBase : public ui::WindowImplBase
{
private:
    /* data */
    static ModalWndBase *m_pModalWindow;

    std::wstring m_xmlPath;
    HWND m_parentHwnd;
    std::wstring m_wndClassName;

    bool IsModal = false;

public:
    ModalWndBase(/* args */) = default;
    ~ModalWndBase() = default;
    static ModalWndBase *CreateInstance();
    /**
     * @brief 展示模态对话框，注意，暂不支持模态对话框的嵌套使用
     * @param[in] parent：父亲窗口的句柄，className ，类名，xmlpath：xml文件的路径
     * @return
     */
    void ShowModal(HWND parent, const std::wstring &className,
                   const std::wstring &xmlpath);

    /**
     * @brief 展示非模态对话框
     * @param[in] parent：父亲窗口的句柄，className ，类名，xmlpath：xml文件的路径
     * @return
     */
    void Show(HWND parent, const std::wstring &className, const std::wstring &xmlpath);

private:
    LPCTSTR GetWindowClassName(void) const override;
    virtual ui::CDuiString GetSkinFile() override;
    virtual void InitWindow() override;
    virtual void OnFinalMessage(HWND hWnd) override;

    virtual LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/,
                            BOOL &bHandled) final;
    virtual void Notify(ui::TNotifyUI &msg) final;
};

#endif // __INCLUDE_MODALWNDBASE_H__