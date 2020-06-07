#ifndef __INCLUDE_ASYNCFILEDIALOG_H__
#define __INCLUDE_ASYNCFILEDIALOG_H__

#pragma once
#include <functional>
#include <string>
#include <map>
#include "Windows.h"
#include "async_modal/AsyncModalBase.h"
#include <commdlg.h>

struct __POSITION {
};
typedef __POSITION *POSITION;

class AsyncFileDialog : public AsyncModalBase
{
public:
    using FileDialogCallback = std::function<void(BOOL, std::wstring)>;

    enum FileDialogType
    {
        FDT_None,
        FDT_OpenFile,
        FDT_SaveFile
    };

    AsyncFileDialog(void);
    ~AsyncFileDialog(void);

public:
    void SetDefExt(LPCTSTR lpszDefExt);
    void SetFileName(LPCTSTR lpszFileName);
    void SetFlags(DWORD dwFlags);
    void SetFilter(LPCTSTR lpszFilter);
    void SetFilter(std::map<LPCTSTR, LPCTSTR> &filters);
    void SetMultiSel(BOOL bMultiSel = TRUE);
    void SetParentWnd(HWND hParentWnd);
    void SetTitle(LPCTSTR lpszTitle);
    void SetFileNameBufferSize(DWORD dwSize);

    void AsyncShowOpenFileDlg(FileDialogCallback file_dialog_callback1);
    void AsyncShowSaveFileDlg(FileDialogCallback file_dialog_callback2);
    virtual void SyncShowModal() override;
    std::wstring GetPathName();
    std::wstring GetFileName();
    std::wstring GetFileExt();
    std::wstring GetFileTitle();
    std::wstring GetFolderPath();

    POSITION GetStartPosition();
    std::wstring GetNextPathName(POSITION &pos);

private:
    void OpenFileDialog();

public:
    OPENFILENAME m_stOFN;

private:
    wchar_t m_szDefExt[64];
    wchar_t m_szFilter[MAX_PATH];
    wchar_t m_szFileName[MAX_PATH];
    wchar_t *m_lpszFileName;
    FileDialogType file_dialog_type_;
    FileDialogCallback m_file_dialog_callback;
};
#endif // __INCLUDE_ASYNCFILEDIALOG_H__