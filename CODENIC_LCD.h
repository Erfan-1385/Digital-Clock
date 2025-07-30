/*
sahand rajabi
@CODENIC
WWW.CODENIC.IR
mr.sahandrjb@gmail.com
*/
#ifndef LCDCHAR_H
#define LCDCHAR_H 120

/**
 *	It also supports all HD44780 compatible LCD drivers.
 *
 *	\par Default pinout
 *	
@verbatim
LCD		STM32FXXX		DESCRIPTION

GND		GND				Ground
VCC		+5V				Power supply for LCD
V0		Potentiometer	Contrast voltage. Connect to potentiometer
RS		PB2				Register select, can be overwritten in your project's defines.h file
RW		GND				Read/write
E		PB7				Enable pin, can be overwritten in your project's defines.h file
D0		-				Data 0 - doesn't care
D1		-				Data 1 - doesn't care
D2		-				Data 2 - doesn't care
D3		-				Data 3 - doesn't  care
D4		PC12			Data 4, can be overwritten in your project's defines.h file
D5		PC13			Data 5, can be overwritten in your project's defines.h file
D6		PB12			Data 6, can be overwritten in your project's defines.h file
D7		PB13			Data 7, can be overwritten in your project's defines.h file
A		+3V3			Back light positive power
K		GND				Ground for back light
@endverbatim	
 *	
 * If you want to change pinout, do this in your defines.h file with lines below and set your own settings:
 *	
@verbatim
//RS - Register select pin
#define HD44780_RS_PORT     GPIOB
#define HD44780_RS_PIN      GPIO_PIN_2
//E - Enable pin
#define HD44780_E_PORT      GPIOB
#define HD44780_E_PIN       GPIO_PIN_7
//D4 - Data 4 pin
#define HD44780_D4_PORT     GPIOC
#define HD44780_D4_PIN      GPIO_PIN_12
//D5 - Data 5 pin
#define HD44780_D5_PORT     GPIOC
#define HD44780_D5_PIN      GPIO_PIN_13
//D6 - Data 6 pin
#define HD44780_D6_PORT     GPIOB
#define HD44780_D6_PIN      GPIO_PIN_12
//D7 - Data 7 pin
#define HD44780_D7_PORT     GPIOB
#define HD44780_D7_PIN      GPIO_PIN_13
 */
#include "stm32f1xx.h"
//#include "gpio.h"

/**
 * @defgroup LCDCHAR_Macros
 * @brief    Library defines
 * @{
 */
//RS - Register select pin
#define HD44780_RS_PORT     GPIOA
#define HD44780_RS_PIN      GPIO_PIN_1
//E - Enable pin
#define HD44780_E_PORT      GPIOA
#define HD44780_E_PIN       GPIO_PIN_3
//D4 - Data 4 pin
#define HD44780_D4_PORT     GPIOA
#define HD44780_D4_PIN      GPIO_PIN_4
//D5 - Data 5 pin
#define HD44780_D5_PORT     GPIOA
#define HD44780_D5_PIN      GPIO_PIN_5
//D6 - Data 6 pin
#define HD44780_D6_PORT     GPIOA
#define HD44780_D6_PIN      GPIO_PIN_6
//D7 - Data 7 pin
#define HD44780_D7_PORT     GPIOA
#define HD44780_D7_PIN      GPIO_PIN_7
/* 4 bit mode */
/* Control pins, can be overwritten */
/* RS - Register select pin */
#ifndef HD44780_RS_PIN
#define HD44780_RS_PORT				LCD_RS_GPIO_Port
#define HD44780_RS_PIN				LCD_RS_Pin
#endif
/* E - Enable pin */
#ifndef HD44780_E_PIN
#define HD44780_E_PORT				LCD_E_GPIO_Port
#define HD44780_E_PIN				LCD_E_Pin
#endif
/* Data pins */
/* D4 - Data 4 pin */
#ifndef HD44780_D4_PIN
#define HD44780_D4_PORT				LCD_DB4_GPIO_Port
#define HD44780_D4_PIN				LCD_DB4_Pin
#endif
/* D5 - Data 5 pin */
#ifndef HD44780_D5_PIN
#define HD44780_D5_PORT				LCD_DB5_GPIO_Port
#define HD44780_D5_PIN				LCD_DB5_Pin
#endif
/* D6 - Data 6 pin */
#ifndef HD44780_D6_PIN
#define HD44780_D6_PORT				LCD_DB6_GPIO_Port
#define HD44780_D6_PIN				LCD_DB6_Pin
#endif
/* D7 - Data 7 pin */
#ifndef HD44780_D7_PIN
#define HD44780_D7_PORT				LCD_DB7_GPIO_Port
#define HD44780_D7_PIN				LCD_DB7_Pin
#endif

/**
 * @}
 */

/**
 * @defgroup LCDCHAR_Functions
 * @brief    Library Functions
 * @{
 */

/**
 * @brief  Initializes HD44780 LCD
 * @brief  cols: width of lcd
 * @param  rows: height of lcd
 * @retval None
 */
void LCDCHAR_Init(uint8_t cols, uint8_t rows);

/**
 * @brief  Turn display on
 * @param  None
 * @retval None
 */
void LCDCHAR_DisplayOn(void);

/**
 * @brief  Turn display off
 * @param  None
 * @retval None
 */
void LCDCHAR_DisplayOff(void);

/**
 * @brief  Clears entire LCD
 * @param  None
 * @retval None
 */
void LCDCHAR_Clear(void);

/**
 * @brief  Puts string on lcd
 * @param  x location
 * @param  y location
 * @param  *str: pointer to string to display
 * @retval None
 */
void LCDCHAR_Puts(uint8_t x, uint8_t y, char* str);

/**
 * @brief  Enables cursor blink
 * @param  None
 * @retval None
 */
void LCDCHAR_BlinkOn(void);

/**
 * @brief  Disables cursor blink
 * @param  None
 * @retval None
 */
void LCDCHAR_BlinkOff(void);

/**
 * @brief  Shows cursor
 * @param  None
 * @retval None
 */
void LCDCHAR_CursorOn(void);

/**
 * @brief  Hides cursor
 * @param  None
 * @retval None
 */
void LCDCHAR_CursorOff(void);

/**
 * @brief  Scrolls display to the left
 * @param  None
 * @retval None
 */
void LCDCHAR_ScrollLeft(void);

/**
 * @brief  Scrolls display to the right
 * @param  None
 * @retval None
 */
void LCDCHAR_ScrollRight(void);

/**
 * @brief  Creates custom character
 * @param  location: Location where to save character on LCD. LCD supports up to 8 custom characters, so locations are 0 - 7
 * @param *data: Pointer to 8-bytes of data for one character
 * @retval None
 */
void LCDCHAR_CreateChar(uint8_t location, uint8_t* data);

/**
 * @brief  Puts custom created character on LCD
 * @param  location: Location on LCD where character is stored, 0 - 7
 * @retval None
 */
void LCDCHAR_PutCustom(uint8_t x, uint8_t y, uint8_t location);


void LCDCHAR_CursorSet(uint8_t col, uint8_t row);


void EC_LCD_Puts(char* str);


#endif

/*
sahand rajabi
@CODENIC
WWW.CODENIC.IR
mr.sahandrjb@gmail.com
*/
