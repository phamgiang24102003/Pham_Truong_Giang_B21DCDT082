#include "main.h"

TIM_HandleTypeDef htim4;
uint8_t dht11_data[5];

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM4_Init(void);
void delay_us(uint16_t us);
void DHT11_Read(void);

int main(void)
{
	HAL_Init();
	SystemClock_Config();
	MX_GPIO_Init();
	MX_TIM4_Init();
	HAL_TIM_Base_Start(&htim4);
	while (1)
	{
		DHT11_Read();
		HAL_Delay(2000);
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
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
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

static void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_Pin, GPIO_PIN_RESET);
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

void DHT11_Read(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_Pin, GPIO_PIN_RESET);
	HAL_Delay(20);

	HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_Pin, GPIO_PIN_SET);
	delay_us(30);

	GPIO_InitStruct.Pin = DHT11_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);

	__HAL_TIM_SET_COUNTER(&htim4, 0);
	while (HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin) == GPIO_PIN_RESET)
	{
		if (__HAL_TIM_GET_COUNTER(&htim4) >= 10)
			goto error;
	}

	__HAL_TIM_SET_COUNTER(&htim4, 0);
	while (HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin) == GPIO_PIN_SET)
	{
		if (__HAL_TIM_GET_COUNTER(&htim4) >= 45 || __HAL_TIM_GET_COUNTER(&htim4) <= 5)
			goto error;
	}

	__HAL_TIM_SET_COUNTER(&htim4, 0);
	while (HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin) == GPIO_PIN_RESET)
	{
		if (__HAL_TIM_GET_COUNTER(&htim4) >= 90 || __HAL_TIM_GET_COUNTER(&htim4) <= 70)
			goto error;
	}

	__HAL_TIM_SET_COUNTER(&htim4, 0);
	while (HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin) == GPIO_PIN_SET)
	{
		if (__HAL_TIM_GET_COUNTER(&htim4) >= 95 || __HAL_TIM_GET_COUNTER(&htim4) <= 75)
			goto error;
	}

	for (int j = 0; j < 5; j++)
	{
		for (int i = 0; i < 8; ++i)
		{
			__HAL_TIM_SET_COUNTER(&htim4, 0);
			while (HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin) == GPIO_PIN_RESET)
			{
				if (__HAL_TIM_GET_COUNTER(&htim4) >= 65 || __HAL_TIM_GET_COUNTER(&htim4) <= 45)
					goto error;
			}

			__HAL_TIM_SET_COUNTER(&htim4, 0);
			while (HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin) == GPIO_PIN_SET)
			{
				if (__HAL_TIM_GET_COUNTER(&htim4) >= 80 || __HAL_TIM_GET_COUNTER(&htim4) <= 10)
					goto error;
			}
		}

		dht11_data[j] <<= 1;
		if (__HAL_TIM_GET_COUNTER(&htim4) > 45)
		{
			dht11_data[j] |= 1;
		}
		else
		{
			dht11_data[j] &= ~1;
		}
	}

	uint8_t sum = dht11_data[0] + dht11_data[1] + dht11_data[2] + dht11_data[3];
	if (sum != dht11_data[4])
		goto error;

	error:
	GPIO_InitStruct.Pin = DHT11_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);
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
