#ifndef __INCLUDE_FILEDOWNLOADER_H__
#define __INCLUDE_FILEDOWNLOADER_H__

#pragma once
#include <string>

class FileDownloader
{
private:
    std::string m_url;
    std::string m_dstDownloadFilePath;

public:
    FileDownloader(const std::string &url, const std::string &filePath);
    bool Do();
    ~FileDownloader() = default;

    int ProgressOnCallback(double dltotal, double dlnow);
};

#endif // __INCLUDE_FILEDOWNLOADER_H__