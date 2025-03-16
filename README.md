# ESP32_Module_Lib



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

