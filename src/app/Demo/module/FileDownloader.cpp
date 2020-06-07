#include "FileDownloader.h"
#include "http/CurlClient.h"
#include "win32/file_utils.h"
#include "win32/string_code_convert.h"

FileDownloader::FileDownloader(const std::string &url, const std::string &filePath)
    : m_url(url)
    , m_dstDownloadFilePath(filePath)
{
}

bool FileDownloader::Do()
{
    std::string localPathZipFileTmp = m_dstDownloadFilePath + ".tmp";

    FuncDownloadProgress callback =
        std::bind(&FileDownloader::ProgressOnCallback, this, std::placeholders::_1,
                  std::placeholders::_2);

    CurlClient curlclient;
    auto ret = curlclient.DownloadFile(m_url, localPathZipFileTmp, callback);
    if (!ret) {
        auto unicodPath = ckbase::win32::MBCSToUnicode(localPathZipFileTmp);
        ckbase::DeleteFile(unicodPath);
    }
    else {
        //改名
        MoveFileA(localPathZipFileTmp.c_str(), m_dstDownloadFilePath.c_str());
    }
    return ret;
}

int FileDownloader::ProgressOnCallback(double dltotal, double dlnow)
{
    // we can know from the callback function the download progress
    return 0;
}
