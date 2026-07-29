#include "sht30.h"
#include "ms5837_uart.h"
#include "usart.h"    // CubeMX 生成的 huart5 句柄
#include "stm32f4xx_hal.h"
#include <string.h>
#include "jy901.h"
#include "math.h"    // sinf
#include "stdio.h"
#include "FreeRTOS.h"
#include "task.h"
UART5_Msg_t uart5_msg;
uint8_t uart5_it_flag=0;
uint8_t uart5_rx_buffer[UART5_DMA_BUF_SIZE];

/* 协议里“温度”中文前缀的 GBK 编码 */
static const uint8_t PREFIX_TEMP[4]  = {'T','e','m','p'};
/* 协议里“深度”中文前缀的 GBK 编码 */
static const uint8_t PREFIX_DEPTH[4] = {'H','u','m','i'};


/* 解析结果 */
volatile float sht30_temperature = 0.0f;
volatile float sht30_humidity       = 0.0f;

/**
 * @brief  从 buf 中以“前缀 + ':' + ASCII 数字 + '.' + ASCII 数字”格式解析一个数值
 * @param  buf   起始地址
 * @param  len   buf 可读长度
 * @param  prefix 4 字节前缀编码
 * @param  out   结果（带符号，减去 0x30）
 * @return  >=0 成功解析到并写入 *out，返回消耗的字节数；否则返回 -1
 */
static int parse_value(const uint8_t *buf, int len,
                       const uint8_t prefix[4],
                       float *out)
{
    int i = 0;
    // 找前缀
    for (; i + 5 < len; i++)
    {
        if (memcmp(buf + i, prefix, 4) == 0)
            break;
    }
    if (i + 5 >= len) return -1;
    int j = i + 5+2;
    int sign = 1;
    int iv = 0, fv = 0, fd = 0;
    // 整数部分
    while (j < len && buf[j] != 0x2E /*'.'*/)
    {
        if (buf[j] == 0x2D /*'-'*/) { sign = -1; }
        else if (buf[j] >= '0' && buf[j] <= '9')
        {
            iv = iv * 10 + (buf[j] - '0');
        }
        else break;
        j++;
    }
    if (j >= len || buf[j] != 0x2E) return -1;
    j++; // 跳过 '.'
    // 小数两位
    for (fd = 0; fd < 2 && j < len; fd++, j++)
    {
        if (buf[j] >= '0' && buf[j] <= '9')
            fv = fv * 10 + (buf[j] - '0');
        else break;
    }
    // 组装
    *out = sign * (iv + fv / (float)(fd == 0 ? 1 : (fd==1?10:100)));
    return j - i;
}

/**
 * @brief  接收一段 DMA 数据后一次性调用，提取“温度”和“深度”
 */
void sht30_parse_frame(const uint8_t *buf, int len)
{
    // 解析温度
    parse_value(buf, len, PREFIX_TEMP, &sht30_temperature);
    // 解析深度
    parse_value(buf, len, PREFIX_DEPTH, &sht30_humidity);

  
}

/**
 * @brief  初始化，启动 DMA + 空闲中断接收
 */
void sht30_Init(void)
{
    // 启动 DMA 接收
    HAL_UART_Receive_DMA(&huart5, uart5_rx_buffer, UART5_DMA_BUF_SIZE);

    // 使能空闲中断
    __HAL_UART_ENABLE_IT(&huart5, UART_IT_IDLE);

}
int b_p=0;
void UART5_IT_TASK(void)
{
    // 空闲检测
    if (__HAL_UART_GET_FLAG(&huart5, UART_FLAG_IDLE))
    {
        __HAL_UART_CLEAR_IDLEFLAG(&huart5);
			
        // 停 DMA，算长度
        HAL_UART_DMAStop(&huart5);
        uint16_t len = UART5_DMA_BUF_SIZE - __HAL_DMA_GET_COUNTER(huart5.hdmarx);

        if (uart5_it_flag == 0)
        {
					if(b_p<30)
					{
						uart5_msg.len = len;
            memcpy(uart5_msg.data+b_p, uart5_rx_buffer, len);
						b_p+=len;
					}
					else
					{
						uart5_it_flag = 1;
						b_p=0;
					}
            
          //sht30_parse_frame(uart5_msg.data, uart5_msg.len);
					
					//uart5_it_flag = 1;
        }
        // 重启 DMA
        HAL_UART_Receive_DMA(&huart5, uart5_rx_buffer, UART5_DMA_BUF_SIZE);
    }
}


	



uint8_t buffer[30];
int bp=0;

/*void sht30_task()
{
	
	
	while(1)
	{
		if(uart5_it_flag==1)
		{
		if(bp<30)
		{
			for(int i=0;i<uart5_msg.len;i++)
			{
				buffer[bp]=uart5_msg.data[i];
				bp++;
			}
			
		}
		else
		{
			sht30_parse_frame(buffer, 30);
			bp=0;
			
		}
		
		//char str[100];
			//sprintf(str,"press=%f\n",ms5837_depth);
			//HAL_UART_Transmit(&huart6,(uint8_t*)str,strlen(str),100);
		uart5_it_flag=0;
		
		}
		vTaskDelay(pdMS_TO_TICKS(100));
		
	}
}*/


void sht30_task()
{
	
	
	while(1)
	{
		if(uart5_it_flag==1)
		{
		
			sht30_parse_frame(uart5_msg.data, 30);
		
		//char str[100];
			//sprintf(str,"press=%f\n",ms5837_depth);
			//HAL_UART_Transmit(&huart6,(uint8_t*)str,strlen(str),100);
		uart5_it_flag=0;
		
		}
		vTaskDelay(pdMS_TO_TICKS(50));
		
	}
}


//

