/********************************************
 *      This is example for OCMA library    *
 ********************************************/


#include <OCMA.h>

// define some standard colors; not used in this example
#define RED 0xf800
#define GREEN 0x07e0
#define BLUE 0x001f
#define WHITE 0xffff
#define BLACK 0x0000
#define YELLOW 0xffe0
#define ORANGE 0xfc08
#define PINK 0xf810

//constructor

OCMA tft;

void menu1(void){
  tft.clearScreen();
  
  tft.stroke(tft.color(0, 0, 31));                 //sets stroke color to blue
  tft.fillRoundRect(100, 10, 220, 40, 5, 5);
  tft.fillRoundRect(100, 70, 220, 100, 5, 5);
  tft.fillRoundRect(100, 130, 220, 160, 5, 5);
  tft.fillRoundRect(100, 190, 220, 220, 5, 5);

  tft.stroke(31, 63, 41);             //sets stroke color to white; you can use direct stroke(r, g, b);
  tft.textSize(2);
  tft.setCursor(150, 15);
  tft.print("Draw");
  tft.setCursor(130, 75);
  tft.print("Position");
  tft.setCursor(140, 135);
  tft.print("Images");
  tft.setCursor(140, 195);
  tft.print("Rainbow");

  tft.touchMode(1);   //touchmode 1 (feedback sent only when called)
  delay(500);         //delay to avoid accidental touch read when returning from rainbow()
  while(1){                                                           //touch feedback loop
    tft.touchRead();                                                  //reads the current touch situation
    if(tft.isTouched() && tft.touchX()>=100 && tft.touchX() <=220){       //if touched and x position inbetween 100 and 220
      if(tft.touchY() >= 10 && tft.touchY() <= 40) draw();                //if Draw button touched, go to draw
      else if(tft.touchY() >= 60 && tft.touchY() <= 100) t_position();   //if Position button touched, go to position
      else if(tft.touchY() >= 130 && tft.touchY() <= 150) images();       //if Images button touched, go to images
      else if(tft.touchY() > 180) rainbow();
    }
  }
}


void draw_menu(void){                                   //menu for draw app
  tft.stroke(tft.color(0, 0, 0));
  tft.fillRect(0, 0, 59, 239);
  
  tft.stroke(tft.color(31, 63, 31));
  tft.fillRoundRect(20, 10, 50, 28, 2, 2);
  tft.stroke(tft.color(31, 0, 0));
  tft.fillRoundRect(20, 100, 50, 118, 2, 2);
  tft.stroke(tft.color(0, 63, 0));
  tft.fillRoundRect(20, 210, 50, 228, 2, 2);
  
  tft.stroke(tft.color(31, 63, 31));
  tft.drawLine(59, 0, 59, 239);

  tft.stroke(tft.color(0, 0, 0));
  tft.textSize(1);
  tft.setCursor(20, 11);
  tft.print("Color");
  tft.setCursor(20, 101);
  tft.print("Clear");
  tft.setCursor(20, 211);
  tft.print("Back");
}
void draw(void){                      //draw app
  tft.clearScreen();
  tft.touchMode(1);

  draw_menu();
  
  tft.stroke(tft.color(31, 63, 31));

 
  while(1){
    tft.touchRead();
    if(tft.isTouched()){
          if(tft.touchX() >= 0 && tft.touchX() <= 60){
            if(tft.touchY() >= 15 && tft.touchY() <= 32) colorchoose();
            else if(tft.touchY() >= 95 && tft.touchY() <= 132){               //clear drawing
              tft.stroke(tft.color(0, 0, 0));
              tft.fillRect(60, 0, 319, 239);
              tft.stroke(tft.color(31, 63, 31));
            }
            else if(tft.touchY() >= 195 && tft.touchY() <= 239) menu1();
          }
          else tft.fillCircle(tft.touchX(), tft.touchY(), 2);
          }
    }
}



void colorchoose(void){               //choose color for draw app
  delay(100);
  tft.stroke(tft.color(31, 0, 0));
  tft.fillRect(0, 0, 58, 80);
  tft.stroke(tft.color(0, 63, 0));
  tft.fillRect(0, 81, 58, 160);
  tft.stroke(tft.color(0, 0, 31));
  tft.fillRect(0, 161, 58, 239);

  while(1){
    tft.touchRead();
    if(tft.isTouched()){
      if(tft.touchX() < 59){
        if(tft.touchY() <= 80){
          draw_menu();
          tft.stroke(tft.color(31, 0, 0)); 
          break;
        }
        else if(tft.touchY() <= 160){
          draw_menu();
          tft.stroke(tft.color(0, 63, 0));
          break;
        }
        else {
          draw_menu();
          tft.stroke(tft.color(0, 0, 31));
          break;
        }
      }
      else{
        draw_menu();
        tft.stroke(tft.color(random()%31, random()%63, random()%31));
        break;
      }
    }
  }
}

void t_position(void){
  tft.clearScreen();
  tft.touchMode(5);
  tft.textSize(2);

  while(1){
    tft.touchRead();
    tft.clearScreen();
    tft.stroke(tft.color(0,0,31));
    tft.fillRect(0, 200, 319, 239);
    tft.stroke(tft.color(31,63,31));
    tft.setCursor(150, 210);
    tft.print("BACK");

    tft.setCursor(0, 0);
    tft.print("Touched at ");
    tft.print(tft.touchPX());
    tft.print(", ");
    tft.print(tft.touchPY());
    tft.setCursor(0, 16);
    tft.print((double)(millis() - tft.lastTouched())/1000);
    tft.print(" s ago");

    tft.setCursor(0, 50);
    tft.print("Released at ");
    tft.print(tft.touchRX());
    tft.print(", ");
    tft.print(tft.touchRY());
    tft.setCursor(0, 66);
    tft.print((double)(millis() - tft.lastReleased())/1000);
    tft.print(" s ago");

    tft.setCursor(60, 150);
    tft.print("Touchpanel is ");
    if(tft.isTouched())  tft.print("touched.");
    else tft.print("released.");

    if(tft.touchPY() > 199) menu1();
  }
}

void images(void){
  int id=0;

  tft.clearScreen();
  tft.touchMode(1);

  draw_img(id);
  while(1){
    
    tft.touchRead();
    if(tft.isTouched()){
      if(tft.touchY()>220 && tft.touchX() > 145 && tft.touchX() < 175) menu1();
      else if(tft.touchX()<126) draw_img(--id);
      else if(tft.touchX()>194) draw_img(++id);
      delay(100);
    }
    if(id>106) id = 0;
    if(id < 0) id = 106;
  }
}

void draw_img(int id){
  if(id>106) id = 0;
  else if(id<0) id = 106;
  tft.drawImg(id);
  tft.stroke(tft.color(31, 0, 31));
  tft.setCursor(120, 0);
  tft.textSize(2);
  tft.print("Image ID: ");
  tft.print(id);
  tft.stroke(tft.color(0, 0, 0));
  tft.fillRect(5, 110, 25, 130);
  tft.fillRect(294, 110, 314, 130);
  tft.stroke(tft.color(0, 0, 31));
  tft.fillTriangle(6, 120, 23, 111, 23, 129);
  tft.fillTriangle(313, 120, 296, 111, 296, 129);

  tft.stroke(0, 0, 31);
  tft.fillRect(145, 220, 175, 239);
  tft.stroke(tft.color(31, 0, 0));
  tft.textSize(1);
  tft.setCursor(150, 225);
  tft.print("BACK");
}

void rainbow(void){
  tft.clearScreen();
  uint8_t r=31, g=0, b=0;
  uint16_t x=0;

  do {
    g++;
    tft.stroke(tft.color(r, g, b));
    tft.drawLine(0,x,319,x);
    x++;
   }while(g<63);
   do{
    r--;
    tft.stroke(tft.color(r, g, b));
    tft.drawLine(0,x,319,x);
    x++;    
   }while(r>0);
   do{
    b++;
    tft.stroke(tft.color(r, g, b));
    tft.drawLine(0,x,319,x);
    x++;
   }while(b<31);
   do{
    g--;
    tft.stroke(tft.color(r, g, b));
    tft.drawLine(0,x,319,x);
    x++;
   }while(g>0);
   do{
    r++;
    tft.stroke(tft.color(r, g, b));
    tft.drawLine(0,x,319,x);
    x++;
   }while(r<31);

   tft.stroke(0, 0, 31);
   tft.fillRect(0, x, 319, 239);
   tft.stroke(31, 63, 31);
   tft.setCursor(150, x+3);
   tft.print("BACK");

   while(1){
    tft.touchRead();
    if(tft.isTouched() && tft.touchY()> x - 20) menu1();
   }
}

void setup() {
  delay(5000);        //delay 5 seconds to connect RX wire.
  
  tft.init(115200);   //synchronises to the tft screen at a baud rate of 115200 and clears the screen

  menu1();            //displays main menu

}

void loop() {

}
