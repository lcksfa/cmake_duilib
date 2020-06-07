// #include "../pch.h"
#include "http/HttpInterface.h"
#include "http/CurlClient.h"

#include "toolset/string_util.h"
#include "win32/string_code_convert.h"

#include "toolset/date.h"
#include "win32/win_util.h"
#include "json/json.h"

constexpr auto responseWithNetError = "网络异常，请稍后重试!";
constexpr auto responseWithServiceException = "服务器异常，请稍后重试!";

Json::Value ParseResponseHelper::doCommonParse(const std::string &response)
{
    //这里统一处理下，避免后台返回数据为空
    if (response.empty()) {
        Json::Value resultRoot;
        resultRoot["success"] = "false";
        resultRoot["msg"] = responseWithNetError;
        return resultRoot;
    }

    Json::Value root;
    Json::Reader reader;
    OrderStringMap responseMap;

    //服务器返回的数据 非json格式
    if (!reader.parse(response, root) || root.size() <= 0 ||
        !Json_HasField(root, "success")) {
        Json::Value resultRoot;
        resultRoot["success"] = "false";
        resultRoot["msg"] = responseWithServiceException;
        return resultRoot;
    }

    return root;
}

constexpr auto REQUEST_TIMEOUT = 3000; // util for millseconds

std::string IHttpInterface::request(const std::string &requesturl,
                                    const std::string &requestParam, bool IsUseSSL)
{
    std::string strUtf8request = ckbase::win32::MBCSToUTF8(requestParam);
    CurlClient curlClient;
    std::string strResponse;

    curlClient.Posts(requesturl, strUtf8request, strResponse, REQUEST_TIMEOUT, IsUseSSL);

    std::string ansiResponse = ckbase::win32::UTF8ToMBCS(strResponse);
    return ansiResponse;
}
