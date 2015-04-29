/*
************************************************************************
 ECE 362 - Mini-Project C Source File - Spring 2015
***********************************************************************
	 	   			 		  			 		  		
 Team ID: 11

 Project Name: Star Treker

 Team Members:

   - Team/Doc Leader: < ? >      Signature: Allison Gruninger________
   - Software Leader: < ? >      Signature: Alexander Marcellus______
   - Interface Leader: < ? >     Signature: Kyle Rakos / Nicole laGue
   - Peripheral Leader: < ? >    Signature: Christopher Chow_________

 Academic Honesty Statement:  In signing above, we hereby certify that we 
 are the individuals who created this HC(S)12 source file and that we have
 not copied the work of any other student (past or present) while completing 
 it. We understand that if we fail to honor this agreement, we will receive 
 a grade of ZERO and be subject to possible disciplinary action.

***********************************************************************

 The objective of this board is to allow for user input for transmission
 to the second board. A keypad via the AN1-7 pins are interfaced and used
 as digital inputs. A custom Nokia 48x84 LCD display is used via the SPI
 interface, so that it is not necessary to use a shift register to send
 commands to the LCD. The TX/RX pins are connected to a RS232->5V level
 translator, then a 5V->3V level translator, then to the XBee wireless
 communicator that will communicate with another XBee on the other board,
 sending the following data:
   Latitude, Longitude
   Year, Month, Day
   Hour, Minute
   Star Index
   
***********************************************************************

 List of project-specific success criteria (functionality that will be
 demonstrated):

 1.

 2.

 3.

 4.

 5.

***********************************************************************

  Date code started: 4/24

  Update history (add an entry every time a significant change is made):

  Date: 4/25   Name: Christopher Chow   Update: Created: LCD, Keypad code
  Date: 4/26   Name: Christopher Chow   Update: Merged RPG code from Alex
  Date: 4/26   Name: Christopher Chow   Update: Added input prompts
  Date: 4/27   Name: Christopher Chow   Update: Error trapping for input prompts
  Date: 4/27   Name: Nicole LaGue       Update: Merged XBee code
  Date: 4/29   Name: Christopher Chow   Update: more Error Trapping

***********************************************************************
*/

#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include <mc9s12c32.h>
//#include <float.h> 

/* DEBUG DECLARATION */
// for protoboard use only
#define DEBUG

// LCD DECLARATION
#define LCD_WIDTH 84
#define LCD_HEIGHT 48

static const byte ASCII[][5] = {
  // First 32 characters (0x00-0x19) are ignored. These are
  // non-displayable, control characters.
   {0x00, 0x00, 0x00, 0x00, 0x00} // 0x20  
  ,{0x00, 0x00, 0x5f, 0x00, 0x00} // 0x21 !
  ,{0x00, 0x07, 0x00, 0x07, 0x00} // 0x22 "
  ,{0x14, 0x7f, 0x14, 0x7f, 0x14} // 0x23 #
  ,{0x24, 0x2a, 0x7f, 0x2a, 0x12} // 0x24 $
  ,{0x23, 0x13, 0x08, 0x64, 0x62} // 0x25 %
  ,{0x36, 0x49, 0x55, 0x22, 0x50} // 0x26 &
  ,{0x00, 0x05, 0x03, 0x00, 0x00} // 0x27 '
  ,{0x00, 0x1c, 0x22, 0x41, 0x00} // 0x28 (
  ,{0x00, 0x41, 0x22, 0x1c, 0x00} // 0x29 )
  ,{0x14, 0x08, 0x3e, 0x08, 0x14} // 0x2a *
  ,{0x08, 0x08, 0x3e, 0x08, 0x08} // 0x2b +
  ,{0x00, 0x50, 0x30, 0x00, 0x00} // 0x2c ,
  ,{0x08, 0x08, 0x08, 0x08, 0x08} // 0x2d -
  ,{0x00, 0x60, 0x60, 0x00, 0x00} // 0x2e .
  ,{0x20, 0x10, 0x08, 0x04, 0x02} // 0x2f /
  ,{0x3e, 0x51, 0x49, 0x45, 0x3e} // 0x30 0
  ,{0x00, 0x42, 0x7f, 0x40, 0x00} // 0x31 1
  ,{0x42, 0x61, 0x51, 0x49, 0x46} // 0x32 2
  ,{0x21, 0x41, 0x45, 0x4b, 0x31} // 0x33 3
  ,{0x18, 0x14, 0x12, 0x7f, 0x10} // 0x34 4
  ,{0x27, 0x45, 0x45, 0x45, 0x39} // 0x35 5
  ,{0x3c, 0x4a, 0x49, 0x49, 0x30} // 0x36 6
  ,{0x01, 0x71, 0x09, 0x05, 0x03} // 0x37 7
  ,{0x36, 0x49, 0x49, 0x49, 0x36} // 0x38 8
  ,{0x06, 0x49, 0x49, 0x29, 0x1e} // 0x39 9
  ,{0x00, 0x36, 0x36, 0x00, 0x00} // 0x3a :
  ,{0x00, 0x56, 0x36, 0x00, 0x00} // 0x3b ;
  ,{0x08, 0x14, 0x22, 0x41, 0x00} // 0x3c <
  ,{0x14, 0x14, 0x14, 0x14, 0x14} // 0x3d =
  ,{0x00, 0x41, 0x22, 0x14, 0x08} // 0x3e >
  //,{0x02, 0x01, 0x51, 0x09, 0x06} // 0x3f ?
  // Replace question mark with a checkmark
    ,{0b00110000, 0b01100000, 0b00110000, 0b00011000, 0b00001100}
  ,{0x32, 0x49, 0x79, 0x41, 0x3e} // 0x40 @
  ,{0x7e, 0x11, 0x11, 0x11, 0x7e} // 0x41 A
  ,{0x7f, 0x49, 0x49, 0x49, 0x36} // 0x42 B
  ,{0x3e, 0x41, 0x41, 0x41, 0x22} // 0x43 C
  ,{0x7f, 0x41, 0x41, 0x22, 0x1c} // 0x44 D
  ,{0x7f, 0x49, 0x49, 0x49, 0x41} // 0x45 E
  ,{0x7f, 0x09, 0x09, 0x09, 0x01} // 0x46 F
  ,{0x3e, 0x41, 0x49, 0x49, 0x7a} // 0x47 G
  ,{0x7f, 0x08, 0x08, 0x08, 0x7f} // 0x48 H
  ,{0x00, 0x41, 0x7f, 0x41, 0x00} // 0x49 I
  ,{0x20, 0x40, 0x41, 0x3f, 0x01} // 0x4a J
  ,{0x7f, 0x08, 0x14, 0x22, 0x41} // 0x4b K
  ,{0x7f, 0x40, 0x40, 0x40, 0x40} // 0x4c L
  ,{0x7f, 0x02, 0x0c, 0x02, 0x7f} // 0x4d M
  ,{0x7f, 0x04, 0x08, 0x10, 0x7f} // 0x4e N
  ,{0x3e, 0x41, 0x41, 0x41, 0x3e} // 0x4f O
  ,{0x7f, 0x09, 0x09, 0x09, 0x06} // 0x50 P
  ,{0x3e, 0x41, 0x51, 0x21, 0x5e} // 0x51 Q
  ,{0x7f, 0x09, 0x19, 0x29, 0x46} // 0x52 R
  ,{0x46, 0x49, 0x49, 0x49, 0x31} // 0x53 S
  ,{0x01, 0x01, 0x7f, 0x01, 0x01} // 0x54 T
  ,{0x3f, 0x40, 0x40, 0x40, 0x3f} // 0x55 U
  ,{0x1f, 0x20, 0x40, 0x20, 0x1f} // 0x56 V
  ,{0x3f, 0x40, 0x38, 0x40, 0x3f} // 0x57 W
  ,{0x63, 0x14, 0x08, 0x14, 0x63} // 0x58 X
  ,{0x07, 0x08, 0x70, 0x08, 0x07} // 0x59 Y
  ,{0x61, 0x51, 0x49, 0x45, 0x43} // 0x5a Z
  ,{0x00, 0x7f, 0x41, 0x41, 0x00} // 0x5b [
  ,{0x02, 0x04, 0x08, 0x10, 0x20} // 0x5c \
  ,{0x00, 0x41, 0x41, 0x7f, 0x00} // 0x5d ]
  ,{0x00, 0x41, 0x41, 0x7f, 0x00} // 0x5d ]
  ,{0x04, 0x02, 0x01, 0x02, 0x04} // 0x5e ^
  ,{0x40, 0x40, 0x40, 0x40, 0x40} // 0x5f _
  ,{0x00, 0x01, 0x02, 0x04, 0x00} // 0x60 `
  ,{0x20, 0x54, 0x54, 0x54, 0x78} // 0x61 a
  ,{0x7f, 0x48, 0x44, 0x44, 0x38} // 0x62 b
  ,{0x38, 0x44, 0x44, 0x44, 0x20} // 0x63 c
  ,{0x38, 0x44, 0x44, 0x48, 0x7f} // 0x64 d
  ,{0x38, 0x54, 0x54, 0x54, 0x18} // 0x65 e
  ,{0x08, 0x7e, 0x09, 0x01, 0x02} // 0x66 f
  ,{0x0c, 0x52, 0x52, 0x52, 0x3e} // 0x67 g
  ,{0x7f, 0x08, 0x04, 0x04, 0x78} // 0x68 h
  ,{0x00, 0x44, 0x7d, 0x40, 0x00} // 0x69 i
  ,{0x20, 0x40, 0x44, 0x3d, 0x00} // 0x6a j 
  ,{0x7f, 0x10, 0x28, 0x44, 0x00} // 0x6b k
  ,{0x00, 0x41, 0x7f, 0x40, 0x00} // 0x6c l
  ,{0x7c, 0x04, 0x18, 0x04, 0x78} // 0x6d m
  ,{0x7c, 0x08, 0x04, 0x04, 0x78} // 0x6e n
  ,{0x38, 0x44, 0x44, 0x44, 0x38} // 0x6f o
  ,{0x7c, 0x14, 0x14, 0x14, 0x08} // 0x70 p
  ,{0x08, 0x14, 0x14, 0x18, 0x7c} // 0x71 q
  ,{0x7c, 0x08, 0x04, 0x04, 0x08} // 0x72 r
  ,{0x48, 0x54, 0x54, 0x54, 0x20} // 0x73 s
  ,{0x04, 0x3f, 0x44, 0x40, 0x20} // 0x74 t
  ,{0x3c, 0x40, 0x40, 0x20, 0x7c} // 0x75 u
  ,{0x1c, 0x20, 0x40, 0x20, 0x1c} // 0x76 v
  ,{0x3c, 0x40, 0x30, 0x40, 0x3c} // 0x77 w
  ,{0x44, 0x28, 0x10, 0x28, 0x44} // 0x78 x
  ,{0x0c, 0x50, 0x50, 0x50, 0x3c} // 0x79 y
  ,{0x44, 0x64, 0x54, 0x4c, 0x44} // 0x7a z
  ,{0x00, 0x08, 0x36, 0x41, 0x00} // 0x7b {
  ,{0x00, 0x00, 0x7f, 0x00, 0x00} // 0x7c |
  ,{0x00, 0x41, 0x36, 0x08, 0x00} // 0x7d }
  ,{0x10, 0x08, 0x08, 0x10, 0x08} // 0x7e ~
  ,{0x78, 0x46, 0x41, 0x46, 0x78} // 0x7f DEL
};


byte displayMap[LCD_WIDTH * LCD_HEIGHT / 8] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (0,0)->(11,7) ~ These 12 bytes cover an 8x12 block in the left corner of the display 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (12,0)->(23,7)
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, // (24,0)->(35,7)
  0xF0, 0xF8, 0xFC, 0xFC, 0xFE, 0xFE, 0xFE, 0xFE, 0x1E, 0x0E, 0x02, 0x00, // (36,0)->(47,7)
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (48,0)->(59,7)
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (60,0)->(71,7)
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (72,0)->(83,7)
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (0,8)->(11,15)
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (12,8)->(23,15)
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, // (24,8)->(35,15)
  0x0F, 0x1F, 0x3F, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFC, 0xF8, // (36,8)->(47,15)
  0xF8, 0xF0, 0xF8, 0xFE, 0xFE, 0xFC, 0xF8, 0xE0, 0x00, 0x00, 0x00, 0x00, // (48,8)->(59,15)
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (60,8)->(71,15)
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (72,8)->(83,15)
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (0,16)->(11,23)
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (12,16)->(23,23)
  0x00, 0x00, 0xF8, 0xFC, 0xFE, 0xFE, 0xFF, 0xFF, 0xF3, 0xE0, 0xE0, 0xC0, // (24,16)->(35,23)
  0xC0, 0xC0, 0xE0, 0xE0, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // (36,16)->(47,23)
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3E, 0x00, 0x00, 0x00, // (48,16)->(59,23)
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (60,16)->(71,23)
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (72,16)->(83,23)
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (0,24)->(11,31)
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (12,24)->(23,31)
  0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // (24,24)->(35,31)
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // (36,24)->(47,31)
  0xFF, 0xFF, 0xFF, 0x7F, 0x3F, 0x1F, 0x07, 0x01, 0x00, 0x00, 0x00, 0x00, // (48,24)->(59,31)
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (60,24)->(71,31)
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (72,24)->(83,31)
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (0,32)->(11,39)
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (12,32)->(23,39)
  0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x3F, 0x1F, // (24,32)->(35,39)
  0x0F, 0x0F, 0x0F, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x03, 0x03, // (36,32)->(47,39)
  0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (48,32)->(59,39)
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (60,32)->(71,39)
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (72,32)->(83,39)
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (0,40)->(11,47)
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (12,40)->(23,47)
  0x00, 0x00, 0x3F, 0x1F, 0x0F, 0x07, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, // (24,40)->(35,47)
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (36,40)->(47,47)
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (48,40)->(59,47)
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (60,40)->(71,47)
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // (72,40)->(83,47) !!! The bottom right pixel!
};


/* All functions after main should be initialized here */
char inchar(void);
void outchar(char x);
void bco(char x);

// LCD Commands
void shiftout(char x);
void LCD_command(char x);
void gotoXY(int x, int y);
void updateDisplay(void);
void setPixel(int x, int y, char bw);
void drawLine(int x0, int y0, int x1, int y1, char bw);
void drawRect(int x0, int y0, int x1, int y1, char fill, char bw);
void fillDisplay(char bw);
void setChar(char character, int x, int y, char bw);
void LCD_print(char * message, int x, int y, char bw);
void LCD_printWrap(char * message, char bw);
void LCD_moveCurGlob(int x, int y);
void updateLatitude(void);
void updateLongitude(void);
void updateDate(void);
void updateTime(void);
void printStarMenu(void);
void wait(void);
void prompt_for_star(void);

// Keypad Commands
void clearKeypad(void);
int getKeypadPressed(void);

/* Variable declarations */
int keyPadData[3][4] = {{0,0,0,0},{0,0,0,0},{0,0,0,0}}; 
// Keypad Flags
char prevkp[] = {0,0,0,0,0,0,0,0,0,0,0,0};
char keypad[] = {0,0,0,0,0,0,0,0,0,0,0,0};
char deltaKP[3][4] = {{0,0,0,0},{0,0,0,0},{0,0,0,0}};

// LCD variables
int cursorX = 2;
int cursorY = 12;

// TIM 
char tencnt = 0;
char onecnt = 0;
char cursorToggle = 0;

// RPG Variables
#define RPGA PTT_PTT6
#define RPGB PTT_PTT5
#define RPGBUTTON PTT_PTT4
char prevRPG = 0;
char RPG = 0;
char rpgleftflag = 0;
char rpgrightflag = 0;
char prevrpgA = 1;
char prevrpgB = 1;

char validInput = 0;
char Latitude[6] = "+____";
char Longitude[7] = "+_____";
int lastKeypad = 0;
int i = 0;
int j = 0;
char getOut = 0;
char date[9] = "2YYYMMDD";
char time[6] = "HHMMA";
char star = '1';
int Menu = 1;
int Position = 1;
char *starList[8];//[11];
//char New_Data = '0';


// Transmit Variables
char tin	= 0;	// SCI transmit display buffer IN pointer
char tout	= 0;	// SCI transmit display buffer OUT pointer
#define TSIZE 101	// transmit buffer size (80 characters)
char tbuf[TSIZE];	// SCI transmit display buffer    

       	   			 		  			 		      
/*	 	   		
***********************************************************************
 Initializations
***********************************************************************
*/

void  initializations(void) {

/* Set the PLL speed (bus clock = 24 MHz) */
  CLKSEL = CLKSEL & 0x80; //; disengage PLL from system
  PLLCTL = PLLCTL | 0x40; //; turn on PLL
  SYNR = 0x02;            //; set PLL multiplier
  REFDV = 0;              //; set PLL divider
  while (!(CRGFLG & 0x08)){  }
  CLKSEL = CLKSEL | 0x80; //; engage PLL

/* Disable watchdog timer (COPCTL register) */
  COPCTL = 0x40   ; //COP off; RTI and COP stopped in BDM-mode

/* Initialize asynchronous serial port (SCI) for 9600 baud, interrupts off initially */
  SCIBDH =  0x00; //set baud rate to 9600
  SCIBDL =  0x9C; //24,000,000 / 16 / 156 = 9600 (approx)  
  SCICR1 =  0x00; //$9C = 156
  SCICR2 =  0x0C; //initialize SCI for program-driven operation
  DDRB   =  0x10; //set PB4 for output mode
  PORTB  =  0x10; //assert DTR pin on COM port

/* Initialize peripherals */
  DDRAD = 0b10101001; // set Analog data registers as inputs for bits 7-1 (
  PERAD = 0b01010110; // Set Row Pins to Pullup
                      // and AN0 as output (reset for XBee)
  ATDDIEN = 0b01010110;                    
  #ifdef DEBUG
  DDRT_DDRT0 = 1; 
  DDRT_DDRT1 = 1;                    
  #endif        
  
  
// TIM isn't necessary
/* 
   Initialize TIM Ch 7 (TC7) for periodic interrupts every 10.0 ms  
    - Enable timer subsystem                         
    - Set channel 7 for output compare
    - Set appropriate pre-scale factor and enable counter reset after OC7
    - Set up channel 7 to generate 10 ms interrupt rate
    - Initially disable TIM Ch 7 interrupts	 	   			 		  			 		  		
*/	 	   	
/*		 		  			 		  		
  TSCR1_TEN = 1; // enable timer subsystem
  TIOS_IOS7 = 1; // 7 as output compare
  //TSCR2_PR2 = 1; // prescale to 16
  TSCR2 = 0x0C;
  TSCR2_TCRE = 1;// counter reset after successful output
  TIE_C7I = 0;   // disable channel 7 interrupts
  
  TC7 = 15000;     // 10ms interrupt rate = tc7 15000
  */
    
  // Initialize SPI for LCD
  //
  DDRT = 0b00001111; // PTT 3, 2, 1 are outputs.
  SPIBR = 0x10;
  SPICR1 = 0x50;
  SPICR2 = 0x00;
  PERT_PERT4 = 0;
  PTT_PTT0 = 0;
  DDRAD_DDRAD0 = 1;
  PTAD_PTAD0 = 1;
  
            
/* Initialize interrupts */

/* Initialize RTI for 2.048 ms interrupt rate */	
  CRGINT_RTIE = 1; // enable RTI interrupt
  RTICTL = 0x1F; // 1.024ms interrupt rate
  //RTICTL =0x50; // 2.048ms interrupt rate
  
  // Initialize LCD
  // Reset LCD by pulsing RST low, then high
  PTT_PTT2 = 0;
  PTT_PTT2 = 1;    
  LCD_command(0x21); //Tell LCD extended commands follow
  LCD_command(0xB0); //Set LCD Vop (Contrast)
  LCD_command(0x04); //Set Temp coefficent
  LCD_command(0x14); //LCD bias mode 1:48 (try 0x13)
  //We must send 0x20 before modifying the display control mode
  LCD_command(0x80 | 65); //Set LCD Vop (Contrast): Try 0xB1(good @ 3.3V) or 0xBF if your display is too dark
  LCD_command(0x20); 
  LCD_command(0x0C); //Set display control, normal mode. 
  fillDisplay(0);
  // Draw GUI boxes
  drawRect(0,0,83,47,0,1);
  drawRect(0,0,83,10,0,1);
  //         123456789ABCDE
  //         total of 13ish characters
  LCD_print("Star Treker!", 2, 2, 1);
  updateDisplay();  
  
  
    
}

	 		  			 		  		
/*	 		  			 		  		
***********************************************************************
Main
***********************************************************************
*/
void main(void) {
/*#############################
STAR DECLARATIONS
#############################*/

starList[0] = "Betelgeuse";
starList[1] = "Canopus";
starList[2] = "Pleiades";
starList[3] = "Polaris";
starList[4] = "Rigel";
starList[5] = "Sirius";
starList[6] = "Sun";
starList[7] = "Vega";

   DisableInterrupts;
	initializations(); 		  			 		  		
	EnableInterrupts;
   
// Begin Prompt Section
 // Position
 //////////////////////////////////
 //Latitude
 //////////////////////////////////
 LCD_printWrap("Position:\r\n", 1);
 LCD_printWrap(" LAT:+ __.__\r\n",1);
 LCD_printWrap("LONG:+___.__\r\n",1);
 
 while(!validInput)
 {
  while(RPG == 0 && !getOut)
  {
  /*
    for(i = 0; i < 12; i++)
    {
      if(keypad[i] == 1)
      { 
        getOut = 1;
        break;
      }
    }
   */
   i = getKeypadPressed();
  }
  getOut = 0;
  if(RPG == 1)
  {
    RPG = 0;
    if(Latitude[1] != '_' &&
       Latitude[2] != '_' &&
       Latitude[3] != '_' &&
       Latitude[4] != '_')
    {
      validInput = 1;
      continue; 
    }
    
  }
  else
  { 
   // reset input if you input too much
   if(j == 0 && Latitude[4] != '_' && i != 9)
    {
       for(j=1; j<5; j++)
       {
       Latitude[j] = '_'; 
       }
      updateLatitude();
        j=0;
    }
    // Zero pressed
    if(i == 10)
    {
      Latitude[1+j] = '0';
    }
    // Change sign
    else if(i == 9 || i == 11)
    {
      if(Latitude[0] == '+')
        Latitude[0] = '-';
      else
        Latitude[0] = '+';
    updateLatitude();
    j = (j-1)%5;
    }
    // Error trap for |latitude| > 90.00
    else 
    {
      if(i != 10 && Latitude[1] == '9' && (j == 1 || j == 2 || j == 3)) {
        j = (j-1)%4; 
      } else {
        
        Latitude[1+j] =  i+'1';
      }
    }
    j = (j+1)%4;    
  }
  updateLatitude();
  clearKeypad();
 }
 setChar('?', 74, 22, 1);
 updateDisplay();
 validInput = 0;
 RPG = 0;
 j = 0;
 /////////////////////////////
 // Get Longitude
 /////////////////////////////
 
 while(!validInput)
 {
  while(RPG == 0 && !getOut)
  {
    for(i = 0; i < 12; i++)
    {
      if(keypad[i] == 1)
      { 
        getOut = 1;
        break;
      }
    }
  }
  getOut = 0;
  if(RPG == 1)
  {
    RPG = 0;
    if(Longitude[1] != '_' &&
       Longitude[2] != '_' &&
       Longitude[3] != '_' &&
       Longitude[4] != '_' &&
       Longitude[5] != '_')
    {
      validInput = 1;
      continue; 
    }
    
  }
  else
  { 
    //reset longitude
    if(j == 0 && Longitude[5] != '_' && i != 9)
    {
       for(j=1; j<6; j++)
       {
       Longitude[j] = '_'; 
       }   
        updateLongitude();
        j=0;
    }
    if(i == 10)
    {
      Longitude[1+j] = '0';
    }
    else if(i == 9 || i == 11)
    {
      if(Longitude[0] == '+')
        Longitude[0] = '-';
      else
        Longitude[0] = '+';
        updateLongitude();
    j = (j-1)%5;
    }
    else 
    {
      if((j == 0 && (i != 0)) ||
         (j > 0 && Longitude[1] == '1' && i == 8) ||
         (j > 1 && Longitude[1] == '1' && Longitude[2] == '8'))
      {
        j = (j-1)%5;
      } else {
        Longitude[1+j] =  i+'1';
      }
    }
    j = (j+1)%5;    
  }
  updateLongitude();
  clearKeypad();
 }
 setChar('?', 74, 32, 1);
 updateDisplay();
 /////////////////////////////////////////
 // DATE
 /////////////////////////////////////////
 wait();
 drawRect(2,12,82,46,1,0);
 updateDisplay(); 
 LCD_moveCurGlob(2,12);
 LCD_printWrap("Date:\r\n", 1); 
 updateDate();
 validInput = 0;
 getOut = 0;
 i = 0;
 j = 0;
 while(!validInput)
 {
    if (j==8)
      j=0;
    clearKeypad();
    while(!getOut && !RPG)
    {
      for(i=0; i<11; i++)
      {
         if(keypad[i])
         {
           getOut=1;
           break;
         }
      }
      
    }
    getOut = 0;
    if(RPG)
    {
      RPG = 0;
      if(date[0] != 'Y' && date[1] != 'Y' && date[2] != 'Y' && date[3] != 'Y' &&  
         date[4] != 'M' && date[5] != 'M' && date[6] != 'D' && date[7] != 'D') {
        
          validInput = 1;
          break;
         }
    }
    //reset longitude
    if(j == 0 && date[7] != 'D')
    {
       date[0] = '2';
       date[1] = 'Y';
       date[2] = 'Y';
       date[3] = 'Y';
       date[4] = 'M';
       date[5] = 'M';
       date[6] = 'D';
       date[7] = 'D';
       updateDisplay();
       updateDate();
       j=0;
    }
    if(i == 9 || i == 11)
    {
      continue;
    }
    //check valid months
    if(
        (i != 0 && i != 10 && j == 3) ||
        (i > 1 && i < 9 && date[4] == '1' && j == 4) ||
        (i == 10 && date[4] == '0' && j == 4)
        )
    {
      continue; 
    }
    // valid dates
    if(((i != 0 && i != 10 && i != 1 && i != 2) && j == 5) ||
       (i > 0 && i != 10 && j == 6 && date[6] == '3') ||
       (i == 10 && date[6] == '0' && j==6)
    )
    {                   
      continue; 
    }
    
    date[j+1] = i+'1';
    if(i == 10)
    {
      date[j+1] = '0'; 
    }
    j = (j+1)%7;
    updateDate(); 
  
 }
 setChar('?', 68, 22, 1);
 updateDisplay();
 /////////////////////////////
 // TIME
 /////////////////////////////
 wait();
 drawRect(2,12,82,46,1,0);
 updateDisplay(); 
 LCD_moveCurGlob(2,12);
 LCD_printWrap("Time:\r\n", 1); 
 updateTime();
 validInput = 0;
 getOut = 0;
 i = 0;
 j = 0;
 while(!validInput)
 {
    clearKeypad();
    while(!getOut && !RPG)
    {
      for(i=0; i<11; i++)
      {
         if(keypad[i])
         {
           getOut=1;
           break;
         }
      }
      
    }
    getOut = 0;
    if(RPG)
    {
      RPG = 0;
      if(time[0] != 'H' && time[1] != 'H' && time[2] != 'M' && time[3] != 'M') {
        
          validInput = 1;
          break;
      }
    }
    RPG = 0;
    //reset time
    if(j == 0 && time[3] != 'M' && i != 9)
    {
       time[0] = 'H';
       time[1] = 'H';
       time[2] = 'M';
       time[3] = 'M';
       updateDisplay();
       updateTime();
       j=0;
    }
    if(i == 9)
    {
      if(time[4] == 'A')
        time[4] = 'P';
      else
        time[4] = 'A';
      updateTime();
      continue; 
    }
    if(i == 11)
    {
      continue;
    }
    time[j] = i+'1';
    if(i == 10)
    {
      time[j] = '0'; 
    }  
    if(time[0] > '1' && j == 0)
    {
      time[0] = 'H';
      j = (j-1)%4;     
    }
    if(j == 1 && (time[0] == '1' && time[1] > '2')) {
      time[1] = 'H';
      j = (j-1)%4; 
    }
    if(time[2] > '5' && j == 2)
    {
      time[2] = 'M';
      j = (j-1)%4; 
    }
    j = (j+1)%4;
    updateTime(); 
  
 }
 setChar('?', 62, 22, 1);
 updateDisplay();
 //////////////////////
 //MAIN
 ////////////////////////
 // Adjust time if necessary
    RPG = 0;
 if (time[4] == 'P') 
 { 
    time[0]+=1;
    time[1]+=2;
 }
    Menu = 1;
    wait();
    prompt_for_star();
    
 // SEND THINGS
 // Latitude is stored in Latitude = "+XXXX"
 // Longitude is stored in Longitude = "+YYYYY"
 // Date is stored in date = "YYYYMMDD"
 // Time is stored in time = "HHMMA"
 // then send
 
    RPG = 0;
    for(;;) 
 
 {
     // Transmit forever until new data is entered
     //bco('n');
     //bco(New_Data);
     bco('p');
     bco(Latitude[0]);  // Latitude Sign
     bco(Latitude[1]);
     bco(Latitude[2]);
     bco(Latitude[3]);
     bco(Latitude[4]);    
     bco('o');
     bco(Longitude[0]); //Longitude Sign
     bco(Longitude[1]);
     bco(Longitude[2]);
     bco(Longitude[3]);
     bco(Longitude[4]);
     bco(Longitude[5]);
     bco('t'); // Month
     bco(date[4]);
     bco(date[5]);
     bco('d'); // Date
     bco(date[6]);
     bco(date[7]);
     bco('y'); // Year
     bco(date[0]);
     bco(date[1]);
     bco(date[2]);
     bco(date[3]);
     bco('h'); // Hour
     bco(time[0]);
     bco(time[1]);
     bco('m'); // Min
     bco(time[2]);
     bco(time[3]);
     bco('s');
     bco(star);
     
     // Change star
     if(RPG == 1) 
     {
        RPG = 0;
        prompt_for_star();
        //New_Data++;
     }
  
   } /* loop forever */
   
}   /* do not leave main */




/*
***********************************************************************                       
 RTI interrupt service routine: RTI_ISR
************************************************************************
*/

interrupt 7 void RTI_ISR(void)
{ 
    int i = 0;
    int j = 0;
    
    
  	
    /*
    // Column Port Register
    char colPT[3] = {&PTAD_PTAD7, &PTAD_PTAD5, &PTAD_PTAD3};
    // Column Direction Registers
    char colDR[3] = {&DDRAD_DDRAD7, &DDRAD_DDRAD5, &DDRAD_DDRAD3};      
    // Row Port Registers
    char rowPT[4] = {&PTAD_PTAD6, &PTAD_PTAD4, &PTAD_PTAD2, &PTAD_PTAD1};      
    // Row Direction Registers
    char rowDR[4] = {&DDRAD_DDRAD6, &DDRAD_DDRAD4, &DDRAD_DDRAD2, &DDRAD_DDRAD1};
    */
    // clear RTI interrupt flagt
  	CRGFLG = CRGFLG | 0x80;
  	// Rotary pulse generator push button
  	if(RPGBUTTON == 1 && prevRPG == 0) RPG=1;
  	if(RPGB == 1 && RPGA == 1 && prevrpgA == 1 && prevrpgB == 0) rpgrightflag = 1;
  	if(RPGA == 1 && RPGB == 1 && prevrpgA == 0 && prevrpgB == 1) rpgleftflag = 1;
  	prevrpgA = RPGA;
  	prevrpgB = RPGB;
  	prevRPG = RPGBUTTON;   	
    /*    Keypad Section      */
    // Set row to inputs, column to outputs
    // column = 1,3,5
    // row = 2,4,6,7
    // AN7 maps to 1, etc
    // 7 6 5 4 3 2 1 //AN
    // 1 2 3 4 5 6 7 //KEYPAD PINS
    //DDRAD = 0b00000001;
    // Start Scan
    //DDRAD_DDRAD7 = 1; // Set Column 1 (KP 1) to output
    for(i=0;i<3;i++) {
      for(j=0;j<4;j++) { 
        deltaKP[i][j] = 0;
      }
    }// reset deltaKP
    PTAD_PTAD7 = 0;   // Set Column 1 (KP 1) low
    //for(i=0;i<16000;i++);
    while(PTAD_PTAD7 !=0);
    keyPadData[1][0] += !PTAD_PTAD6;
    deltaKP[1][0] = !PTAD_PTAD6;
    keyPadData[1][3] += !PTAD_PTAD4;
    deltaKP[1][3] = !PTAD_PTAD4;
    keyPadData[1][2] += !PTAD_PTAD2;
    deltaKP[1][2] = !PTAD_PTAD2;
    keyPadData[1][1] += !PTAD_PTAD1;
    deltaKP[1][1] = !PTAD_PTAD1;
    PTAD_PTAD7 = 1;   // Set Column 1 (KP 1) high
    //DDRAD_DDRAD5 = 1; // Set Column 2 (KP 3) to output
    PTAD_PTAD5 = 0;   // Set Column 2 (KP 3) low
    while(PTAD_PTAD5 !=0);
    //for(i=0;i<16000;i++);
    keyPadData[0][0] += !PTAD_PTAD6;
    deltaKP[0][0] = !PTAD_PTAD6;
    keyPadData[0][3] += !PTAD_PTAD4;
    deltaKP[0][3] = !PTAD_PTAD4;
    keyPadData[0][2] += !PTAD_PTAD2;
    deltaKP[0][2] = !PTAD_PTAD2;
    keyPadData[0][1] += !PTAD_PTAD1;
    deltaKP[0][1] = !PTAD_PTAD1;
    PTAD_PTAD5 = 1;   // Set Column 2 (KP 3) high
    //DDRAD_DDRAD3 = 1; // Set Column 3 (KP 5) to output
    PTAD_PTAD3 = 0;   // Set Column 3 (KP 5) low
    while(PTAD_PTAD3 !=0);
    //for(i=0;i<16000;i++);
    keyPadData[2][0] += !PTAD_PTAD6;
    deltaKP[2][0] = !PTAD_PTAD6;
    keyPadData[2][3] += !PTAD_PTAD4;    
    deltaKP[2][3] = !PTAD_PTAD4;
    keyPadData[2][2] += !PTAD_PTAD2;
    deltaKP[2][2] = !PTAD_PTAD2;
    keyPadData[2][1] += !PTAD_PTAD1;
    deltaKP[2][1] = !PTAD_PTAD1;
    PTAD_PTAD3 = 1;   // Set Column 3 (KP 5) high
    
    for(i=0; i<3; i++) {
      for(j=0; j<4; j++) {        
       if( prevkp[i+3*j] && !deltaKP[i][j] ) 
       {
           prevkp[i+3*j] = 0;
           keypad[i+3*j] = 1;   
       }
       if(keyPadData[i][j] > 10) {
           prevkp[i+3*j] = 1;
           keyPadData[i][j] = 0; 
       }
      }
    }
    
    // Rotary Pulse Generator
    /* if(PTT_PTT4 != 0) {
		   if(prevRPG == 1) {
		    RPG = 1;
		   }
		    prevRPG = 0;
		  } else if (PTT_PTT4 == 0) {
		   prevRPG = 1;
		  } */
    // if(PTT_PTT4 != 0) {
		//   if(prevRPG == 1) {
		//    RPG = 1;
		//   }
		//    prevRPG = 0;
		//  } else if (PTT_PTT4 == 0) {
		//   prevRPG = 1;
		//  }
  	
  	}

/*
***********************************************************************                       
  TIM interrupt service routine	  		
***********************************************************************
*/

interrupt 15 void TIM_ISR(void)
{
  
  	// clear TIM CH 7 interrupt flag 
 	TFLG1 = TFLG1 | 0x80; 
 	/*
 	tencnt++;
 	if(tencnt == 10) {
 	  tencnt = 0;
 	  onecnt++;
 	  if(onecnt == 10) {
 	    onecnt = 0;
 	    cursorToggle = (cursorToggle + 1)%2;
 	    if(cursorToggle) { 	      
 	      setChar('_', cursorX, cursorY, 1);
 	      updateDisplay();
 	    }
 	    else { 
 	      setChar(' ', cursorX, cursorY, 1);
 	      updateDisplay();
 	    }
 	  }
 	}
 	*/
 	
}

/*
***********************************************************************                       
  SCI interrupt service routine		 		  		
***********************************************************************
*/

interrupt 20 void SCI_ISR(void)
{
 
    //while ((SCISR1 & 0x80) != 0x80)
   while(SCISR1_TDRE == 0)
   { 
   }
      if (tin == tout)
      {
         // SCICR2 =  0x0C;
         SCICR2_SCTIE = 0;
      }
      else
      {
         SCIDRL = tbuf[tout];
         tout = (tout % TSIZE) + 1;
      }

}

/*
***********************************************************************                              
  SCI buffered character output routine - bco

  Places character x passed to it into TBUF

   - check TBUF status: if FULL, wait for space; else, continue
   - place character in TBUF[TIN]
   - increment TIN mod TSIZE
   - enable SCI transmit interrupts

  NOTE: DO NOT USE OUTCHAR (except for debugging)
***********************************************************************
*/

void bco(char x)
{
   while(((tin % TSIZE)+1) == tout)
   {
   }
  
   tbuf[tin] = x;
   tin = (tin % TSIZE) + 1;
   
   SCICR2_SCTIE  = 1;
 

}



/*
***********************************************************************
 Character I/O Library Routines for 9S12C32 
***********************************************************************
 Name:         inchar
 Description:  inputs ASCII character from SCI serial port and returns it
 Example:      char ch1 = inchar();
***********************************************************************
*/

char inchar(void) {
  /* receives character from the terminal channel */
        while (!(SCISR1 & 0x20)); /* wait for input */
    return SCIDRL;
}

/*
***********************************************************************
 Name:         outchar    (use only for DEBUGGING purposes)
 Description:  outputs ASCII character x to SCI serial port
 Example:      outchar('x');
***********************************************************************
*/

void outchar(char x) {
  /* sends a character to the terminal channel */
    while (!(SCISR1 & 0x80));  /* wait for output buffer empty */
    SCIDRL = x;
}


/*
########################################################################

 Keypad!

########################################################################
*/

int getKeypadPressed(void)
{
  int i = 0;
  for(i=0; i<12; i++)
  {
     if(keypad[i])
     {     
        getOut = 1;
        return i;
     }
  }
  return -1;
}

void clearKeypad(void)
{
  int i = 0;
  for(i=0;i<12;i++)
  {
    keypad[i] = 0;
  }
}
  

/*
#########################################################################
  Nokia 84x48 LCD board Commands
  PTT_PTT3 = Chip Select (SCE)
  PTT_PTT2 = Reset
  PTT_PTT1 = Command (low) vs Data (high) mode
  PM5/SCICK = SCLK
  PM4/MOSI = DN (data in/mosi)
  
#########################################################################
*/

void shiftout(char x) {
  int i = 0;  
  while((SPISR_SPTEF) != 1) {};
  
  SPIDR = x;
  for(i=0; i<300; i++);
}

void LCD_command(char x) {
    //assert low for command
    PTT_PTT1 = 0;
    shiftout(x);
    PTT_PTT1 = 1;  
}

void updateDisplay()
{
  int i = 0;
  TIE_C7I = 0;
  gotoXY(0, 0);
  for (i=0; i < (LCD_WIDTH * LCD_HEIGHT / 8); i++)
  {
    shiftout(displayMap[i]);
  }
  TIE_C7I = 1;
}

void gotoXY(int x, int y) {
 LCD_command(0x80 | x);
 LCD_command(0x80 | y); 
  
}

void drawLine(int x0, int y0, int x1, int y1, char bw) {
 
  int dy = y1 - y0; // Difference between y0 and y1
  int dx = x1 - x0; // Difference between x0 and x1
  int stepx, stepy;
  int fraction = 0;

  if (dy < 0)
  {
    dy = -dy;
    stepy = -1;
  }
  else
    stepy = 1;

  if (dx < 0)
  {
    dx = -dx;
    stepx = -1;
  }
  else
    stepx = 1;

  dy <<= 1; // dy is now 2*dy
  dx <<= 1; // dx is now 2*dx
  setPixel(x0, y0, bw); // Draw the first pixel.

  if (dx > dy) 
  {
    fraction = dy - (dx >> 1);
    while (x0 != x1)
    {
      if (fraction >= 0)
      {
        y0 += stepy;
        fraction -= dx;
      }
      x0 += stepx;
      fraction += dy;
      setPixel(x0, y0, bw);
    }
  }
  else
  {
    fraction = dx - (dy >> 1);
    while (y0 != y1)
    {
      if (fraction >= 0)
      {
        x0 += stepx;
        fraction -= dy;
      }
      y0 += stepy;
      fraction += dx;
      setPixel(x0, y0, bw);
    }
  }   
}


void setPixel(int x, int y, char bw)
{
  byte shift = 0;
  // First, double check that the coordinate is in range.
  if ((x >= 0) && (x < LCD_WIDTH) && (y >= 0) && (y < LCD_HEIGHT))
  {
    shift = (byte)y % 8;
  
    if (bw) // If black, set the bit.
      displayMap[x + (y/8)*LCD_WIDTH] |= 1<<shift;
    else   // If white clear the bit.
      displayMap[x + (y/8)*LCD_WIDTH] &= ~(1<<shift);
  }
}

void drawRect(int x0, int y0, int x1, int y1, char fill, char bw)
{
  int xDiff;
  // check if the rectangle is to be filled
  if (fill == 1)
  {

    if(x0 > x1)
      xDiff = x0 - x1; //Find the difference between the x vars
    else
      xDiff = x1 - x0;

    while(xDiff > 0)
    {
      drawLine(x0, y0, x0, y1, bw);

      if(x0 > x1)
        x0--;
      else
        x0++;

      xDiff--;
    }
  }
  else 
  {
    // best way to draw an unfilled rectangle is to draw four lines
    drawLine(x0, y0, x1, y0, bw);
    drawLine(x0, y1, x1, y1, bw);
    drawLine(x0, y0, x0, y1, bw);
    drawLine(x1, y0, x1, y1, bw);
  }
}



void fillDisplay(char bw) {
  int i = 0;
  for(i=0;i<(LCD_WIDTH*LCD_HEIGHT/8); i++) {
    if(bw)
      displayMap[i] = 0xFF;
    else
      displayMap[i] = 0;
  }
    
  
}

// This function will draw a char (defined in the ASCII table
// near the beginning    of this sketch) at a defined x and y).
// The color can be either black (1) or white (0).
void setChar(char character, int x, int y, char bw)
{
  int i, j;
  byte column; // temp byte to store character's column bitmap
  for (i=0; i<5; i++) // 5 columns (x) per character
  {
    column = ASCII[character - 0x20][i];
    for (j=0; j<8; j++) // 8 rows (y) per character
    {
      if (column & (0x01 << j)) // test bits to set pixels
        setPixel(x+i, y+j, bw);
      else
        setPixel(x+i, y+j, !bw);
    }
  }
}

void LCD_print(char * message, int x, int y, char bw) {
  int i = 0;
  for(i=0; message[i] != '\0'; i++) {
    setChar(message[i], x, y, bw);
    x+=6;
    if (message[i] == 'i' || message[i] == 'I' || message[i] == 'l' 
     || message[i] == '|' || message[i] == '1') {
      x+=-1;
     }
  }
  updateDisplay();
}

void LCD_printWrap(char * message, char bw) {
  int i = 0;
  for(i=0; message[i] != '\0'; i++) {
    if(message[i] == '\n')
    {
      cursorY += 10;
    }
    else if(message[i] == '\r')
    {
      cursorX = 2;
    }
    else
    {
      setChar(message[i], cursorX, cursorY, bw);
      cursorX+=6;
      if (cursorX > 77) {
        cursorX = 2;
        cursorY += 10;
      }
    }
  }
  updateDisplay();
}

void LCD_moveCurGlob(int x, int y)
{
   cursorX = x;
   cursorY = y;
}

/////////////////////////////////////////////////////
// LCD / INPUT FUNCTIONS
/////////////////////////////////////////////////////

void updateLatitude(void)
{
      setChar(Latitude[0], 32, 22, 1);
      setChar(Latitude[1], 44, 22, 1);
      setChar(Latitude[2], 50, 22, 1);
      setChar(Latitude[3], 62, 22, 1);
      setChar(Latitude[4], 68, 22, 1);
      updateDisplay();
}

void updateLongitude(void)
{
   setChar(Longitude[0], 32, 32, 1);
   setChar(Longitude[1], 38, 32, 1);
   setChar(Longitude[2], 44, 32, 1);
   setChar(Longitude[3], 50, 32, 1);
   setChar(Longitude[4], 62, 32, 1);
   setChar(Longitude[5], 68, 32, 1);
   updateDisplay();
}

void updateDate(void)
{
   setChar(date[0], 8,22,1);
   setChar(date[1], 14,22,1);
   setChar(date[2], 20,22,1);
   setChar(date[3], 26,22,1);
   setChar('/',     32,22,1);
   setChar(date[4], 38,22,1);
   setChar(date[5], 44,22,1);
   setChar('/',     50,22,1);
   setChar(date[6], 56,22,1);
   setChar(date[7], 62,22,1);
   updateDisplay();
}

void updateTime(void)
{
   setChar(time[0], 14, 22, 1); 
   setChar(time[1], 20, 22, 1);
   setChar(':', 26, 22, 1);
   setChar(time[2], 32, 22, 1);
   setChar(time[3], 38, 22, 1);
   setChar(' ', 44, 22, 1);
   setChar(time[4], 50, 22, 1);
   setChar('M', 56, 22, 1);
   updateDisplay();
}

void printStarMenu(void)
{
   drawRect(2,12,82,46,1,0);
   updateDisplay();
   LCD_moveCurGlob(2,12);
   if(Menu>0)
    LCD_printWrap(starList[(Menu-1)%8],1);
   else
    LCD_printWrap(starList[7],1);
   LCD_printWrap("\r\n->",1);
   LCD_printWrap(starList[Menu],1);
   LCD_printWrap("\r\n",1);
   LCD_printWrap(starList[(Menu+1)%8],1);
   updateDisplay();
}

void wait(void)
{
 for(i=0;i<5000;i++)
 {
   for(j=0;j<500;j++);
 }
}

void prompt_for_star(void)
{
 // Set star
    star = '1';
    Menu = 1;  
    RPG = 0;
    clearKeypad();
    drawRect(2,12,82,46,1,0);
    LCD_moveCurGlob(2,2);
    LCD_printWrap("Select Star:", 1);     
    LCD_moveCurGlob(2,12);
    validInput = 0;
    getOut = 0;
    i = 0;
    j = 0;
    validInput = 0;
    printStarMenu();
    while(!validInput)
    {
      while(!getOut && !RPG)
      {
        for(i=0; i<12; i++)
        {
           if(keypad[i])
           {
             getOut=1;
             break;
           }
        }
      }
      getOut = 0;
      LCD_moveCurGlob(2,12);
      // if KP 9 or 11 are pressed, move up or down
      if(RPG)
      {
        RPG = 0;
        validInput = 1;
        break;
      }
      if(i == 9)
      {
        Menu = (Menu-1);//%8;
        if(Menu <0)
        {
           Menu = 7;
        }
        star = '0' + Menu;
      } 
      else if(i == 11)
      {
        Menu = (Menu+1);//%8;
        if(Menu > 7)
        {
           Menu = 0;
        }
        star = '0' + Menu; 
      }
      // Print 3 things
      printStarMenu();
      clearKeypad();
    } 
     drawRect(2,12,82,46,1,0);
     updateDisplay();
     LCD_moveCurGlob(2,12);
     LCD_printWrap("Finding:\r\n", 1);
     LCD_printWrap(starList[Menu],2);
     LCD_printWrap("...",2);    
    //setChar('?', 74, 22, 1);
}