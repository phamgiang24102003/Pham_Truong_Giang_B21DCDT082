/* USER CODE BEGIN Header */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdio.h>
#include <string.h>

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c2;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
#define BH1750_ADDR (0x23 << 1)  // Địa chỉ I2C của BH1750
#define BH1750_POWER_ON  0x01
#define BH1750_RESET     0x07
#define BH1750_ONE_H_MODE  0x20  // Chế độ đo 1 lần, độ phân giải cao
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C2_Init(void);

/* USER CODE BEGIN PFP */
void BH1750_Init(void);
uint16_t BH1750_Read_Lux(void);
/* USER CODE END PFP */

int main(void) {
    /* MCU Configuration */
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART2_UART_Init();
    MX_I2C2_Init();

    /* Khởi tạo cảm biến BH1750 */
    BH1750_Init();

    char msg[50];

    while (1) {
        uint16_t lux = BH1750_Read_Lux();
        sprintf(msg, "Lux: %d\r\n", lux);
        HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
        HAL_Delay(1000);
    }
}

/* Hàm khởi tạo cảm biến BH1750 */
void BH1750_Init(void) {
    uint8_t cmd = BH1750_POWER_ON;
    HAL_I2C_Master_Transmit(&hi2c2, BH1750_ADDR, &cmd, 1, HAL_MAX_DELAY);
    cmd = BH1750_RESET;
    HAL_I2C_Master_Transmit(&hi2c2, BH1750_ADDR, &cmd, 1, HAL_MAX_DELAY);
}

/* Hàm đọc dữ liệu từ BH1750 */
uint16_t BH1750_Read_Lux(void) {
    uint8_t cmd = BH1750_ONE_H_MODE;
    uint8_t tmp8[2];

    HAL_I2C_Master_Transmit(&hi2c2, BH1750_ADDR, &cmd, 1, HAL_MAX_DELAY);
    HAL_Delay(180);
    HAL_I2C_Master_Receive(&hi2c2, BH1750_ADDR, tmp8, 2, HAL_MAX_DELAY);

    uint16_t lux = ((tmp8[0] << 8) | tmp8[1]) / 1.2;
    return lux;
}

/* Cấu hình Clock */
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
        Error_Handler();
    }
}

/* Cấu hình I2C2 */
static void MX_I2C2_Init(void) {
    hi2c2.Instance = I2C2;
    hi2c2.Init.ClockSpeed = 100000;
    hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c2.Init.OwnAddress1 = 0;
    hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c2.Init.OwnAddress2 = 0;
    hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2c2) != HAL_OK) {
        Error_Handler();
    }
}

/* Cấu hình UART2 */
static void MX_USART2_UART_Init(void) {
    huart2.Instance = USART2;
    huart2.Init.BaudRate = 9600;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart2) != HAL_OK) {
        Error_Handler();
    }
}

/* Cấu hình GPIO */
static void MX_GPIO_Init(void) {
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
}

/* Xử lý lỗi */
void Error_Handler(void) {
    while (1) {}
}
