#include "UDP.h"
#include "esp_log.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"

static const char *TAG = "UDP";

// 全局变量，用于存储UDP接收数据
static char udp_receive_buffer[1024];
static int udp_receive_length = 0;

// 初始化UDP客户端
void UDP_Init(void) {
    // 清空接收缓冲区
    memset(udp_receive_buffer, 0, sizeof(udp_receive_buffer));
    udp_receive_length = 0;
}

// 创建UDP套接字
static int create_udp_socket() {
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock < 0) {
        ESP_LOGE(TAG, "Failed to create UDP socket: errno %d", errno);
        return -1;
    }
    return sock;
}

// 发送UDP数据
int UDP_Send(uint8_t* data, size_t data_len) {
    struct sockaddr_in dest_addr;
    dest_addr.sin_addr.s_addr = inet_addr(UDP_HostIPAddress); // 目标IP地址
    dest_addr.sin_family = AF_INET;            // IPv4
    dest_addr.sin_port = htons(UDP_HostPort);          // 目标端口号

    // 创建UDP套接字
    int sock = create_udp_socket();
    if (sock < 0) {
        return -1;
    }

    // 发送数据
    int err = sendto(sock, data, data_len, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err < 0) {
        ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
        close(sock);
        return -1;
    }

    ESP_LOGI(TAG, "UDP message sent to %s:%d", UDP_HostIPAddress, UDP_HostPort);
    close(sock);
    return err;
}

// 接收UDP数据
int UDP_Receive(int port, char *buffer, size_t buffer_len, int timeout_ms) {
    struct sockaddr_in source_addr;
    socklen_t addr_len = sizeof(source_addr);

    // 创建UDP套接字
    int sock = create_udp_socket();
    if (sock < 0) {
        return -1;
    }

    // 绑定到本地端口
    struct sockaddr_in local_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr.s_addr = htonl(INADDR_ANY) // 监听所有网络接口
    };
    if (bind(sock, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0) {
        ESP_LOGE(TAG, "Failed to bind UDP socket: errno %d", errno);
        close(sock);
        return -1;
    }

    // 设置接收超时
    struct timeval timeout = {
        .tv_sec = timeout_ms / 1000,
        .tv_usec = (timeout_ms % 1000) * 1000
    };
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    // 接收数据
    int len = recvfrom(sock, buffer, buffer_len - 1, 0, (struct sockaddr *)&source_addr, &addr_len);
    if (len < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            ESP_LOGI(TAG, "UDP receive timeout");
        } else {
            ESP_LOGE(TAG, "Error occurred during receiving: errno %d", errno);
        }
        close(sock);
        return -1;
    }

    // 确保缓冲区以 null 结尾
    buffer[len] = '\0';
    ESP_LOGI(TAG, "Received UDP message from %s:%d: %s",
             inet_ntoa(source_addr.sin_addr), ntohs(source_addr.sin_port), buffer);

    close(sock);
    return len;
}

// 启动UDP服务器
void UDP_Start_Server(int port, void (*handler)(const char *data, int len)) {
    while (1) {
        // 接收数据
        int len = UDP_Receive(port, udp_receive_buffer, sizeof(udp_receive_buffer), 5000);
        if (len > 0) {
            // 调用回调函数处理数据
            if (handler) {
                handler(udp_receive_buffer, len);
            }
        }
    }
}