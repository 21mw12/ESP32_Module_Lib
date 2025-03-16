# ESP32_Module_Lib

## 项目简介

在使用ESP32过程中编写并测试的库文件，再次留存合共各位一起学习交流。



## 项目结构

- components：各个功能组件
- Demo：测试好的具体功能的示例
- Template：某一块板子的示例工程



# 开发流程

```shell
# 先启用idf

#创建空白工程
idf.py create-project [filename]
# 指定工程使用的芯片
idf.py set-target esp32
# 进入配置菜单
idf.py menuconfig
# 编译工程
idf.py build
# 烧录（Linux串口格式位ttyUSB0，Win格式为COM4）
idf.py <-p PORT> flash <monitor>
# 打开串口监视器
idf.py <-p ttyUSB0> monitor
```



# 使用流程

- 在项目根目录下新建 `components` 目录
- 将所需的组件复制进项目当中
- 查看组件的 `README.md` 文件，添加依赖的库或官方库
- 修改对应引脚
- 编译下载测试
