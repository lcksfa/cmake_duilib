#include "http/HttpWithDes.h"
#include "encrypt/des.h"
#include "encrypt/md5.h"
#include "toolset/string_util.h"
#include "json/json.h"
#include <algorithm>
#include <utility>

static const std::string DES_Key = "EPDbwyfzWp";

std::string HttpWithDes::request(const UnorderedStringMap &requestParms)
{
    std::string requestString(makeRequestParmString(requestParms));
    std::string strUrl(GetUrl());

    auto des_encrypt_response = __super::request(strUrl, requestString, false);
    return Decrypt(des_encrypt_response);
}

std::string HttpWithDes::makeRequestParmString(const UnorderedStringMap &inputParms)
{
    Json::Value root;

    //将map转成json数组
    auto makeJsonString = [&root](const std::pair<std::string, std::string> &pair) {
        root[pair.first] = pair.second;
    };
    std::for_each(inputParms.begin(), inputParms.end(), makeJsonString);
    auto jsonString = root.toStyledString();
    // auto desRequeststring = Encrypt(jsonString);

    std::string desRequest{"json="};
    desRequest += jsonString;
    return desRequest;
}

std::string HttpWithDes::Decrypt(const std::string &strIn)
{
    // 1 取出随机数和Hash值
    if (strIn.size() < 40)
        return "";
    string SendKeyIn = strIn.substr(0, 8);
    string strHash = strIn.substr(strIn.length() - 32);

    char buf[40];
    // 2 通过随机数得到会话秘钥然后解密
    memset(buf, 0, 40);
    ckbase::yxDES des;

    des.RunDes(ckbase::ENCRYPT, ckbase::ECB, (char *) SendKeyIn.c_str(), buf, 8,
               DES_Key.c_str(), DES_Key.length());
    // string strKey = buf;

    string strData = strIn.substr(8, strIn.length() - 40);
    unsigned int nLength = strData.length();
    if (nLength == 0 || nLength % 8 != 0)
        return false;
    nLength = (nLength / 2) + 1;
    char *pMiData = new char[nLength];
    ckbase::AsciiToHex((unsigned char *) strData.c_str(), (unsigned char *) pMiData,
                       strData.length());

    char *pDesData = new char[nLength];
    memset(pDesData, 0, nLength);

    des.RunDes(ckbase::DECRYPT, ckbase::ECB, pMiData, pDesData, strData.length() / 2, buf,
               8);

    delete[] pMiData;
    return pDesData;
    // 3将解密后的字符串进行MD5加密，然后比较hash值是否一致
    // char strMD5[128];
    // memset(strMD5, 0, sizeof(strMD5));
    // int iLen = strlen(pDesData) - (SendKeyIn[0] - 48);
    // getMD5String((unsigned char *) pDesData, iLen, (unsigned char *) strMD5);
    // pDesData[iLen] = 0;
    // strOut = pDesData;
    // delete[] pDesData;

    // char chTemp2x[256] = {0};
    // ckbase::HexToAscii((unsigned char *) strMD5, (unsigned char *) chTemp2x, 16);
    // string strHashResult = (char *) chTemp2x;

    // if (strHashResult == strHash) {
    //     // strOut = ckbase::UnicodeToUtf8(strOut.c_str());
    //     return true;
    // }
    // else {
    //     strOut = "Hash值不一致";
    //     return false;
    // }
}

std::string HttpWithDes::Encrypt(const std::string &srcIn)
{
    char SendKeyIn[9] = {0};
    srand(GetTickCount());
    char buf[40];

    for (int j = 0; j < 8; j++) {
        int i = 1 + (int) (10.0 * rand() / (RAND_MAX + 1.0));
        memset(buf, 0, 40);
        sprintf_s(buf, "%d", i);
        memcpy(SendKeyIn + j, buf, 1);
    }
    SendKeyIn[0] = (8 - (srcIn.length() & 0x00000007)) % 8 + 48;
    std::string strOut = SendKeyIn;

    // 1 产生会话用的会话秘钥
    ckbase::yxDES des;
    memset(buf, 0, 40);
    des.RunDes(ckbase::ENCRYPT, ckbase::ECB, SendKeyIn, buf, 8, DES_Key.c_str(),
               DES_Key.length());

    // 2 用会话秘钥将输入字符串加密
    unsigned int nLength = srcIn.length() + 16;
    char *pData = new char[nLength];
    memset(pData, 0, nLength);
    unsigned int padlen = nLength;
    des.RunPad(ckbase::PAD_PKCS_7, srcIn.c_str(), srcIn.length(), pData, padlen);
    pData[padlen] = 0;

    char *pPadData = new char[padlen];
    memset(pPadData, 0, padlen);
    des.RunDes(ckbase::ENCRYPT, ckbase::ECB, pData, pPadData, padlen, buf, 8);
    delete[] pData;

    unsigned int iLen = (padlen * 2) + 1;
    char *pMiData = new char[iLen];
    memset(pMiData, 0, iLen);
    ckbase::HexToAscii((unsigned char *) pPadData, (unsigned char *) pMiData, padlen);
    delete[] pPadData;
    strOut.append(pMiData);
    delete[] pMiData;

    // 3 将输入字符串进行MD5加密

    char chTemp2x[256];
    memset(chTemp2x, 0, sizeof(chTemp2x));
    auto strMD5 = ckbase::MD5(srcIn).toStr();
    ckbase::HexToAscii((unsigned char *) strMD5.c_str(), (unsigned char *) chTemp2x, 16);
    strOut.append(chTemp2x);

    // char strMD5[128];
    // memset(strMD5, 0, sizeof(strMD5));
    // getMD5String((unsigned char *) strIn.c_str(), strIn.length(),
    //              (unsigned char *) strMD5);
    // char chTemp2x[256];
    // memset(chTemp2x, 0, sizeof(chTemp2x));
    // ckbase::HexToAscii((unsigned char *) strMD5, (unsigned char *) chTemp2x,
    // 16); strOut.append(chTemp2x);
    return strOut;
}
