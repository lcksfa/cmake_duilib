#ifndef __INCLUDE_RESTREQUEST_H__
#define __INCLUDE_RESTREQUEST_H__
#pragma once
#include <string>
#include <memory>
#include <unordered_map>
#include <functional>
#include "http/HttpWithRsa2.h"

#include "json/json.h"

struct RegisterRequestParms {
    std::string identifyingCode;
    std::string username;
    std::string phone;
    std::string password;
    std::string taxNo;
    std::string entName;
    std::string taxType;
    std::string province;
    std::string city;
};

enum class KeyParmType
{
    EZT,   // E账通
    OTHER, //应对后续的接口BOOM
};

class CKeyHelper : public CIKeyHelper
{
public:
    CKeyHelper(KeyParmType keytype)
        : m_keyType(keytype){};
    const std::string GetAppId() final;
    const std::string GetRsa2Key() final;
    const std::string GetRequestUrl() final;

private:
    KeyParmType m_keyType;
};

class CIRestRequest : public HttpWithRsa2
{
private:
    /* data */
    std::unique_ptr<CKeyHelper> _mKeyData_ =
        std::make_unique<CKeyHelper>(KeyParmType::EZT);

public:
    UnorderedStringMap request(const OrderStringMap &params);

private:
    UnorderedStringMap parseResponseData(const std::string &responseData);
    virtual UnorderedStringMap doParseDataResponse(const Json::Value &root) = 0;
    virtual std::string getRequestMethod() = 0;
};

/**
 * @brief 登录请求接口
 * @param[in]
 * @return
 */
class CLoginRequest : public CIRestRequest
{
public:
    OrderStringMap makeRequestParms(const std::string &phone,
                                    const std::string &password);

private:
    UnorderedStringMap doParseDataResponse(const Json::Value &root) final;

private:
    std::string getRequestMethod() final;
};

/**
 * @brief 程序升级请求接口
 * @param[in]
 * @return
 */
class CUpdateRequest : public CIRestRequest
{
public:
    OrderStringMap makeRequestParms(const std::string &appId,
                                    const std::string &provinceId);

private:
    UnorderedStringMap doParseDataResponse(const Json::Value &root) final;

private:
    std::string getRequestMethod() final;
};

/**
 * @brief 注册请求
 * @param[in]
 * @return
 */
class CRegisterRequest : public CIRestRequest
{
public:
    OrderStringMap makeRequestParms(const RegisterRequestParms &parms);

private:
    UnorderedStringMap doParseDataResponse(const Json::Value &root) final;

private:
    std::string getRequestMethod() final;
};

/**
 * @brief 获取验证码
 * @param[in]
 * @return
 */
class CVerificationCodeRequest : public CIRestRequest
{
public:
    /**
    * @brief 请求参数组合成map，
    * @param[in] 手机号，验证码的类型 ，001注册验证码
    002找回密码验证码
    * @return 参数map，用于rest请求
    */
    OrderStringMap makeRequestParms(const std::string &phone, const std::string &type);

private:
    UnorderedStringMap doParseDataResponse(const Json::Value &root) final;

private:
    std::string getRequestMethod() final;
};

/**
 * @brief 上传营业执照
 * @param[in]
 * @return
 */
class CUploadBusinessLicenseRequest : public CIRestRequest
{
public:
    /**
    * @brief 请求参数组合成map，
    * @param[in] uploadFile 待上传的文件路径，resourceId 用户id
    002找回密码验证码
    * @return 参数map，用于rest请求
    */
    OrderStringMap makeRequestParms(const std::string &uploadFile,
                                    const std::string &resourceId);

private:
    UnorderedStringMap doParseDataResponse(const Json::Value &root) final;

private:
    std::string getRequestMethod() final;
};

/**
 * @brief 获取用户基础信息接口
 * @param[in]
 * @return
 */
class CGetUserBasicInfoRequest : public CIRestRequest
{
public:
    /**
    * @brief 请求参数组合成map，
    * @param[in] userId 用户id
    002找回密码验证码
    * @return 参数map，用于rest请求
    */
    OrderStringMap makeRequestParms(const std::string &userId);

private:
    UnorderedStringMap doParseDataResponse(const Json::Value &root) final;

private:
    std::string getRequestMethod() final;
};

/**
 * @brief 获取用户订单信息接口
 * @param[in]
 * @return
 */
class CGetUserOrderInfoRequest : public CIRestRequest
{
public:
    /**
    * @brief 请求参数组合成map，
    * @param[in] userId 用户税号
    002找回密码验证码
    * @return 参数map，用于rest请求
    */
    OrderStringMap makeRequestParms(const std::string &taxno);

private:
    UnorderedStringMap doParseDataResponse(const Json::Value &root) final;

private:
    std::string getRequestMethod() final;
};

class CModifyUserAreaInfoRequest : public CIRestRequest
{
public:
    /**
    * @brief 请求参数组合成map，
    * @param[in] userId 用户税号
    002找回密码验证码
    * @return 参数map，用于rest请求
    */
    OrderStringMap makeRequestParms(const std::string &userId,
                                    const std::string &province, const std::string &city);

private:
    UnorderedStringMap doParseDataResponse(const Json::Value &root) final;

private:
    std::string getRequestMethod() final;
};

/**
 * @brief 获取用户审核原因
 * @param[in]
 * @return
 */
class CAuditReasonRequest : public CIRestRequest
{
public:
    /**
    * @brief 请求参数组合成map，
    * @param[in] userId 用户id
    002找回密码验证码
    * @return 参数map，用于rest请求
    */
    OrderStringMap makeRequestParms(const std::string &userId);

private:
    UnorderedStringMap doParseDataResponse(const Json::Value &root) final;

private:
    std::string getRequestMethod() final;
};

/**
 * @brief 定时获取用户状态的接口
 * @param[in]
 * @return
 */

class CCheckUserStatusRequest : public CIRestRequest
{
public:
    /**
    * @brief 请求参数组合成map，
    * @param[in] userId 用户id
    002找回密码验证码
    * @return 参数map，用于rest请求
    */
    OrderStringMap makeRequestParms(const std::string &userId);

private:
    UnorderedStringMap doParseDataResponse(const Json::Value &root) final;

private:
    std::string getRequestMethod() final;
};

class CSiheAccountStatusRequest : public CIRestRequest
{
public:
    /**
    * @brief 请求参数组合成map，
    * @param[in] userId 用户id
    002找回密码验证码
    * @return 参数map，用于rest请求
    */
    OrderStringMap makeRequestParms(const std::string &userId);

private:
    UnorderedStringMap doParseDataResponse(const Json::Value &root) final;

private:
    std::string getRequestMethod() final;
};

/**
 * @brief 找回密码接口
 * @param[in]
 * @return
 */
class CRestFindPassword : public CIRestRequest
{
public:
    /**
    * @brief 请求参数组合成map，
    * @param[in] userId 用户id
    002找回密码验证码
    * @return 参数map，用于rest请求
    */
    OrderStringMap makeRequestParms(const std::string &identifyingCode,
                                    const std::string &phone,
                                    const std::string &password);

private:
    UnorderedStringMap doParseDataResponse(const Json::Value &root) final;

private:
    std::string getRequestMethod() final;
};
#endif // __INCLUDE_RESTREQUEST_H__