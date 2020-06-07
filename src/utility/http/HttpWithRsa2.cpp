#include "http/HttpWithRsa2.h"
#include "win32/string_code_convert.h"
#include "toolset/string_util.h"
#include "win32/string_code_convert.h"

#include "toolset/date.h"
#include "win32/win_util.h"

#include "encrypt/rsa2.h"
#include "log/logger.h"

std::string HttpWithRsa2::makeRequestParmString(const OrderStringMap &inputParms,
                                                CIKeyHelper &keyData)
{
    auto mapTemp = inputParms;
    mapTemp["appId"] = keyData.GetAppId();
    mapTemp["timestamp"] = ckbase::datetime::getFormatTimeStamp();
    mapTemp["nonce"] = ckbase::win32::GetNewUuid();
    mapTemp["signType"] = "RSA2";
    mapTemp["method"] = getRequestMethod();
    return sortReqMapToSignedString(mapTemp, keyData);
}

std::string HttpWithRsa2::sortReqMapToSignedString(const OrderStringMap &mapReq,
                                                   CIKeyHelper &keyData)
{
    std::string strRet;

    for (auto iter = mapReq.begin(); iter != mapReq.end(); ++iter) {
        if (iter == mapReq.begin())
            strRet += "";
        else
            strRet += "&";
        strRet += iter->first;
        strRet += "=";
        strRet += iter->second;
    }

    auto strRetUtf8 = ckbase::win32::MBCSToUTF8(strRet);
    std::string rqtStrWithSign = CRSA2::sign(strRetUtf8, keyData.GetRsa2Key());
    ckbase::StringReplaceAll(rqtStrWithSign, "+", "%2B");

    std::string sign = "&sign=" + rqtStrWithSign;
    strRet += sign;
    return strRet;
}

std::string HttpWithRsa2::request(const OrderStringMap &inputParms, CIKeyHelper &keyData)
{
    std::string requestString(makeRequestParmString(inputParms, keyData));
    std::string strUrl(keyData.GetRequestUrl());
    std::string urlMethod(getRequestMethod());
    ckbase::StringReplaceAll(urlMethod, ".", "/");
    //为云赋通接口特化，这里有点无语
    ckbase::StringReplaceAll(urlMethod, "ctk", "ctkserver");

    strUrl += urlMethod;
    ckbase::Info("HTTP Rest Request:[url:{}]-->[request params:{}]", strUrl,
                 requestString);

    return __super::request(strUrl, requestString);
}