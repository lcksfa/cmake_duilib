#ifndef __INCLUDE_MODALWND_H__
#define __INCLUDE_MODALWND_H__
#pragma once
#include <string>
#include "ModalWndBase.h"

enum class ModelWndXml
{
    aboutwndxml,
    waitingwndxml,
    // add more
};

/**
 * @brief
 * 手动管理显示和关闭的弹窗类，只支持xml固定且不做消息响应处理的轻量
 * 弹窗，分模拟的模态和非模态对话框，注意别嵌套使用
 * @param[in]
 * @return
 */
class ModalWnd
{
public:
    ModalWnd() = default;
    ~ModalWnd() = default;

private:
    static std::wstring GetXmlPath(ModelWndXml xml)
    {
        switch (xml) {
        case ModelWndXml::aboutwndxml:
            return L"aboutwnd.xml";
        case ModelWndXml::waitingwndxml:
            return L"waitingmodal.xml";
        }

        return L"";
    }

    static std::wstring GetClassName(ModelWndXml xml)
    {
        switch (xml) {
        case ModelWndXml::aboutwndxml:
            return L"AboutWndClass";
        case ModelWndXml::waitingwndxml:
            return L"WaitingWndClass";
        }
        return L"";
    }

    /**
     * @brief 模态：注意，这里不是真正意义上的模态，而只是模拟其效果
     * @param[in]
     * @return
     */
    static void ShowModal(HWND hwnd, ModelWndXml xmlEnum)
    {
        ModalWndBase::CreateInstance()->ShowModal(hwnd, GetClassName(xmlEnum),
                                                  GetXmlPath(xmlEnum));
    }

    /**
     * @brief 非模态
     * @param[in]
     * @return
     */
    static void Show(HWND hwnd, ModelWndXml xmlEnum)
    {
        ModalWndBase::CreateInstance()->Show(hwnd, GetClassName(xmlEnum),
                                             GetXmlPath(xmlEnum));
    }

public:
    static void ShowWaiting(HWND hwnd)
    {
        return ShowModal(hwnd, ModelWndXml::waitingwndxml);
    }

    static void ShowAboutWnd(HWND hwnd)
    {
        return ShowModal(hwnd, ModelWndXml::aboutwndxml);
    }
    static void CloseModal()
    {
        ModalWndBase::CreateInstance()->Close();
    }
};

#endif // __INCLUDE_MODALWND_H__