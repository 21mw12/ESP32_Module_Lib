import requests
import cv2
import numpy as np

# ESP32的MJPEG视频流URL
STREAM_URL = "http://192.168.31.216/esp32"

# 创建一个会话以保持连接
session = requests.Session()

# 发起GET请求获取视频流
response = session.get(STREAM_URL, stream=True)

# 检查请求是否成功
if response.status_code != 200:
    print(f"Failed to connect to the stream. Status code: {response.status_code}")
    exit()

# 初始化字节流缓冲区
bytes_buffer = bytes()

# 遍历视频流的每一帧
for chunk in response.iter_content(chunk_size=1024):
    # 将数据块添加到缓冲区
    bytes_buffer += chunk

    # 查找JPEG图像的起始和结束标记
    a = bytes_buffer.find(b'\xff\xd8')  # JPEG起始标记
    b = bytes_buffer.find(b'\xff\xd9')  # JPEG结束标记

    # 如果找到完整的JPEG图像
    if a != -1 and b != -1:
        # 提取一帧JPEG图像
        jpg = bytes_buffer[a:b + 2]
        bytes_buffer = bytes_buffer[b + 2:]  # 移除已处理的帧数据

        # 将JPEG图像解码为OpenCV图像格式
        frame = cv2.imdecode(np.frombuffer(jpg, dtype=np.uint8), cv2.IMREAD_COLOR)

        # 检查图像是否有效
        if frame is not None:
            # 显示图像
            cv2.imshow('ESP32 Camera Stream', frame)

        # 按下 'q' 键退出
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

# 释放资源
cv2.destroyAllWindows()
session.close()