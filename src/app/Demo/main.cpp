/*
 * Written by Li Zhao <bw_lizhao AT whty Dot com Dot cn>.
 * On 2020年5月27日
 * This file use for EAccoplus solution main entry.
 *
 */

#include "pch.h"
#include "main.h"
#include "MainWindow.h"
#include "app_env\app_ins.h"
#include "module/GlobalSetting.h"

void MainThread::Init()
{
    __super::Init();
    misc_thread_->Start();

    MainWindow *window = new MainWindow();
    window->Create(NULL, MainWindow::kClassName.c_str(), UI_WNDSTYLE_DIALOG, 0);
    window->CenterWindow();
    window->ShowWindow();
}

void MainThread::Cleanup()
{
    misc_thread_->Stop();

    SetThreadWasQuitProperly(true);
    __super::Cleanup();
}

bool MainThread::ResourceInit(HINSTANCE hInstance)
{
    ResourceInitial initres(hInstance);
    if (!initres.Init()) {
        ckbase::Warn(L"资源初始化失败!");
        return false;
    }
    if (!GlobalSetting::ReadSettings()) {
        ckbase::Warn(L"配置文件读取失败!");
        return false;
    }
    return true;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    ckbase::SetTracingLog(AppEnvironment::GetRunningPath(), "log.log");

    ckbase::Info("Release Test Inner Version: {}", "inner_version_0605_15:30");
    MainThread uiThread;
    if (!uiThread.ResourceInit(hInstance)) {
        return -1;
    }

    uiThread.RunOnCurrentThreadWithLoop(ckbase::MessageLoop::kUIMessageLoop);
    ckbase::Info("******Application ended quickly*****\n");

    return 0;
}
