#include "GlobalSetting.h"
#include "app_env/app_ins.h"
#include "log/logger.h"
#include "json/json.h"
#include <fstream>
#include "win32/string_code_convert.h"

std::string GlobalSetting::m_httpInterfaceUrl;
std::string GlobalSetting::m_WpHttpInterfaceUrl;
std::string GlobalSetting::m_ImageUploadUrl;
std::string GlobalSetting::m_ElectricInvoiceDownloadUrl;

bool GlobalSetting::ReadSettings()
{
    auto appRunningPath = AppEnvironment::GetRunningPath();
    auto settingsPath = appRunningPath + L"sets.json";
    auto ansi_path = ckbase::win32::UnicodeToMBCS(settingsPath);
    if (std::ifstream ifs{ansi_path, std::ifstream::binary}) {
        Json::Value root;
        ifs >> root;
        m_httpInterfaceUrl = Json_GetString(root, "url");
        m_WpHttpInterfaceUrl = Json_GetString(root, "wp_url");
        m_ImageUploadUrl = Json_GetString(root, "image_url");
        m_ElectricInvoiceDownloadUrl = Json_GetString(root, "elec_url");
        return true;
    }
    return false;
}

std::string GlobalSetting::GetHttpInterfaceUrl()
{
    return m_httpInterfaceUrl;
}

std::string GlobalSetting::GetWpHttpInterfaceUrl()
{
    return m_WpHttpInterfaceUrl;
}

std::string GlobalSetting::GetImageUploadUrl()
{
    return m_ImageUploadUrl;
}

std::string GlobalSetting::GetElectricInvoiceDownloadUrl()
{
    return m_ElectricInvoiceDownloadUrl;
}
