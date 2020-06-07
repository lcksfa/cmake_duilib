#ifndef __INCLUDE_GLOBALSETTING_H__
#define __INCLUDE_GLOBALSETTING_H__

#pragma once
#include <string>
class GlobalSetting
{
private:
    /* data */
public:
    GlobalSetting(/* args */) = default;
    ~GlobalSetting() = default;
    static bool ReadSettings();

    static std::string GetHttpInterfaceUrl();
    static std::string GetWpHttpInterfaceUrl();
    static std::string GetImageUploadUrl();
    static std::string GetElectricInvoiceDownloadUrl();

private:
    static std::string m_httpInterfaceUrl;
    static std::string m_WpHttpInterfaceUrl;
    static std::string m_ImageUploadUrl;
    static std::string m_ElectricInvoiceDownloadUrl;
};

#endif // __INCLUDE_GLOBALSETTING_H__