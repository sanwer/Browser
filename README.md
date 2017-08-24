# Browser(CEF浏览器)
  CEF浏览器是[Duilib](https://github.com/sanwer/DuiLib)+[CEF3](https://github.com/sanwer/libcef)的开源示例程序，含Flash PPAPI插件。
  ![CEF浏览器](https://github.com/sanwer/Browser/blob/master/Demo.png)

**编译**
```sh
Build.cmd
```

**Cleanup project**
```sh
# bfg.jar --delete-files Bin/cef.pak  Browser.git
# java -jar bfg.jar --strip-blobs-bigger-than 30K Browser.git
Modify deleted-files.txt
# cd Browser.git
# git reflog expire --expire=now --all && git gc --prune=now --aggressive

```
