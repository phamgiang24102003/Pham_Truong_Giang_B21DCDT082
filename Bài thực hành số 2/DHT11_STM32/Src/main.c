#include "main.h"
#include <string.h>

UART_HandleTypeDef huart2;
TIM_HandleTypeDef htim4;
uint8_t dht11_data[5] = {0};

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM4_Init(void);
static void MX_USART2_UART_Init(void);
void delay_us(uint16_t us);
uint8_t DHT11_Read(void);
void Error_Handler(void);

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART2_UART_Init();
    MX_TIM4_Init();
    HAL_TIM_Base_Start(&htim4);

    char msg[50];

    while (1)
    {
        if (DHT11_Read() == 0) // if reading was successful
        {
            sprintf(msg, "Do am: %d.%d%%, Nhiet do: %d.%d*C\r\n",
                    dht11_data[0], dht11_data[1], dht11_data[2], dht11_data[3]);
            HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 1000);
        }
        else
        {
            sprintf(msg, "Loi doc DHT11\r\n");
            HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 1000);
        }

        HAL_Delay(1000);
    }
}

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }
}

static void MX_TIM4_Init(void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    htim4.Instance = TIM4;
    htim4.Init.Prescaler = 72-1;
    htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim4.Init.Period = 0xffff;
    htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
    {
        Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
    {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }
}

static void MX_USART2_UART_Init(void)
{
    huart2.Instance = USART2;
    huart2.Init.BaudRate = 9600;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart2) != HAL_OK)
    {
        Error_Handler();
    }
}

static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* Configure DHT11 pin as output Open Drain */
    HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_Pin, GPIO_PIN_SET);
    GPIO_InitStruct.Pin = DHT11_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);
}

void delay_us(uint16_t us)
{
    __HAL_TIM_SET_COUNTER(&htim4, 0);
    while (__HAL_TIM_GET_COUNTER(&htim4) < us);
}

uint8_t DHT11_Read(void)
{
    uint8_t i, j, temp;
    uint8_t error = 0;

    /* Clear data array */
    for (i = 0; i < 5; i++)
        dht11_data[i] = 0;

    /* Send start signal to DHT11 */
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Set pin as output */
    GPIO_InitStruct.Pin = DHT11_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);

    /* Send low pulse for at least 18ms */
    HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_Pin, GPIO_PIN_RESET);
    HAL_Delay(20);

    /* Send high pulse for 20-40us */
    HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_Pin, GPIO_PIN_SET);
    delay_us(30);

    /* Configure pin as input with pull-up */
    GPIO_InitStruct.Pin = DHT11_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);

    /* Check for DHT11 response (low pulse 80us, high pulse 80us) */
    /* Wait for low pulse */
    __HAL_TIM_SET_COUNTER(&htim4, 0);
    while (HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin) == GPIO_PIN_SET)
    {
        if (__HAL_TIM_GET_COUNTER(&htim4) > 100)
            return 1; /* Error: No response from DHT11 */
    }

    /* Wait for high pulse */
    __HAL_TIM_SET_COUNTER(&htim4, 0);
    while (HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin) == GPIO_PIN_RESET)
    {
        if (__HAL_TIM_GET_COUNTER(&htim4) > 100)
            return 1; /* Error: No response from DHT11 */
    }

    /* Wait for low pulse */
    __HAL_TIM_SET_COUNTER(&htim4, 0);
    while (HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin) == GPIO_PIN_SET)
    {
        if (__HAL_TIM_GET_COUNTER(&htim4) > 100)
            return 1; /* Error: No response from DHT11 */
    }

    /* Start reading 40 bits (5 bytes) data */
    for (i = 0; i < 5; i++)
    {
        for (j = 0; j < 8; j++)
        {
            /* Wait for rising edge (50us low pulse) */
            __HAL_TIM_SET_COUNTER(&htim4, 0);
            while (HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin) == GPIO_PIN_RESET)
            {
                if (__HAL_TIM_GET_COUNTER(&htim4) > 100)
                    return 1; /* Error: Timeout waiting for rising edge */
            }

            /* Measure high pulse width to determine bit value */
            __HAL_TIM_SET_COUNTER(&htim4, 0);
            while (HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin) == GPIO_PIN_SET)
            {
                if (__HAL_TIM_GET_COUNTER(&htim4) > 100)
                    return 1; /* Error: Timeout waiting for falling edge */
            }

            /* If high pulse is ~70us, bit is 1, if ~26-28us, bit is 0 */
            temp = __HAL_TIM_GET_COUNTER(&htim4);
            if (temp > 50)
            {
                /* Bit is 1 */
                dht11_data[i] |= (1 << (7 - j));
            }
        }
    }

    /* Verify checksum */
    if (dht11_data[4] != ((dht11_data[0] + dht11_data[1] + dht11_data[2] + dht11_data[3]) & 0xFF))
    {
        return 1; /* Error: Checksum mismatch */
    }

    /* Set pin back to output mode */
    GPIO_InitStruct.Pin = DHT11_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);
    HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_Pin, GPIO_PIN_SET);

    return 0; /* Success */
}

void Error_Handler(void)
{
    __disable_irq();
    while (1)
    {
    }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
}
#endif
