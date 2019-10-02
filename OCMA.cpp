#include "OCMA.h"
#include "Arduino.h"


/*************************************************************************
							High level commands
*************************************************************************/

//constructor
OCMA::OCMA(void){
}


//initialize the serial; try to set the baud rate 5 times
void OCMA::init(uint32_t baudr){
	int i=1;
	while(!setBaud(baudr)){
		i++;
		if(i>5) break;
	}
	
	//baudrate=flagToBaud(findBaud());		//baudrate variable... for further use!
	cursor_x=cursor_y=0;
	fcolor=0xffff;
	bcolor=0x0000;
	wlayer=dlayer=1;
	textbg=0x00;
	tpPms = 0;
	tpRms = 0;
	tpmode=1;
	touched=0;
	released=0;
	touchMode(tpmode);
	setScanMode(1);
	textSize(2);
	sendColor(fcolor, bcolor);
	textMode(1);
	setWriteLayer(1);
	setDisplayLayer(1);
	brightness(255);
	backlightON();
	clearScreen();
}


//function to set baud rate
bool OCMA::setBaud(uint32_t baudr){
	
	if(findBaud() != baudToFlag(baudr)) {
		sendBaudRate(baudToFlag(baudr));
		delay(10);
		return (checkBaud(baudToFlag(baudr)));
	}
	else return 1;	
}

//finds current screen baud rate; will reset to screen factory baud rate if it fails
uint8_t OCMA::findBaud(void){
	if(checkBaud(Bd19200)) return Bd19200;
	else if(checkBaud(Bd1200)) return Bd1200;
	else if(checkBaud(Bd2400)) return Bd2400;
	else if(checkBaud(Bd4800))return Bd4800;
	else if(checkBaud(Bd9600)) return Bd9600;
	else if(checkBaud(Bd14400)) return Bd14400;
	else if(checkBaud(Bd38400)) return Bd38400;
	else if(checkBaud(Bd56000)) return Bd56000;
	else if(checkBaud(Bd57600)) return Bd57600;
	else if(checkBaud(Bd115200)) return Bd115200;
	//else if(checkBaud(Bd230400)) return Bd230400;
	//else if(checkBaud(Bd460800)) return Bd460800;
	//else if(checkBaud(Bd921600)) return Bd921600;
	
	else checkBaud(Bd19200); return Bd19200; // return to factory baud rate of screen
}

//checks baud rate. also sets the arduino to the baudrate checked.
bool OCMA::checkBaud(uint8_t baud){
	Serial.end();
	delay(10);
	Serial.begin(flagToBaud(baud));
	delay(10);
	return (Handshake() == baud);	
}

//changes background writing color
void OCMA::background(uint16_t color){
	bcolor=color;
	sendColor(fcolor, bcolor);
}

//changes foregrownd writing color
void OCMA::stroke(uint16_t color){
	fcolor=color;
	sendColor(fcolor, bcolor);
}

//changes text size
void OCMA::textSize(uint8_t size){
	if(size == 1) textsize = OCMA_CharSize10Flag;
	else if(size == 2) textsize = OCMA_CharSize16Flag;
	else if(size == 3) textsize = OCMA_CharSize24Flag;
	else if(size == 4) textsize = OCMA_CharSize32Flag;
	else textsize = OCMA_CharSize16Flag;
}

//sets text mode; mode 1 = normal, mode 2 = with background, mode 3 = rotated 90 degrees, mode 4 = rotated 90 degrees with background
void OCMA::textMode(uint8_t mode){
	if(mode == 1) textmode = NOBGR0;
	else if(mode == 2) textmode = BGR0;
	//else if(mode == 3) textmode = NOBGR90;	//these are only for "full angle" characters
	//else if(mode == 4) textmode = BGR90;		//these are only for "full angle" characters
	else textmode = NOBGR0;
	
	sendFontMode(textmode, textbg);
}

//sets text mode and background color
void OCMA::textMode(uint8_t mode, uint16_t color){
	textbg = color;
	
	if(mode == 1) textmode = NOBGR0;
	else if(mode == 2) textmode = BGR0;
	//else if(mode == 3) textmode = NOBGR90;	//these are only for "full angle" characters
	//else if(mode == 4) textmode = BGR90;		//these are only for "full angle" characters
	else textmode = NOBGR0;
	
	sendFontMode(textmode, textbg);
}

//sets cursor
void OCMA::setCursor(uint16_t x, uint16_t y){
	cursor_x=x;
	cursor_y=y;
}

//prints char/string and advances the cursor
void OCMA::print(char *c){
	uint16_t l=strlen(c);
	sendString(cursor_x, cursor_y, c);
	advanceCursor(l+1);
}

//prints single character
void OCMA::print(char c){
	char str[2]={c, '\0'};
	sendString(cursor_x, cursor_y, str);
	advanceCursor(2);
}

//prints uint16_t number and advances the cursor
void OCMA::print(uint16_t x){
	char buf[8*sizeof(uint16_t)+1];
	char *str=&buf[sizeof(buf)-1];
	
	*str='\0';
	do{
		uint16_t m = x;
		x /= 10;
		char c = m - 10*x;
		*--str = c < 10 ? c + '0' : c + 'A' - 10;
	}while(x);
	sendString(cursor_x, cursor_y, str);
	advanceCursor(strlen(str)+1);
}

//prints uint32_t number and advances the cursor
void OCMA::print(uint32_t x){
	char buf[8*sizeof(uint32_t)+1];
	char *str=&buf[sizeof(buf)-1];
	
	*str='\0';
	do{
		uint32_t m = x;
		x /= 10;
		char c = m - 10*x;
		*--str = c < 10 ? c + '0' : c + 'A' - 10;
	}while(x);
	sendString(cursor_x, cursor_y, str);
	advanceCursor(strlen(str)+1);
}

//prints int number and advances the cursor
void OCMA::print(int x){
	char buf[8*sizeof(int)+1];
	char *str=&buf[sizeof(buf)-1];
	bool neg = false;
	
	if(x<0){
		neg = true;
		x=-x;
	}
	
	*str='\0';
	do{
		uint16_t m = x;
		x /= 10;
		char c = m - 10*x;
		*--str = c < 10 ? c + '0' : c + 'A' - 10;
	}while(x);
	if(neg) *--str = '-';
	sendString(cursor_x, cursor_y, str);
	advanceCursor(strlen(str)+1);
}

//prints long number and advances the cursor
void OCMA::print(long x){
	char buf[8*sizeof(long)+1];
	char *str=&buf[sizeof(buf)-1];
	bool neg = false;
	
	if(x<0){
		neg = true;
		x=-x;
	}
	
	*str='\0';
	do{
		uint32_t m = x;
		x /= 10;
		char c = m - 10*x;
		*--str = c < 10 ? c + '0' : c + 'A' - 10;
	}while(x);
	if(neg) *--str = '-';
	sendString(cursor_x, cursor_y, str);
	advanceCursor(strlen(str)+1);
}

//prints double number, with 2 digits
void OCMA::print(double x){
  if(isnan(x)) {print("NAN"); advanceCursor(3);}
  if(isinf(x)) {print("INF"); advanceCursor(3);}
  if(x > 4294967040.0) {print("OVF"); advanceCursor(3);}
  if(x < -4294967040.0) {print("OVF"); advanceCursor(3);}
  bool neg=false;
  
   if(x < 0.0){
    neg=true;
    x=-x;
   }
   
	double rounding = 0.5;
	for(uint8_t i=0; i<2; i++) rounding/=10.0;
	
	x+=rounding;
	
	unsigned long int_part = (unsigned long) x;
	long remainder = (x-(double)int_part)*pow(10,2);
	
	char buf[8*sizeof(double)+3];
	char *str=&buf[sizeof(buf)-1];
	
	*str='\0';
	do{
		unsigned long m=remainder;
		remainder/=10;
		char c=m-10*remainder;
		*--str = c < 10 ? c + '0' : c + 'A' - 10;
	}while(remainder);
	*--str='.';
	do{
		unsigned long m = int_part;
		int_part /= 10;
		char c = m - 10 * int_part;
		*--str = c < 10 ? c + '0' : c + 'A' - 10;
	}while(int_part);
	if(neg) *--str = '-';

	sendString(cursor_x, cursor_y, str);
	advanceCursor(strlen(str)+1);
}

//prints double number, with variable digits
void OCMA::print(double x, uint8_t digits){
  if(isnan(x)) {print("NAN"); advanceCursor(3);}
  if(isinf(x)) {print("INF"); advanceCursor(3);}
  if(x > 4294967040.0) {print("OVF"); advanceCursor(3);}
  if(x < -4294967040.0) {print("OVF"); advanceCursor(3);}
  bool neg=false;
  
   if(x < 0.0){
    neg=true;
    x=-x;
   }
	double rounding = 0.5;
	for(uint8_t i=0; i<digits; i++) rounding/=10.0;
	
	x+=rounding;
	
	unsigned long int_part = (unsigned long) x;
	long remainder = (x-(double)int_part)*pow(10,digits);
	
	char buf[8*sizeof(double)+3];
	char *str=&buf[sizeof(buf)-1];
	
	*str='\0';
	do{
		unsigned long m=remainder;
		remainder/=10;
		char c=m-10*remainder;
		*--str = c < 10 ? c + '0' : c + 'A' - 10;
	}while(remainder);
	*--str='.';
	do{
		unsigned long m = int_part;
		int_part /= 10;
		char c = m - 10 * int_part;
		*--str = c < 10 ? c + '0' : c + 'A' - 10;
	}while(int_part);
	if(neg) *--str = '-';

	sendString(cursor_x, cursor_y, str);
	advanceCursor(strlen(str)+1);
}

//draws rectangle frame on screen
void OCMA::drawRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2){
	sendRect(0x00, x1, y1, x2, y2);
}

//fils rectanghle on screen
void OCMA::fillRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2){
	sendRect(0x01, x1, y1, x2, y2);
}

//draws circle on screen
void OCMA::drawCircle(uint16_t x, uint16_t y, uint8_t r){
	sendCircle(0x00, x, y, r);
}

//fills circle on screen
void OCMA::fillCircle(uint16_t x, uint16_t y, uint8_t r){
	sendCircle(0x01, x, y, r);
}

//draws a triangle on screen
void OCMA::drawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3){
	sendTriangle(0x00, x1, y1, x2, y2, x3, y3);
}

//fills triangle on screen
void OCMA::fillTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3){
	sendTriangle(0x01, x1, y1, x2, y2, x3, y3);
}

//draws elipse shape
void OCMA::drawElipse(uint16_t x, uint16_t y, uint16_t r1, uint16_t r2){
	sendElipse(0x00, x, y, r1, r2);
}

//fills elipse
void OCMA::fillElipse(uint16_t x, uint16_t y, uint16_t r1, uint16_t r2){
	sendElipse(0x01, x, y, r1, r2);
}

//draws a semi elipse
void OCMA::drawSemiElipse(uint8_t m, uint16_t x, uint16_t y, uint16_t r1, uint16_t r2){
	uint8_t mode;
	if(m == 2) mode = B00000010;		//upper right
	else if(m == 3) mode = B00000011;	//lower right
	else if(m == 4) mode = B00000000;	//lower left
	else mode = B00000001;				//upper left
	sendElipseCut(mode, x, y, r1, r2);
}

//fills a semi elipse
void OCMA::fillSemiElipse(uint8_t m, uint16_t x, uint16_t y, uint16_t r1, uint16_t r2){
	uint8_t mode;
	if(m == 2) mode = B10000010;		//upper right
	else if(m == 3) mode = B10000011;	//lower right
	else if(m == 4) mode = B10000000;	//lower left
	else mode = B10000001;				//upper left
	sendElipseCut(mode, x, y, r1, r2);
}

//draws round rectangle with corner elipse of radius r1, r2
void OCMA::drawRoundRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t r1, uint16_t r2){
	sendRoundRect(0x00, x1, y1, x2, y2, r1, r2);
}

//fills round rectangle with corner elipse of radius r1, r2
void OCMA::fillRoundRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t r1, uint16_t r2){
	sendRoundRect(0x01, x1, y1, x2, y2, r1, r2);
}

//sets touch panel functioning mode
void OCMA::touchMode(uint8_t m){
	if(m==2) {tpmode = m; sendTPM(TPM1);}		//coordonates are uploaded automaticaly as soon as touch panel is presed
	else if(m==3) {tpmode = m; sendTPM(TPM2);}	//coordonates are uploaded automaticaly on touch panel release
	else if(m==4) {tpmode = m; sendTPM(TPM3);}	//coordonates uploaded automaticaly every 100ms when pressed
	else if(m==5) {tpmode = m; sendTPM(TPM4);}	//coordonate uploaded automaticaly when touch panel is pressed or released
	else {tpmode = 1; sendTPM(TPM0);}			//default mode; TP status uploaded manualy by touchRead(); (can use interrupt on pin 5 of display)
}

//read touch panel status (deppending on touchMode, it does in two ways); (if not not tched will read the pannel 3 times to make sure)
void OCMA::touchRead(void){
	if(tpmode!=1){										//custom touch panel mode
		touched=false;
		released=false;
		bool ok=true;
		int i=0;
		uint16_t x, y;
		while(read() != 0xfd){
			if(i>200){
				ok=false;
				break;
			}
			i++;
		}
		if(ok){
			uint8_t type=read();
			x=readInt();
			y=readInt();
			if(read()==0xdd)
				if(read()==0xcc)
					if(read()==0xbb)
						if(read()==0xaa){
							if(type==0x01){
								tpPx=x;
								tpPy=y;
								tpPms=millis();
								touched=true;
							}
							if(type==0x03){
								tpRx=x;
								tpRy=y;
								tpRms=millis();
								released=true;
							}
						}
		}
	}
	else{												//default touchpanel mode
		int i=0;
		while(!touchRD()){
			if(i>2) break;
			i++;
		}
	}
}

//touch read helper function (for reliablility) - returns 1 if touched
bool OCMA::touchRD(void){
	while(Serial.available()) read();
	sendTPRead();
	Serial.flush();
	delay(4);
	uint8_t tchd = read();
	if(0xfa == tchd){
		touched = true;
		tpx = readInt();
		tpy = readInt();
		return 1;
	}
	else{
		touched = false;
		return 0;
	}
}

//returns 1 if panel was touched on last read
bool OCMA::isTouched(void){
	return touched;
}

//returns 1 if panel was released on last read
bool OCMA::isReleased(void){
	return released;
}

//returns last confirmed touch read command in ms
uint32_t OCMA::lastTouched(void){
	return tpPms;
}

//returns last confirmed release read command in ms
uint32_t OCMA::lastReleased(void){
	return tpRms;
}

//returns last touch panel x coordonates
uint16_t OCMA::touchX(void){
	if(tpmode == 1)	return tpx;
	else if(tpmode == 2 || tpmode == 4) return tpPx;
	else if(tpmode == 3) return tpRx;
	else if(tpmode == 4) return (tpPms > tpRms ? tpPx : tpRx);
	else return tpx;
}

//returns last touch panel y coordonates
uint16_t OCMA::touchY(void){
	if(tpmode == 1)	return tpy;
	else if(tpmode == 2 || tpmode == 4) return tpPy;
	else if(tpmode == 3) return tpRy;
	else if(tpmode == 4) return (tpPms > tpRms ? tpPy : tpRy);
	else return tpy;
}

//returns last touch X coordonate (for other than touchmode 1)
uint16_t OCMA::touchPX(void){
	return tpPx;
}

//returns last touch Y coordonate (for other than mode 1)
uint16_t OCMA::touchPY(void){
	return tpPy;
}

//returns last release X coordonate (for other than touch mode 1)
uint16_t OCMA::touchRX(void){
	return tpRx;
}

//returns last releas Y coordonate (for other than touch mode 1)
uint16_t OCMA::touchRY(void){
	return tpRy;
}


//sets write layer - layer 1 or layer 2
void OCMA::setWriteLayer(uint8_t layer){
	if(layer == 2) wlayer = 2;
	else wlayer = 1;
	sendWriteLayer(wlayer);
}

//sets displayed layer
void OCMA::setDisplayLayer(uint8_t layer){
	if(layer == 2){
		dlayer = 2;
		sendLayerMode1(OCMA_LAYER2);
	}
	else{
		dlayer = 1;
		sendLayerMode1(OCMA_LAYER1);
	}
}

//sets opacity of foreground layer = 88% ; 2 = 75% ; 3 = 63% ; 4 = 50%
void OCMA::setLayerTransparency(uint8_t transp){
	if(dlayer == 1){
		if(transp == 1) sendLayerMode1(LT78);
		else if(transp == 2) sendLayerMode1(LT68);
		else if(transp == 3) sendLayerMode1(LT58);
		else if(transp == 4) sendLayerMode1(LT48);
		else sendLayerMode1(OCMA_LAYER1);
	}
	else{
		if(transp == 1) sendLayerMode1(LT18);
		else if(transp == 2) sendLayerMode1(LT28);
		else if(transp == 3) sendLayerMode1(LT38);
		else if(transp == 4) sendLayerMode1(LT48);
		else sendLayerMode1(OCMA_LAYER2);
	}
}

//sets layer display to superimposed mode with filter color
void OCMA::setFilteredDisplayLayer(uint16_t color){
	sendLayerMode2(color);
}

//fades between layers with delay del in ms
void OCMA::fadeLayers(uint16_t del){
	for(int i=0; i<=4; i++) {
		setLayerTransparency(i);
		delay(del);
	}
	if(dlayer==1) dlayer=2;
	else dlayer=1;
	for(int i=3; i>=0; i--){
		setLayerTransparency(i);
		delay(del);
	}
}

//draws image to screen
void OCMA::drawImg(uint16_t id){
	if(id>106) id=106;
	sendDrawIMG(id);
}

//save the display contents to internal picture memory at location id
void OCMA::saveScreen(uint16_t id){
	if(id>106) id=106;
	int i=0;
	while(!saveImg(id)){
		i++;
		if(i>4) break;
	}
}

//sets display brightness - 0 backlight off ; 255 - backlight 100%
void OCMA::brightness(uint8_t b){
	if(b == 0) backlightOFF();
	else if(b < 255) {
		sendBrightness(b);
		backlightON();
	}
	else {
		sendBrightness(255);
		backlightON();
	}
}

//sets scan mode
void OCMA::setScanMode(uint8_t scan){
	if(scan == 2) sendScanMode(TBRL);		//TOP -> BOTTOM & RIGHT -> LEFT
	else if(scan == 3) sendScanMode(BTLR);	//BOTTOM -> TOP & LEFT -> RIGHT
	else if(scan == 4) sendScanMode(BTRL);	//BOTTOM -> TOP & RIGHT -> LEFT
	else sendScanMode(TBLR);				//TOP -> BOTTOM & LEFT -> RIGHT (default)
}

//returns baudrate
uint32_t OCMA::showBaud(void){
	return flagToBaud(findBaud());
}


/*************************************************************************
							Medium level commands
*************************************************************************/

//sends handshake command; returns BAUD
uint8_t OCMA::Handshake(void){
	while(Serial.available()) Serial.read();
	
	cmdStart();
	send(OCMA_HandShakeFlag);
	cmdSize(0);
	cmdEnd();
	
	Serial.flush();
	
	uint8_t baud;
	read();
	baud = read();
	read();
	return baud;
}

//sends baud rate change command
void OCMA::sendBaudRate(uint8_t baud){
	cmdStart();
	send(OCMA_BaudRateFlag);
	cmdSize(5);
	send(0x55);
	send(0xaa);
	send(0x5a);
	send(0xa5);
	send(baud);
	cmdEnd();
	
	Serial.flush();
}

//sends color command (foreground and backgrond)
void OCMA::sendColor(uint16_t fg, uint16_t bg){
	cmdStart();
	send(OCMA_SetColorFlag);
	cmdSize(4);
	sendInt(fg);
	sendInt(bg);
	cmdEnd();
}


//sends font mode
void OCMA::sendFontMode(uint8_t m, uint16_t color){
	cmdStart();
	send(OCMA_FontModeFlag);
	cmdSize(3);
	send(m);
	sendInt(color);
	cmdEnd();
	
}

//sends command for layer mode 1 with transparency
void OCMA::sendLayerMode1(uint8_t t){
	cmdStart();
	send(OCMA_LayerDrawFlag);
	cmdSize(2);
	send(DM1);
	send(t);
	cmdEnd();
	
}

//sends command for layer mode 2 with filter color
void OCMA::sendLayerMode2(uint16_t color){
	cmdStart();
	send(OCMA_LayerDrawFlag);
	cmdSize(3);
	send(DM2);
	sendInt(color);
	cmdEnd();
	
}

//save screen to IMG storage with id, returns 1 if successfull
bool OCMA::saveImg(uint16_t id){
	while(Serial.available()) Serial.read();	//flushes arduino serial buffer
	
	cmdStart();
	send(OCMA_SaveScreenFlag);
	cmdSize(2);
	sendInt(id);
	cmdEnd();
	Serial.flush();
	delay(2000);				// for safety
	uint32_t i=millis();
	while(!Serial.available()){
		if(i+10000 > millis()) break;
	}
	return (id == readInt());
}

//draws an image on screen from memory
void OCMA::sendDrawIMG(uint16_t id){
	cmdStart();
	send(OCMA_DrawFullScreenPicFlag);
	cmdSize(2);
	sendInt(id);
	cmdEnd();
}

//print char/string
void OCMA::sendString(uint16_t x, uint16_t y, char *c){
	uint16_t length;
	length=strlen(c);
	length=length+5;
	
	cmdStart();
	send(textsize);
	cmdSize(length);
	sendInt(x);
	sendInt(y);
	do{
		send(*c);
	}while(*c++);
	cmdEnd();
}

//clears the screen
void OCMA::clearScreen(void){
	cmdStart();
	send(OCMA_ClearScreenFlag);
	cmdSize(0);
	cmdEnd();
}

//draws a pixel on screen
void OCMA::drawPixel(uint16_t x, uint16_t y){
	cmdStart();
	send(OCMA_DrawPixelsFlag);
	cmdSize(4);
	sendInt(x);
	sendInt(y);
	cmdEnd();
}

//draws n pixels on screen (maximum 500 at a time)
void OCMA::drawPixels(uint16_t *x, uint16_t *y, uint16_t n){
	cmdStart();
	send(OCMA_DrawPixelsFlag);
	cmdSize(n <= 500 ? n*4 : 2000);
	for(;n>0;n--){
		sendInt(*x++);
		sendInt(*y++);
	}
	cmdEnd();
}

//draws a line on screen
void OCMA::drawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2){
	cmdStart();
	send(OCMA_DrawLineFlag);
	cmdSize(8);
	sendInt(x1);
	sendInt(y1);
	sendInt(x2);
	sendInt(y2);
	cmdEnd();
}

//draws n lines on screen (maximum 250 lines)
void OCMA::drawLines(uint16_t *x, uint16_t *y, uint16_t n){
	cmdStart();
	send(OCMA_DrawLineFlag);
	cmdSize(n <= 250 ? n*8 : 2000);
	for(; n>0; n--){
		sendInt(*x++);
		sendInt(*y++);
		sendInt(*x++);
		sendInt(*y++);
	}
	cmdEnd();	
}

//draws a polygon chain on screen with n segments (maximum 250 lines)
void OCMA::drawPoligon(uint16_t *x, uint16_t *y, uint16_t n){
	cmdStart();
	send(OCMA_DrawLineFlag);
	cmdSize(n <= 250 ? n*8 : 2000);
	for(;n>0;n--){
		sendInt(*x);
		sendInt(*y);
		sendInt(*++x);
		sendInt(*++y);
	}
	cmdEnd();
}

//send rectangular shape to be drawn
void OCMA::sendRect(uint8_t fill, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2){
	cmdStart();
	send(OCMA_DrawRectFlag);
	cmdSize(9);
	send(fill);
	sendInt(x1);
	sendInt(y1);
	sendInt(x2);
	sendInt(y2);
	cmdEnd();
}

//send circular shape to be drawn
void OCMA::sendCircle(uint8_t fill, uint16_t x, uint16_t y, uint8_t r){
	cmdStart();
	send(OCMA_DrawCircleFlag);
	cmdSize(0x06);
	send(fill);
	sendInt(x);
	sendInt(y);
	send(r);
	cmdEnd();
}

//send triangle shape to be drawn
void OCMA::sendTriangle(uint8_t fill, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3){
	cmdStart();
	send(OCMA_DrawTriangleFlag);
	cmdSize(13);
	send(fill);
	sendInt(x1);
	sendInt(y1);
	sendInt(x2);
	sendInt(y2);
	sendInt(x3);
	sendInt(y3);
	cmdEnd();
}


//send elipse shape to be drawn
void OCMA::sendElipse(uint8_t fill, uint16_t x, uint16_t y, uint16_t r1, uint16_t r2){
	cmdStart();
	send(OCMA_DrawElipseFlag);
	cmdSize(9);
	send(fill);
	sendInt(x);
	sendInt(y);
	sendInt(r1);
	sendInt(r2);
	cmdEnd();
}

//send semi elipse to be drawn
void OCMA::sendElipseCut(uint8_t m, uint16_t x, uint16_t y, uint16_t r1, uint16_t r2){
	cmdStart();
	send(OCMA_DrawSemiElipseFlag);
	cmdSize(9);
	send(m);
	sendInt(x);
	sendInt(y);
	sendInt(r1);
	sendInt(r2);
	cmdEnd();
}

//sends round rectangle to be drawn
void OCMA::sendRoundRect(uint8_t fill, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t r1, uint16_t r2){
	cmdStart();
	send(OCMA_DrawRoundRectFlag);
	cmdSize(13);
	send(fill);
	sendInt(x1);
	sendInt(x2);
	sendInt(y1);
	sendInt(y2);
	sendInt(r1);
	sendInt(r2);
	cmdEnd();
}

//enters touch panel calibration
void OCMA::calibrateTouchPanel(void){
	cmdStart();
	send(OCMA_TouchPanelCalibrateFlag);
	cmdSize(4);
	send(0x55);
	send(0xaa);
	send(0x5a);
	send(0xa5);
	cmdEnd();
}

//sends touch panel mode
void OCMA::sendTPM(uint8_t tpm){
	cmdStart();
	send(OCMA_TouchPanelModeFlag);
	cmdSize(1);
	send(tpm);
	cmdEnd();
}

//sends touch panel upload status command
void OCMA::sendTPRead(void){
	cmdStart();
	send(OCMA_TouchFeedbackFlag);
	cmdSize(1);
	send(0x01);
	cmdEnd();
}

//sends write layer command
void OCMA::sendWriteLayer(uint8_t layer){
	cmdStart();
	send(OCMA_WriteLayerSelectFlag);
	cmdSize(1);
	send(layer);
	cmdEnd();
}

//display a cropped image from storage id, on screen location x y (ix, iy - image top left start cropping, w, h - image cropped size in pixels)
void OCMA::drawCroppedImage(uint16_t x, uint16_t y, uint16_t id, uint16_t ix, uint16_t iy, uint16_t w, uint16_t h){
	cmdStart();
	send(OCMA_DrawCroppedPicFlag);
	cmdSize(14);
	sendInt(x);
	sendInt(y);
	sendInt(id);
	sendInt(ix);
	sendInt(iy);
	sendInt(w);
	sendInt(h);
	send(0x00);	//reserved mask;
	cmdEnd();
}

//truns off backlight
void OCMA::backlightOFF(void){
	cmdStart();
	send(OCMA_BacklightOffFlag);
	cmdSize(0);
	cmdEnd();	
}

//turns on backlight to full brightness
void OCMA::backlightON(void){
	cmdStart();
	send(OCMA_BacklightOnFlag);
	cmdSize(0);
	cmdEnd();
}

//sets backlight brightness
void OCMA::sendBrightness(uint8_t b){
	cmdStart();
	send(OCMA_BacklightLevelFlag);
	cmdSize(1);
	send(b);
	cmdEnd();
}

//sends set scan mode command
void OCMA::sendScanMode(uint8_t scan){
	cmdStart();
	send(OCMA_ScanModeFlag);
	cmdSize(1);
	send(scan);
	cmdEnd();
}

/*************************************************************************
							Low level commands
*************************************************************************/

//send byte
void OCMA::send(uint8_t c){
	Serial.write(c);
}

//send cmd start flag
void OCMA::cmdStart(void){
	send(0xfd);
}

//send cmd end flags
void OCMA::cmdEnd(void){
	send(0xdd);
	send(0xcc);
	send(0xbb);
	send(0xaa);
}

//send cmd size (two bites)
void OCMA::cmdSize(uint16_t s){
	send((s>>8)&0xff);
	send(s&0xff);
}

//send int (two bytes)
void OCMA::sendInt(uint16_t i){
	send((i>>8)&0xff);
	send(i&0xff);
}

//read (one byte)
uint8_t OCMA::read(void){
	int i=0;
	while(!Serial.available()){
		i++;
		delay(1);
		if(i>100) break;
	}
	delay(10);
	uint8_t a = Serial.read();
	return a;
}

//read (two bytes)
uint16_t OCMA::readInt(void){
		uint16_t b,c;
		int i=0;
		while(!Serial.available()){
			i++;
			delay(1);
			if(i>100) break;
		}
		delay(10);
		b=Serial.read();
		c=Serial.read();
		b = ((b<<8)&0xff00) | (c & 0xff); 
	return b;
}

/*************************************************************************
							Helper functions
*************************************************************************/

//convert baud flag to baud rate
uint32_t OCMA::flagToBaud(uint8_t baud){
	if(baud == Bd1200) return 1200;
	else if(baud == Bd2400) return 2400;
	else if (baud == Bd4800) return 4800;
	else if (baud == Bd9600) return 9600;
	else if (baud == Bd14400) return 14400;
	else if (baud == Bd19200) return 19200;
	else if (baud == Bd38400) return 38400;
	else if (baud == Bd56000) return 56000;
	else if (baud == Bd57600) return 57600;
	else if (baud == Bd115200) return 115200;
	//else if (baud == Bd230400) return 230400;
	//else if (baud == Bd460800) return 460800;
	//else if (baud == Bd921600) return 921600;
	else return 19200;
}

//convert baud rate to baud flag
uint8_t OCMA::baudToFlag(uint32_t baud){
	if(baud == 1200) return Bd1200;
	else if(baud == 2400) return Bd2400;
	else if (baud == 4800) return Bd4800;
	else if (baud == 9600) return Bd9600;
	else if (baud == 14400) return Bd14400;
	else if (baud == 19200) return Bd19200;
	else if (baud == 38400) return Bd38400;
	else if (baud == 56000) return Bd56000;
	else if (baud == 57600) return Bd57600;
	else if (baud == 115200) return Bd115200;
	//else if (baud == 230400) return Bd230400;
	//else if (baud == 460800) return Bd460800;
	//else if (baud == 921600) return Bd921600;
	else return Bd19200;
	
}

//advances the cursor after print
void OCMA::advanceCursor(uint16_t l){
	int size = 8;
	if(textsize == OCMA_CharSize10Flag) size=6;
	else if(textsize == OCMA_CharSize24Flag) size=12;
	else if(textsize == OCMA_CharSize32Flag) size=16;
	l*=size;
	while(319-cursor_x - size < l){
		l-=(319-cursor_x)-((319-cursor_x)%size);
		cursor_x=0;
		cursor_y+=size*2 - (size == 6 ? 2 : 0);
		if((cursor_y>239-size*2) && (319-cursor_x < l)) return;
	}
	cursor_x += l-size;
}

//transforms r,g,b color to display color (0 <= r <= 31 ; 0<=g<=63 ; 0<= b <= 31
uint16_t OCMA::color(uint8_t r, uint8_t g, uint8_t b){
	return (((r<31 ? r : 31) & 0x1f)<<11) | (((g<63 ? g : 63) & 0x3f) << 5) | ((b<31 ? b : 31)&0x1f);
}