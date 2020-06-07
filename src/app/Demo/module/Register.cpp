#include <regex>
#include "Register.h"
#include "Login.h"
#include <fstream>
#include <cassert>

#include "app_env/app_ins.h"
#include "win32/string_code_convert.h"
#include "win32/win_util.h"
#include "log/logger.h"

#ifdef _DEBUG
constexpr auto WEB_PROTOCOL_URL = L"http://192.168.5.208:9010/protocol.html";
#else
constexpr auto WEB_PROTOCOL_URL = L"http://bztserver.580kp.com/protocol.html";
#endif //_DEBUG

bool Register::IsValidTaxNO(const std::wstring &taxno)
{
    std::wregex re_taxno(L"^[A-Za-z0-9]{15,20}$");
    return std::regex_match(taxno, re_taxno);
}

bool Register::IsValidName(const std::wstring &name)
{
    std::wregex reg(L"^[\u4E00-\u9FA5A-Za-z0-9]+$");
    return std::regex_match(name, reg);
}

bool Register::IsValidPhone(const std::wstring &phone)
{
    Login logic;
    return logic.IsValidPhoneNumber(phone);
}

bool Register::IsValidVerificationcode(const std::wstring &code)
{
    return !code.empty();
}

bool Register::IsValidPassword(const std::wstring &pwd)
{
    Login logic;
    return logic.IsValidPassword(pwd);
}

UnorderedStringMap Register::DoRequest(const RegisterRequestParms &registerParams)
{
    CRegisterRequest requestInstance;
    auto parms = requestInstance.makeRequestParms(registerParams);
    auto ret = requestInstance.request(parms);
    return ret;
}

UnorderedStringMap Register::RequestFetchVerCode(const std::string &phone)
{
    CVerificationCodeRequest requestInstance;
    auto parms = requestInstance.makeRequestParms(phone, "001");
    return requestInstance.request(parms);
}

UnorderedStringMap Register::UploadBusinessLicense(const std::string &uft8_path,
                                                   std::string &userid)
{
    CUploadBusinessLicenseRequest requestInstance;
    auto params = requestInstance.makeRequestParms(uft8_path, userid);
    return requestInstance.request(params);
}

void Register::OpenUserServiceProtocol()
{
    ckbase::win32::SafeOpenUrl(WEB_PROTOCOL_URL);
}

std::vector<std::wstring> Register::GetCityListOf(const std::wstring &provinceName)
{

    std::vector<std::wstring> citesList;

    try {
        auto root = GetDatProviderRoot();

        Json::Value items;
        auto a_provinceName = ckbase::win32::UnicodeToMBCS(provinceName);
        items = root[a_provinceName.c_str()];
        size_t size = items.size();

        for (unsigned int i = 0; i < size; i++) {
            string strCity = Json_GetString(items[i], "name");
            citesList.emplace_back(ckbase::win32::MBCSToUnicode(strCity));
        }
    }
    catch (std::runtime_error &e) {
        ckbase::Warn("GetCityListOf:{}", e.what());
    }

    return citesList;
}

std::wstring Register::GetCityNameByCode(const std::string &citycode)
{

    std::wstring cityName;
    try {
        auto root = GetDatProviderRoot();

        Json::Value items;
        auto provinceListCount = root.size();
        for (auto keyElem : root.getMemberNames()) {
            items = root[keyElem];
            for (auto j = 0; j < items.size(); j++) {
                string strCode = Json_GetString(items[j], "code");
                if (strCode == citycode) {
                    string strCity = Json_GetString(items[j], "name");
                    cityName = ckbase::win32::MBCSToUnicode(strCity);
                    break;
                }
            }
        }
    }
    catch (std::runtime_error &e) {
        ckbase::Warn("GetCityNameByCode:{}", e.what());
    }
    return cityName;
}

std::wstring Register::GetProvinceNameByCode(const std::string &provincecode)
{
    auto provincePrefix = provincecode.substr(0, 2);

    std::wstring provinceName;
    try {
        auto root = GetDatProviderRoot();
        Json::Value items;
        auto provinceListCount = root.size();
        for (auto keyElem : root.getMemberNames()) {
            items = root[keyElem];
            for (auto j = 0; j < items.size(); j++) {
                string strCode = Json_GetString(items[j], "code");
                if (provincePrefix == strCode.substr(0, 2)) {
                    provinceName = ckbase::win32::MBCSToUnicode(keyElem);
                    break;
                }
            }
        }
    }
    catch (std::runtime_error &e) {
        ckbase::Warn("GetProvinceNameByCode:{}", e.what());
    }

    return provinceName;
}

std::string Register::GetCityCodeByName(const std::string &cityName)
{
    std::string cityCode;

    try {
        auto root = GetDatProviderRoot();

        Json::Value items;
        auto provinceListCount = root.size();
        for (auto keyElem : root.getMemberNames()) {
            items = root[keyElem];
            for (auto j = 0; j < items.size(); j++) {
                string strName = Json_GetString(items[j], "name");
                if (strName == cityName) {
                    cityCode = Json_GetString(items[j], "code");
                    break;
                }
            }
        }
    }
    catch (std::runtime_error &e) {
        ckbase::Warn("GetCityCodeByName:{}", e.what());
    }

    return cityCode;
}

std::string Register::GetProvinceCodeByName(const std::string &Provincename)
{
    auto appRunningPath = AppEnvironment::GetRunningPath();
    auto dbFilePath = appRunningPath + L"da.dat";

    std::string provinceCode;
    try {
        auto root = GetDatProviderRoot();

        Json::Value items;
        auto provinceListCount = root.size();
        for (auto keyElem : root.getMemberNames()) {
            if (keyElem == Provincename) {
                items = root[keyElem];
                string strCode = Json_GetString(items[0], "code");
                provinceCode = strCode.substr(0, 2) + "0000";
                break;
            }
        }
    }
    catch (std::runtime_error &e) {
        ckbase::Warn("GetProvinceCodeByName:{}", e.what());
    }

    return provinceCode;
}

std::vector<std::wstring> Register::GetProviceList()
{
    std::vector<std::wstring> provinceList;
    try {
        auto root = GetDatProviderRoot();
        Json::Value items;
        auto provinceListCount = root.size();
        for (auto keyElem : root.getMemberNames()) {
            provinceList.emplace_back(ckbase::win32::MBCSToUnicode(keyElem));
        }
    }
    catch (std::runtime_error &e) {
        ckbase::Warn("GetProviceList:{}", e.what());
    }

    return provinceList;
}

Json::Value Register::GetDatProviderRoot()
{
    auto appRunningPath = AppEnvironment::GetRunningPath();
    auto dbFilePath = appRunningPath + L"da.dat";
    Json::Value root;

    std::ifstream ifs;
    if (std::ifstream ifs{dbFilePath, std::ifstream::binary}) {
        ifs >> root;
    }
    else
        throw std::runtime_error("Error : da.dat cannot read normally!");
    return root;
}