#include "pch.h"
#include "app_env\app_ins.h"
#include <string>
TEST_CASE("app running path should be got at first", "[get_path]")
{
    auto runningPath = AppEnvironment::GetRunningPath();
    std::wstring checkPath{L"f:\\66-projects\\EAccoplus\\build\\test\\Debug\\"};
    CHECK(runningPath == checkPath);
}
