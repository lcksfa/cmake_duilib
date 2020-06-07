#ifndef __INCLUDE_URLCLIENT_H__
#define __INCLUDE_URLCLIENT_H__

#pragma once
#include "curl/curl.h"
#include <mutex>
#include <functional>
#include <vector>
using FuncDownloadProgress = std::function<int(double, double)>;

class CurlClient
{
public:
    CurlClient();
    ~CurlClient();
    int Posts(const std::string &strUrl, const std::string &strPost,
              std::string &strResponse, int iTimeOut = 3000, bool IsUseSSL = true,
              const char *pCaPath = NULL);
    int Gets(const std::string &strUrl, std::string &strResponse,
             const std::vector<std::string> &strHeadArray = std::vector<std::string>(),
             const char *pCaPath = NULL);

    bool DownloadFile(const std::string &url, const std::string &fname,
                      FuncDownloadProgress clientp);

private:
    CURL *curl_handle;
    std::mutex _mutex;
};
#endif // __INCLUDE_URLCLIENT_H__