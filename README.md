# cmake_duilib
> 使用cmake管理的duilib，持续关注duilib的发展

>  duilib 源码来自官方，https://github.com/duilib/duilib/

>  xmlPreview源码来自https://github.com/juhuaguai/duilib，后续会继续优化。使用方法在其文件目录readme.md下。

## How to build:

> 推荐使用VS2010及以上版本

启动一个cmder：输入

- git clone https://github.com/lcksfa/cmake_duilib

- cd cmake_duilib

- mkdir cmakebuildvs2010（该中间目录可自行命名）

- cd cmakebuildvs2010

- "e:/Program Files/Microsoft Visual Studio 10.0/VC/vcvarsall.bat" x86(这里是我VS2010的环境路径 ,你需要换成你自己的，使用x86表示32位)
- cmake .. -G "NMake Makefiles"
- nmake

编译完成后将在bin目录下生成文件