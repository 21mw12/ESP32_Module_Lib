#ifndef __Camera_H_
#define __Camera_H_

#include <stdio.h>
#include "esp_err.h"
#include "esp_camera.h"
#include "esp_http_server.h"

// Pin configuration for the camera
#define CAM_PIN_PWDN    32
#define CAM_PIN_RESET   -1 // Software reset
#define CAM_PIN_XCLK    0
#define CAM_PIN_SIOD    26
#define CAM_PIN_SIOC    27
#define CAM_PIN_D7      35
#define CAM_PIN_D6      34
#define CAM_PIN_D5      39
#define CAM_PIN_D4      36
#define CAM_PIN_D3      21
#define CAM_PIN_D2      19
#define CAM_PIN_D1      18
#define CAM_PIN_D0      5
#define CAM_PIN_VSYNC   25
#define CAM_PIN_HREF    23
#define CAM_PIN_PCLK    22

esp_err_t Camera_Init(void);

esp_err_t Camera_Capture_Handler(httpd_req_t *req);

esp_err_t Camera_jpg_Stream_Handler(httpd_req_t *req);

#endif