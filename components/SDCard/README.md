# 关于库

以esp32cam验证，其他板子需要更改引脚。



# 开启配置

```shell
idf.py menuconfig
找到SD Card Example menu
开启Format the card if mount failed
```



# 保存文本

```c
# 路径有两种方式

const char *file_hello = MOUNT_POINT"/hello.txt";
ret = SDCard_WriteFile(file_hello, "Hello World!\n");

ret = SDCard_WriteFile("/sdcard/hello.txt", "Hello World!\n");
```



# 保存照片

需要拉取esp_camera，无需求建议删除。

拉取命令

```shell
idf.py add-dependency espressif/esp32-camera
```