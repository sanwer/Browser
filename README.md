# Browser(CEF浏览器)
  基于CEF3与Duilib界面库，支持Flash PPAPI插件(系统自带或打包内置)，支持多标签页浏览，支持文件下载，支持使用[miniblink](https://github.com/weolar/miniblink49)替换CEF。

### 编译
1、获取Duilib
+ https://github.com/sanwer/duilib.git
+ https://gitee.com/sanwer/duilib.git

2、获取CEF3
+ https://github.com/sanwer/libcef.git
+ https://gitee.com/sanwer/libcef.git

3、获取Browser
+ https://github.com/sanwer/Browser.git
+ https://gitee.com/sanwer/Browser.git

4、编译
```sh
Build.cmd
```

5、运行
  运行或调试前，请确保Bin目录下有如下文件：
+ locales/en-GB.pak
+ locales/zh-CN.pak
+ cef.pak
+ cef_100_percent.pak
+ cef_extensions.pak
+ d3dcompiler_43.dll
+ d3dcompiler_47.dll
+ icudtl.dat
+ libcef.dll
+ libEGL.dll
+ libGLESv2.dll
+ natives_blob.bin

  CEF Binary下载地址：
+ https://github.com/sanwer/libcef/releases
+ https://gitee.com/sanwer/libcef/releases

### 联系方式
手机/微信：18008480609，邮件/支付宝：sanwer@qq.com，提供付费技术支持。
