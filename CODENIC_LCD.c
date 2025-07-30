/*
sahand rajabi
@CODENIC
WWW.CODENIC.IR
mr.sahandrjb@gmail.com
*/
#include "CODENIC_LCD.h"
/* Private HD44780 structure */
typedef struct {
	uint8_t DisplayControl;
	uint8_t DisplayFunction;
	uint8_t DisplayMode;
	uint8_t Rows;
	uint8_t Cols;
	uint8_t currentX;
	uint8_t currentY;
} HD44780_Options_t;

/* Private functions */
//static void LCDCHAR_InitPins(void);
static void LCDCHAR_Cmd(uint8_t cmd);
static void LCDCHAR_Cmd4bit(uint8_t cmd);
static void LCDCHAR_Data(uint8_t data);
void LCDCHAR_CursorSet(uint8_t col, uint8_t row);

void DelayUS(uint32_t us)
{
	us=us*50;
	
	for(uint32_t i=0;i<us;i++)
	{
		__ASM("NOP");
	}
}

/* Private variable */
static HD44780_Options_t HD44780_Opts;

/* Pin definitions */
#define HD44780_RS_LOW              HAL_GPIO_WritePin(HD44780_RS_PORT, HD44780_RS_PIN, GPIO_PIN_RESET)
#define HD44780_RS_HIGH             HAL_GPIO_WritePin(HD44780_RS_PORT, HD44780_RS_PIN, GPIO_PIN_SET)
#define HD44780_E_LOW               HAL_GPIO_WritePin(HD44780_E_PORT, HD44780_E_PIN, GPIO_PIN_RESET);
#define HD44780_E_HIGH              HAL_GPIO_WritePin(HD44780_E_PORT, HD44780_E_PIN, GPIO_PIN_SET);

#define HD44780_Delay(x)            DelayUS(x)
#define HD44780_E_BLINK             HD44780_E_HIGH; HAL_Delay(1); HD44780_E_LOW; HAL_Delay(1);

/* Commands*/
#define HD44780_CLEARDISPLAY        0x01
#define HD44780_RETURNHOME          0x02
#define HD44780_ENTRYMODESET        0x04
#define HD44780_DISPLAYCONTROL      0x08
#define HD44780_CURSORSHIFT         0x10
#define HD44780_FUNCTIONSET         0x20
#define HD44780_SETCGRAMADDR        0x40
#define HD44780_SETDDRAMADDR        0x80

/* Flags for display entry mode */
#define HD44780_ENTRYRIGHT          0x00
#define HD44780_ENTRYLEFT           0x02
#define HD44780_ENTRYSHIFTINCREMENT 0x01
#define HD44780_ENTRYSHIFTDECREMENT 0x00

/* Flags for display on/off control */
#define HD44780_DISPLAYON           0x04
#define HD44780_CURSORON            0x02
#define HD44780_BLINKON             0x01

/* Flags for display/cursor shift */
#define HD44780_DISPLAYMOVE         0x08
#define HD44780_CURSORMOVE          0x00
#define HD44780_MOVERIGHT           0x04
#define HD44780_MOVELEFT            0x00

/* Flags for function set */
#define HD44780_8BITMODE            0x10
#define HD44780_4BITMODE            0x00
#define HD44780_2LINE               0x08
#define HD44780_1LINE               0x00
#define HD44780_5x10DOTS            0x04
#define HD44780_5x8DOTS             0x00




static void LCDCHAR_Cmd4bit(uint8_t cmd) {
	/* Set output port */
	HAL_GPIO_WritePin(HD44780_D7_PORT, HD44780_D7_PIN, (cmd & 0x08));
	HAL_GPIO_WritePin(HD44780_D6_PORT, HD44780_D6_PIN, (cmd & 0x04));
	HAL_GPIO_WritePin(HD44780_D5_PORT, HD44780_D5_PIN, (cmd & 0x02));
	HAL_GPIO_WritePin(HD44780_D4_PORT, HD44780_D4_PIN, (cmd & 0x01));
	HD44780_E_BLINK;
}

static void LCDCHAR_Cmd(uint8_t cmd) {
	/* Command mode */
	HD44780_RS_LOW;
	
	/* High nibble */
	LCDCHAR_Cmd4bit(cmd >> 4);
	/* Low nibble */
	LCDCHAR_Cmd4bit(cmd & 0x0F);
}

void LCDCHAR_DisplayOn(void) {
	HD44780_Opts.DisplayControl |= HD44780_DISPLAYON;
	LCDCHAR_Cmd(HD44780_DISPLAYCONTROL | HD44780_Opts.DisplayControl);
}

void LCDCHAR_Clear(void) {
	LCDCHAR_Cmd(HD44780_CLEARDISPLAY);
	HAL_Delay(3);
}


void LCDCHAR_Init(uint8_t cols, uint8_t rows) {
	
	/* Init pinout */
	//LCDCHAR_InitPins();
	
	/* At least 40ms */
	HAL_Delay(45);
	
	/* Set LCD width and height */
	HD44780_Opts.Rows = rows;
	HD44780_Opts.Cols = cols;
	
	/* Set cursor pointer to beginning for LCD */
	HD44780_Opts.currentX = 0;
	HD44780_Opts.currentY = 0;
	
	HD44780_Opts.DisplayFunction = HD44780_4BITMODE | HD44780_5x8DOTS | HD44780_1LINE;
	if (rows > 1) {
		HD44780_Opts.DisplayFunction |= HD44780_2LINE;
	}
	
	/* Try to set 4bit mode */
	LCDCHAR_Cmd4bit(0x03);
	HAL_Delay(4);
	
	/* Second try */
	LCDCHAR_Cmd4bit(0x03);
	HAL_Delay(4);
	
	/* Third goo! */
	LCDCHAR_Cmd4bit(0x03);
	HAL_Delay(4);	
	
	/* Set 4-bit interface */
	LCDCHAR_Cmd4bit(0x02);
	HAL_Delay(4);
	
	/* Set # lines, font size, etc. */
	LCDCHAR_Cmd(HD44780_FUNCTIONSET | HD44780_Opts.DisplayFunction);

	/* Turn the display on with no cursor or blinking default */
	HD44780_Opts.DisplayControl = HD44780_DISPLAYON;
	LCDCHAR_DisplayOn();
	
	/* Clear lcd */
	LCDCHAR_Clear();
	
	/* Default font directions */
	HD44780_Opts.DisplayMode = HD44780_ENTRYLEFT | HD44780_ENTRYSHIFTDECREMENT;
	LCDCHAR_Cmd(HD44780_ENTRYMODESET | HD44780_Opts.DisplayMode);

	/* Delay */
	HAL_Delay(4);
		
}

void LCDCHAR_CursorSet(uint8_t col, uint8_t row) {
	uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};
	
	/* Go to beginning */
	if (row >= HD44780_Opts.Rows) {
	row = 0;
	}
	
	/* Set current column and row */
	HD44780_Opts.currentX = col;
	HD44780_Opts.currentY = row;
	
	/* Set location address */
	LCDCHAR_Cmd(HD44780_SETDDRAMADDR | (col + row_offsets[row]));
}


static void LCDCHAR_Data(uint8_t data) {
	/* Data mode */
	HD44780_RS_HIGH;
	
	/* High nibble */
	LCDCHAR_Cmd4bit(data >> 4);
	/* Low nibble */
	LCDCHAR_Cmd4bit(data & 0x0F);
}


void LCDCHAR_Puts(uint8_t x, uint8_t y, char* str) {
  
	LCDCHAR_CursorSet(x, y);
	while (*str) {
		if (HD44780_Opts.currentX >= HD44780_Opts.Cols) {
			HD44780_Opts.currentX = 0;
			HD44780_Opts.currentY++;
			LCDCHAR_CursorSet(HD44780_Opts.currentX, HD44780_Opts.currentY);
		}
		if (*str == '\n') {
			HD44780_Opts.currentY++;
			LCDCHAR_CursorSet(HD44780_Opts.currentX, HD44780_Opts.currentY);
		} else if (*str == '\r') {
			LCDCHAR_CursorSet(0, HD44780_Opts.currentY);
		} else {
			LCDCHAR_Data(*str);
			HD44780_Opts.currentX++;
		}
		str++;
	}

}



void LCDCHAR_DisplayOff(void) {
	HD44780_Opts.DisplayControl &= ~HD44780_DISPLAYON;
	LCDCHAR_Cmd(HD44780_DISPLAYCONTROL | HD44780_Opts.DisplayControl);
}

void LCDCHAR_BlinkOn(void) {
	HD44780_Opts.DisplayControl |= HD44780_BLINKON;
	LCDCHAR_Cmd(HD44780_DISPLAYCONTROL | HD44780_Opts.DisplayControl);
}

void LCDCHAR_BlinkOff(void) {
	HD44780_Opts.DisplayControl &= ~HD44780_BLINKON;
	LCDCHAR_Cmd(HD44780_DISPLAYCONTROL | HD44780_Opts.DisplayControl);
}

void LCDCHAR_CursorOn(void) {
	HD44780_Opts.DisplayControl |= HD44780_CURSORON;
	LCDCHAR_Cmd(HD44780_DISPLAYCONTROL | HD44780_Opts.DisplayControl);
}

void LCDCHAR_CursorOff(void) {
	HD44780_Opts.DisplayControl &= ~HD44780_CURSORON;
	LCDCHAR_Cmd(HD44780_DISPLAYCONTROL | HD44780_Opts.DisplayControl);
}

void LCDCHAR_ScrollLeft(void) {
	LCDCHAR_Cmd(HD44780_CURSORSHIFT | HD44780_DISPLAYMOVE | HD44780_MOVELEFT);
}

void LCDCHAR_ScrollRight(void) {
	LCDCHAR_Cmd(HD44780_CURSORSHIFT | HD44780_DISPLAYMOVE | HD44780_MOVERIGHT);
}


void LCDCHAR_CreateChar(uint8_t location, uint8_t *data) {
	uint8_t i;
	/* We have 8 locations available for custom characters */
	location &= 0x07;
	LCDCHAR_Cmd(HD44780_SETCGRAMADDR | (location << 3));
	
	for (i = 0; i < 8; i++) {
		LCDCHAR_Data(data[i]);
	}
}


void LCDCHAR_PutCustom(uint8_t x, uint8_t y, uint8_t location) {
	LCDCHAR_CursorSet(x, y);
	LCDCHAR_Data(location);
}



/*
sahand rajabi
@CODENIC
WWW.CODENIC.IR
mr.sahandrjb@gmail.com
*/
