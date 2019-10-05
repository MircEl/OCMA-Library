/***************************************************************
	This is a library for OCMA320240t350-1A 3.5" 320x240 TFT
	display	with UART communication and STM32 controller
	with RA8875 for Arduino.

	This has been done in the spare time, with no special
	academic training in programming, so it might not be
	the most efficient way to controll it. I could not find
	the English datasheet, so this was done from a Chinese
	one translated with Google Translate. Please take it
	as is and modify it as you please for your needs.
	
	Written by Veleanu Mircea.
***************************************************************/

//the OCMA320240t350-1A display receives 8-bit instruction packets in MSBF order
//the instruction set has the following construction:
// [0xFD]			[instruction code]			 	[data length] 				[data] 						[0xDD 0xCC 0xBB 0xAA]
// start flag		instruction code (1 byte)		data size (2 bytes)     	data (max 2000 bytes)		instruction end flag (4 bytes)

#ifndef OCMA_h
#define OCMA_h

#include "Arduino.h"


//flags for UART instructions
#define OCMA_HandShakeFlag 0x00
#define OCMA_BaudRateFlag 0x01
#define OCMA_ClearScreenFlag 0x02
#define OCMA_SetColorFlag 0x05
#define OCMA_DwPixelBgColorFlag 0x06 //not working for me
#define OCMA_DwPixelFgColorFlag 0x07 //not working for me
#define OCMA_FontModeFlag 0x08
#define OCMA_BacklightOffFlag 0x09
#define OCMA_BacklightOnFlag 0x0A
#define OCMA_BacklightLevelFlag 0x0B
#define OCMA_ScanModeFlag 0x0C	//sets scan mode of screen
#define OCMA_BuzzerTyepFlag 0x0D	//to be added
#define OCMA_CharSize10Flag 0x10	//6x10 ASCII dot matrix characters
#define OCMA_CharSize16Flag 0x11	//8x16 ASCII dot matrix characters
#define OCMA_CharSize16faFlag 0x12  //8x16 ASCII or 16x16 "full angle GB2312" dot matrix characters
#define OCMA_CharSize32faFlag 0x13	//16x32 ASCII or 32x32 "full angle GB2312" dot matrix characters
#define OCMA_CharSize24faFlag 0x14	//12x24 ASCII or 24x24 "full angle GB2312" dot matrix characters
#define OCMA_CharSize24Flag 0x15	//12x24 ASCII dot matrix characters
#define OCMA_CharSize32Flag 0x16	//16x32 ASCII dot matrix characters
#define OCMA_DrawPixelsFlag 0x21
#define OCMA_DrawLineFlag 0x23
#define OCMA_DrawCircleFlag 0x26
#define OCMA_DrawRectFlag 0x27
#define OCMA_DrawTriangleFlag 0x28
#define OCMA_DrawElipseFlag 0x29
#define OCMA_DrawRoundRectFlag 0x2a
#define OCMA_DrawSemiElipseFlag 0x2b	//to be added
#define OCMA_DrawFullScreenPicFlag 0x30
#define OCMA_SavePicFlag 0x31	//to be added
#define OCMA_SaveScreenFlag 0x32
#define OCMA_DrawPicFlag 0x34
#define OCMA_SendPicFlag 0x35	//to send pictures to the display. not sure how to use this, in datasheet it's indicated to use their computer software for that
#define OCMA_DrawCroppedPicFlag 0x37
#define OCMA_TouchPanelCalibrateFlag 0x50
#define OCMA_TouchFeedbackFlag 0x51
#define OCMA_TouchPanelModeFlag 0x52
#define OCMA_TouchPanelTestFlag 0x53 //to test the touch screen using given software OCM-A.exe
#define OCMA_DrawPageIDFlag 0x70
#define OCMA_DrawTPImageFlag 0x71
#define OCMA_ReturnTPImageFlag 0x72
#define OCMA_DrawTextInShapeFlag 0x73
#define OCMA_DrawTPCroppedImageFlag 0x74
#define OCMA_DrawCursorFlag 0x80
#define OCMA_CursorModeFlag 0x85
#define OCMA_WriteLayerSelectFlag 0x90
#define OCMA_LayerDrawFlag 0x91

//baudrates flags
#define Bd1200 0x00
#define Bd2400 0x01
#define Bd4800 0x02
#define Bd9600 0x03
#define Bd14400 0x04
#define Bd19200 0x05
#define Bd38400 0x06
#define Bd56000 0x07
#define Bd57600 0x08
#define Bd115200 0xff
#define Bd230400 0x8b
#define Bd460800 0x8d
#define Bd921600 0x8f

//text mode bit flags
#define NOBGR0 B00000000	//background color, no rotation
#define BGR0 B00000001		//no background color, no rotation (default)
#define BGR90 B10000001		//background color, 90 degrees rotation
#define NOBGR90 B10000000	//no background color, 90 degrees rotation

//scan mode bit flags
#define TBLR B00000000		//Top to bottom, left to right (default)
#define TBRL B00001000		//Top to bottom, right to left
#define BTLR B00000100		//Bottom to top, left to right
#define BTRL B00001100		//Bottom to top, right to left

//touch panel modes bit flags
#define TPM0 0x00			//disable automatic upload on touch; use command to read panel status
#define TPM1 0x01			//coordonates uploaded to serial as soon as touch panel is pressed
#define TPM2 0x02			//touch panel status is uploaded automatically on finger lift
#define TPM3 0x03			//touch panel status uploaded every 100ms
#define TPM4 0x04			//touch panel status uploaded on press and release

//layer flags
#define DM1 0x01	//display layer 1, or layer 2, or both with transparency
#define DM2 0x02	//display both layers with layer 2 overlayed with color filter

#define OCMA_LAYER1 0x00
#define LT78 0x01
#define LT68 0x02
#define LT58 0x03
#define LT48 0x04
#define LT38 0x05
#define LT28 0x06
#define LT18 0x07
#define OCMA_LAYER2 0x08

class OCMA {
	
	public:
		OCMA(void);
		
		
		void init(uint32_t baudr);
		uint32_t showBaud(void);
		void clearScreen(void);		
		
		void background(uint16_t color);
		void background(uint8_t r, uint8_t g, uint8_t b);
		void stroke(uint16_t color);
		void stroke(uint8_t r, uint8_t g, uint8_t b);
		
		// text print
		void textSize(uint8_t size);
		void textMode(uint8_t mode);
		void textMode(uint8_t mode, uint16_t bgcolor);
		void setCursor(uint16_t x, uint16_t y);
		void print(char *c);
		void print(char c);
		void print(uint16_t x);
		void print(uint32_t x);
		void print(int x);
		void print(long x);
		void print(double x);
		void print(double x, uint8_t digits);
		
		//drawing and shapes
		void drawPixel(uint16_t x, uint16_t y);
		void drawPixels(uint16_t *x, uint16_t *y, uint16_t n);
		void drawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
		void drawLines(uint16_t *x, uint16_t *y, uint16_t n);
		void drawPoligon(uint16_t *x, uint16_t *y, uint16_t n);
		void drawRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
		void fillRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
		void drawCircle(uint16_t x, uint16_t y, uint8_t r);
		void fillCircle(uint16_t x, uint16_t y, uint8_t r);
		void drawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3);
		void fillTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3);
		void drawElipse(uint16_t x, uint16_t y, uint16_t r1, uint16_t r2);
		void fillElipse(uint16_t x, uint16_t y, uint16_t r1, uint16_t r2);
		void drawSemiElipse(uint8_t m, uint16_t x, uint16_t y, uint16_t r1, uint16_t r2);
		void fillSemiElipse(uint8_t m, uint16_t x, uint16_t y, uint16_t r1, uint16_t r2);
		void drawRoundRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t r1, uint16_t r2);
		void fillRoundRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t r1, uint16_t r2);
		
		//touch panel
		void calibrateTouchPanel(void);
		void touchMode(uint8_t m);
		void touchRead(void);
		bool isTouched(void);
		bool isReleased(void);
		uint16_t touchX(void);
		uint16_t touchY(void);
		uint32_t lastTouched(void);
		uint32_t lastReleased(void);
		uint16_t touchPX(void);
		uint16_t touchPY(void);
		uint16_t touchRX(void);
		uint16_t touchRY(void);
		
		//layers (not working on v1.5)
		void setWriteLayer(uint8_t layer);
		void setDisplayLayer(uint8_t layer);
		void setLayerTransparency(uint8_t transp);
		void setFilteredDisplayLayer(uint16_t color);
		void fadeLayers(uint16_t delay);
		
		//images
		void drawImg(uint16_t id);
		void saveScreen(uint16_t id);
		void drawCroppedImage(uint16_t x, uint16_t y, uint16_t id, uint16_t ix, uint16_t iy, uint16_t w, uint16_t h);
		
		//screen commands
		void brightness(uint8_t b);
		void setScanMode(uint8_t scan);
	
		//r,g,b to uin16_t color
		uint16_t color(uint8_t r, uint8_t g, uint8_t b);
		
		
		
	private:
		uint16_t cursor_x;
		uint16_t cursor_y;
		uint16_t fcolor;
		uint16_t bcolor;
		uint8_t wlayer;
		uint8_t dlayer;
		//uint32_t baudrate;
		uint8_t textsize;
		uint8_t textmode;
		uint16_t textbg;
		volatile uint16_t tpx;
		volatile uint16_t tpy;
		volatile uint16_t tpPx;
		volatile uint16_t tpRx;
		volatile uint16_t tpPy;
		volatile uint16_t tpRy;
		volatile uint32_t tpPms;
		volatile uint32_t tpRms;
		volatile bool touched;
		bool released;
		uint8_t tpmode;
	
		void send(uint8_t c);
		void cmdStart(void);
		void cmdEnd(void);
		void cmdSize(uint16_t s);
		void sendInt(uint16_t i);
		uint8_t read(void);
		uint16_t readInt(void);
		uint8_t Handshake(void);
		void sendBaudRate(uint8_t baud);
		void sendColor(uint16_t fg, uint16_t bg);
		void sendFontMode(uint8_t m, uint16_t color);
		void sendLayerMode1(uint8_t t);
		void sendLayerMode2(uint16_t color);
		void sendWriteLayer(uint8_t layer);
		bool saveImg(uint16_t id);
		void sendDrawIMG(uint16_t id);
		uint8_t findBaud(void);
		bool checkBaud(uint8_t baud);
		uint32_t flagToBaud(uint8_t baud);
		uint8_t baudToFlag(uint32_t baud);
		bool setBaud(uint32_t baudr);
		void sendString(uint16_t x, uint16_t y, char *c);
		void advanceCursor(uint16_t l);
		void sendRect(uint8_t fill, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
		void sendCircle(uint8_t fill, uint16_t x, uint16_t y, uint8_t r);
		void sendTriangle(uint8_t fill, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3);
		void sendRoundRect(uint8_t fill, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t r1, uint16_t r2);
		void sendElipse(uint8_t fill, uint16_t x, uint16_t y, uint16_t r1, uint16_t r2);
		void sendElipseCut(uint8_t m, uint16_t x, uint16_t y, uint16_t r1, uint16_t r2);
		void sendTPM(uint8_t);
		void sendTPRead(void);
		void backlightOFF(void);
		void backlightON(void);
		void sendBrightness(uint8_t b);
		bool touchRD(void);
		void sendScanMode(uint8_t scan);
		void touchInit(void);
	
	
};

#endif
