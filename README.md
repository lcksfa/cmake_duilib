# cmake_duilib

## 说明

- 使用 cmake 管理的 duilib，持续关注 duilib 的发展

- duilib 源码来自`https://github.com/qdtroy/DuiLib_Ultimate`

- 采用 Google Chrome 的消息循环管理机制。

## How to build

> 推荐使用 VS2015 及以上版本

### 使用 VSCode 开发

只需要下载 Cmake 插件即可全自动编译

### 使用 VS 开发，nmake 编译

> 使用 VS2015 的 amd64_X86 平台集

- 输入如下命令：

```txt
git clone https://github.com/lcksfa/cmake_duilib

cd cmake_duilib
mkdir build

打开VS2015 本机命令提示符，切换到工程build目录下
- cmake .. -G "NMake Makefiles"
- nmake
```

编译完成后将在 bin 目录下生成文件
