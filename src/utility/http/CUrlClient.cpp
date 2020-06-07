// #include "../pch.h"
#include "http/CurlClient.h"
#include "toolset/string_util.h"

CurlClient::CurlClient()
{
    curl_handle = curl_easy_init();
}
CurlClient::~CurlClient()
{
    curl_easy_cleanup(curl_handle);
}

static size_t OnWriteData(void *buffer, size_t size, size_t nmemb, void *lpVoid)
{
    std::string *str = dynamic_cast<std::string *>((std::string *) lpVoid);
    if (NULL == str || NULL == buffer) {
        return -1;
    }

    char *pData = (char *) buffer;
    //    int len = strlen(pData);
    str->append(pData, size * nmemb);
    return nmemb;
}

int CurlClient::Posts(const std::string &strUrl, const std::string &strPost,
                      std::string &strResponse, int iTimeOut, bool IsUseSSL,
                      const char *pCaPath)
{
    if (NULL == curl_handle) {
        return CURLE_FAILED_INIT;
    }
    /*post数据大于1024长度导致返回为空 修改 2018-12-25 beginning */
    //使用curl post，若post的数据大于1024，curl不会自己发起post请求，而分为2步
    //第一步：发送一个请求，header中包含一个Expect:100-continue，询问server是否愿意接受数据
    //第二步：接收到server返回的100-continue回应之后才post数据
    //这样会导致请求有延迟
    //解决方法：将header中Expect设置为空
    if (strPost.length() >= 1024) {
        // 改协议头
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Expect:");
        curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers);
    }

    curl_easy_setopt(curl_handle, CURLOPT_URL, strUrl.c_str());
    curl_easy_setopt(curl_handle, CURLOPT_POST, 1);
    curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, strPost.c_str());
    curl_easy_setopt(curl_handle, CURLOPT_READFUNCTION, NULL);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, OnWriteData);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *) &strResponse);
    curl_easy_setopt(curl_handle, CURLOPT_NOSIGNAL, 1);
    if (IsUseSSL) {
        if (NULL == pCaPath) {
            curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, false);
            curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYHOST, false);
        }
        else {
            //缺省情况就是PEM，所以无需设置，另外支持DER
            // curl_easy_setopt(curl_handle,CURLOPT_SSLCERTTYPE,"PEM");
            curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, true);
            curl_easy_setopt(curl_handle, CURLOPT_CAINFO, pCaPath);
        }
    }

    curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT, 3);
    /*抬头信息获取响应时间限制 新增iTimeOut配置字段 修改 2018-12-20 */
    curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT_MS, iTimeOut);
    return curl_easy_perform(curl_handle);
}

//添加自定义head
int CurlClient::Gets(const std::string &strUrl, std::string &strResponse,
                     const std::vector<std::string> &strHeadArray, const char *pCaPath)
{
    if (NULL == curl_handle) {
        return CURLE_FAILED_INIT;
    }

    curl_easy_setopt(curl_handle, CURLOPT_URL, strUrl.c_str());
    curl_easy_setopt(curl_handle, CURLOPT_READFUNCTION, NULL);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, OnWriteData);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *) &strResponse);
    curl_easy_setopt(curl_handle, CURLOPT_NOSIGNAL, 1);
    if (NULL == pCaPath) {
        curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, false);
        curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYHOST, false);
    }
    else {
        curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, true);
        curl_easy_setopt(curl_handle, CURLOPT_CAINFO, pCaPath);
    }
    curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT, 3);
    curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, 3);

    struct curl_slist *headers = NULL; /* init to NULL is important */
    for (unsigned int i = 0; i < strHeadArray.size(); i++) {
        headers = curl_slist_append(headers, strHeadArray[i].c_str());
    }

    /* pass our list of custom made headers */
    curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers);

    return curl_easy_perform(curl_handle);
}

int progress_callback(FuncDownloadProgress *callback, double dltotal, double dlnow,
                      double ultotal, double ulnow)
{
    return (*callback)(dltotal, dlnow);
}

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
    size_t written = fwrite(ptr, size, nmemb, (FILE *) stream);
    return written;
}

bool CurlClient::DownloadFile(const std::string &url, const std::string &fname,
                              FuncDownloadProgress progressClient)
{

    if (NULL == curl_handle) {
        return false;
    }
    FILE *pagefile = NULL;
    // the url may has chinese characters
    std::string urlU(url);
    char *encode_url = curl_easy_escape(curl_handle, urlU.c_str(), 0);
    std::string whileEncodeUrl(encode_url);
    ckbase::StringReplaceAll(whileEncodeUrl, "%3A", ":");
    ckbase::StringReplaceAll(whileEncodeUrl, "%2F", "/");

    const char *urlc = whileEncodeUrl.c_str();
    /* set URL to get here */
    curl_easy_setopt(curl_handle, CURLOPT_URL, urlc);
    /* disable progress meter, set to 0L to enable and disable debug output */
    curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(curl_handle, CURLOPT_PROGRESSFUNCTION, progress_callback);
    curl_easy_setopt(curl_handle, CURLOPT_PROGRESSDATA, &progressClient);

    curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, false);
    curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYHOST, false);

    /* send all data to this function  */
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
    /* open the file */
    fopen_s(&pagefile, fname.c_str(), "wb");

    CURLcode downret = CURL_LAST;
    if (pagefile) {
        /* write the page body to this file handle */
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, pagefile);
        /* get it! */
        downret = curl_easy_perform(curl_handle);
        /* close the header file */
        fclose(pagefile);
    }
    return (downret == CURLE_OK);
}