#pragma once
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN // 从 Windows 头中排除极少使用的资料
// Windows 头文件:
#include <windows.h>

#include <olectl.h>

#ifdef GDIPVER
#undef GDIPVER
#endif
#define GDIPVER 0x0110
#include <GdiPlus.h>

#include <commctrl.h>
#include <stddef.h>
#include <richedit.h>
#include <tchar.h>
#include <assert.h>
#include <crtdbg.h>
#include <malloc.h>
#include <algorithm>
#include <string>
#include <memory>
#include <vector>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <functional>
#include <regex>

// base header

// #include "base.h"

// #include "commons.h"

#include "UIlib.h"
// #include "ui_embedded.h"
#include "log/logger.h"

//#pragma comment(linker, "/manifestdependency:\"type='win32' "                            \
//                        "name='runtime140' "                                             \
//                        "version='1.0.0.0' "                                             \
//                        "processorArchitecture='x86' "                                   \
//                        "language='*' "                                                  \
//                        "\"")