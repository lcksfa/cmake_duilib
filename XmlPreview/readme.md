How To Use：



bin目录下已有编译好的exe执行文件。

- 将dui xml文件直接拖到XmlPreview.exe上即可预览，使用Esc或者ALT+F4关闭之。
- 嵌套到notepad++等编辑器：
    1. 使用notepad++打开xml文件
    2. F5打开运行配置窗口
    3. 在`运行程序名`编辑框输入入`E:\GoderTime\cmake_duilib\bin\XmlPreview.exe $(FULL_CURRENT_PATH)`，前面是XmlPreview.exe的绝对路径，后面宏表示本文件。
    4. 点击运行即可查看， 
    5. 可以配置该动作的快捷方式。比如Alt+F1等，方面后续使用。

