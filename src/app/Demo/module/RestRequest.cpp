/*
 * Written by Li Zhao <bw_lizhao AT whty Dot com Dot cn>.
 * On 2020年5月28日
 * This file use for RESTful request interface
 *
 */

#include "../pch.h"
#include "GlobalSetting.h"

#define KEY_PARAMETER_DEVELOP
#include "RestRequest.h"
#include "RestRequestParms.inl"

const std::string CKeyHelper::GetAppId()
{
    if (m_keyType == KeyParmType::EZT)
        return "b9077c9d9eb34b548dfed1ddf373d291";
    else if (m_keyType == KeyParmType::OTHER) {
        return "";
    }
    return "";
}

const std::string CKeyHelper::GetRsa2Key()
{
    if (m_keyType == KeyParmType::EZT)
        return pKey;
    else if (m_keyType == KeyParmType::OTHER) {
        return "";
    }
    return "";
}

const std::string CKeyHelper::GetRequestUrl()
{
    if (m_keyType == KeyParmType::EZT)
        return GlobalSetting::GetHttpInterfaceUrl();
    else if (m_keyType == KeyParmType::OTHER) {
        return "";
    }
    return "";
}

UnorderedStringMap CIRestRequest::request(const OrderStringMap &params)
{
    auto responseData = __super::request(params, *_mKeyData_);
    ckbase::Info("Response Rest: {}", responseData);
    return parseResponseData(responseData);
}

UnorderedStringMap CIRestRequest::parseResponseData(const std::string &responseData)
{
    auto Root = ParseResponseHelper::doCommonParse(responseData);

    UnorderedStringMap respMap;
    respMap.emplace("success", Json_GetString(Root, "success"));
    respMap.emplace("msg", Json_GetString(Root, "msg"));

    //不存在data域的返回不需要再解析了
    if (!Json_HasField(Root, "data")) {
        return respMap;
    }
    auto dataField = Json_GetString(Root, "data");
    Json::Reader reader;
    Json::Value dataRoot;

    reader.parse(dataField, dataRoot);
    auto dataMap = doParseDataResponse(dataRoot);

    dataMap.insert(respMap.begin(), respMap.end());
    return dataMap;
}

/*----------------------------------------------------------------
/*----------------------------------------------------------------
/*----------------------------------------------------------------
/*----------------------------------------------------------------
/*----------------------------------------------------------------
/*----------------------------------------------------------------*/
OrderStringMap CLoginRequest::makeRequestParms(const std::string &phone,
                                               const std::string &password)
{
    OrderStringMap dataMap;
    dataMap.emplace("phone", phone);
    dataMap.emplace("password", password);
    return dataMap;
}

UnorderedStringMap CLoginRequest::doParseDataResponse(const Json::Value &root)
{
    UnorderedStringMap respDataMap;
    respDataMap.emplace("userId", Json_GetString(root, "userId"));
    respDataMap.emplace("username", Json_GetString(root, "username"));
    respDataMap.emplace("taxNo", Json_GetString(root, "taxNo"));
    respDataMap.emplace("taxType", Json_GetString(root, "taxType"));
    respDataMap.emplace("auditStatus", Json_GetString(root, "auditStatus"));
    respDataMap.emplace("effectiveDate", Json_GetString(root, "effectiveDate"));
    respDataMap.emplace("siheAuthStatus", Json_GetString(root, "siheAuthStatus"));
    respDataMap.emplace("entId", Json_GetString(root, "entId"));
    respDataMap.emplace("entName", Json_GetString(root, "entName"));
    respDataMap.emplace("helperStatus", Json_GetString(root, "helperStatus"));
    respDataMap.emplace("helperAuthEnd", Json_GetString(root, "helperAuthEnd"));
    respDataMap.emplace("checkTotal", Json_GetString(root, "checkTotol"));

    return respDataMap;
}

std::string CLoginRequest::getRequestMethod()
{
    return Restful_req_method_login;
}
// ------------------------------------------------------------------------

OrderStringMap CRegisterRequest::makeRequestParms(const RegisterRequestParms &parms)
{
    OrderStringMap reqDataMap;
    reqDataMap.emplace("identifyingCode", parms.identifyingCode);
    reqDataMap.emplace("username", parms.username);
    reqDataMap.emplace("phone", parms.phone);
    reqDataMap.emplace("password", parms.password);
    reqDataMap.emplace("taxNo", parms.taxNo);
    reqDataMap.emplace("entName", parms.entName);
    reqDataMap.emplace("taxType", parms.taxType);
    reqDataMap.emplace("province", parms.province);
    reqDataMap.emplace("city", parms.city);

    return reqDataMap;
}

UnorderedStringMap CRegisterRequest::doParseDataResponse(const Json::Value &root)
{
    UnorderedStringMap respDataMap;
    respDataMap.emplace("userId", Json_GetString(root, "userId"));

    return respDataMap;
}

std::string CRegisterRequest::getRequestMethod()
{
    return Restful_req_method_registry;
}

OrderStringMap CVerificationCodeRequest::makeRequestParms(const std::string &phone,
                                                          const std::string &type)
{
    OrderStringMap reqDataMap;
    reqDataMap.emplace("phone", phone);
    reqDataMap.emplace("type", type);

    return reqDataMap;
}

UnorderedStringMap CVerificationCodeRequest::doParseDataResponse(const Json::Value &root)
{
    return UnorderedStringMap();
}

std::string CVerificationCodeRequest::getRequestMethod()
{
    return Rest_req_verificationCode;
}
// -------------------------------------------------------------

OrderStringMap
CUploadBusinessLicenseRequest::makeRequestParms(const std::string &uploadFile,
                                                const std::string &resourceId)
{
    OrderStringMap reqDataMap;
    reqDataMap.emplace("uploadFile", uploadFile);
    reqDataMap.emplace("resourceId", resourceId);
    return reqDataMap;
}

UnorderedStringMap
CUploadBusinessLicenseRequest::doParseDataResponse(const Json::Value &root)
{
    UnorderedStringMap respDataMap;
    respDataMap.emplace("key", Json_GetString(root, "key"));
    return respDataMap;
}

std::string CUploadBusinessLicenseRequest::getRequestMethod()
{
    return Restful_req_method_upload_lice_img;
}

// -------------------------------------------------------------

OrderStringMap CGetUserBasicInfoRequest::makeRequestParms(const std::string &userId)
{
    OrderStringMap reqDataMap;
    reqDataMap.emplace("userId", userId);
    return reqDataMap;
}

UnorderedStringMap CGetUserBasicInfoRequest::doParseDataResponse(const Json::Value &root)
{
    UnorderedStringMap respDataMap;
    respDataMap.emplace("phone", Json_GetString(root, "phone"));
    respDataMap.emplace("taxNo", Json_GetString(root, "taxNo"));
    respDataMap.emplace("auditStatus", Json_GetString(root, "auditStatus"));
    respDataMap.emplace("username", Json_GetString(root, "username"));
    respDataMap.emplace("licensePath", Json_GetString(root, "licensePath"));
    respDataMap.emplace("entName", Json_GetString(root, "entName"));
    respDataMap.emplace("taxType", Json_GetString(root, "taxType"));
    respDataMap.emplace("province", Json_GetString(root, "province"));
    respDataMap.emplace("city", Json_GetString(root, "city"));

    return respDataMap;
}

std::string CGetUserBasicInfoRequest::getRequestMethod()
{
    return Restful_req_get_user_basic_info;
}
// -------------------------------------------------------------

OrderStringMap CGetUserOrderInfoRequest::makeRequestParms(const std::string &taxno)
{
    OrderStringMap reqDataMap;
    reqDataMap.emplace("taxNo", taxno);
    return reqDataMap;
}

UnorderedStringMap CGetUserOrderInfoRequest::doParseDataResponse(const Json::Value &root)
{
    UnorderedStringMap respDataMap;
    respDataMap.emplace("count", Json_GetString(root, "count"));
    respDataMap.emplace("infos", Json_GetString(root, "infos"));
    return respDataMap;
}

std::string CGetUserOrderInfoRequest::getRequestMethod()
{
    return Restful_req_get_user_order_info;
}

// -------------------------------------------------------------

OrderStringMap CModifyUserAreaInfoRequest::makeRequestParms(const std::string &userId,
                                                            const std::string &province,
                                                            const std::string &city)
{
    OrderStringMap reqDataMap;
    reqDataMap.emplace("userId", userId);
    reqDataMap.emplace("province", province);
    reqDataMap.emplace("city", city);
    return reqDataMap;
}

UnorderedStringMap
CModifyUserAreaInfoRequest::doParseDataResponse(const Json::Value &root)
{
    return UnorderedStringMap();
}

std::string CModifyUserAreaInfoRequest::getRequestMethod()
{
    return Restful_req_modify_user_area_info;
}

// -------------------------------------------------------------

OrderStringMap CAuditReasonRequest::makeRequestParms(const std::string &userId)
{
    OrderStringMap reqDataMap;
    reqDataMap.emplace("userId", userId);
    return reqDataMap;
}

UnorderedStringMap CAuditReasonRequest::doParseDataResponse(const Json::Value &root)
{
    UnorderedStringMap respDataMap;
    respDataMap.emplace("auditRemark", Json_GetString(root, "auditRemark"));
    return respDataMap;
}

std::string CAuditReasonRequest::getRequestMethod()
{
    return Restful_req_audit_reason;
}
// -------------------------------------------------------------

OrderStringMap CCheckUserStatusRequest::makeRequestParms(const std::string &userId)
{
    OrderStringMap reqDataMap;
    reqDataMap.emplace("userId", userId);
    return reqDataMap;
}

UnorderedStringMap CCheckUserStatusRequest::doParseDataResponse(const Json::Value &root)
{
    UnorderedStringMap respDataMap;
    // TODO
    return respDataMap;
}

std::string CCheckUserStatusRequest::getRequestMethod()
{
    return Restful_req_check_user_status;
}
// -------------------------------------------------------------

OrderStringMap CSiheAccountStatusRequest::makeRequestParms(const std::string &userId)
{
    OrderStringMap reqDataMap;
    reqDataMap.emplace("userId", userId);
    return reqDataMap;
}

UnorderedStringMap CSiheAccountStatusRequest::doParseDataResponse(const Json::Value &root)
{
    UnorderedStringMap respDataMap;
    respDataMap.emplace("key", Json_GetString(root, "key"));
    return respDataMap;
}

std::string CSiheAccountStatusRequest::getRequestMethod()
{
    return Restful_req_sihe_account_status;
}

OrderStringMap CRestFindPassword::makeRequestParms(const std::string &identifyingCode,
                                                   const std::string &phone,
                                                   const std::string &password)
{
    OrderStringMap reqDataMap;
    reqDataMap.emplace("identifyingCode", identifyingCode);
    reqDataMap.emplace("phone", phone);
    reqDataMap.emplace("password", password);
    return reqDataMap;
}

UnorderedStringMap CRestFindPassword::doParseDataResponse(const Json::Value &root)
{
    return UnorderedStringMap();
}

std::string CRestFindPassword::getRequestMethod()
{
    return Restful_req_method_find_pwd;
}

// -------------------------------------------------------------
