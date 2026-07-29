#ifndef SHT30_UART_H
#define SHT30_UART_H

#include "stm32f4xx_hal.h"
#include <stdint.h>

/**
 * @brief  启动 UART5 DMA + 空闲中断接收，并解析温度/深度
 * @note   在 HAL 库初始化并 MX_USART5_UART_Init() 后调用
 */
void sht30_Init(void);
void UART5_IT_TASK(void);
void sht30_parse_frame(const uint8_t *buf, int len);
void sht30_task();

/* 解析得到的物理量，单位：℃、m */
extern volatile float sht30_temperature;
extern volatile float sht30_humidity;

extern uint8_t uart5_it_flag;
/* DMA 接收缓冲区大小（要大于一帧最大长度） */
#define UART5_DMA_BUF_SIZE  40

typedef struct {
    uint8_t  data[UART5_DMA_BUF_SIZE];
    uint16_t len;
} UART5_Msg_t;
extern UART5_Msg_t uart5_msg;



#endif /* SHT30_UART_H */
