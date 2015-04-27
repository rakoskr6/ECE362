/*
************************************************************************
 ECE 362 - Mini-Project C Source File - Spring 2015
***********************************************************************
	 	   			 		  			 		  		
 Team ID: < ? >

 Project Name: < ? >

 Team Members:

   - Team/Doc Leader: < ? >      Signature: ______________________
   
   - Software Leader: < ? >      Signature: ______________________

   - Interface Leader: < ? >     Signature: ______________________

   - Peripheral Leader: < ? >    Signature: ______________________


 Academic Honesty Statement:  In signing above, we hereby certify that we 
 are the individuals who created this HC(S)12 source file and that we have
 not copied the work of any other student (past or present) while completing 
 it. We understand that if we fail to honor this agreement, we will receive 
 a grade of ZERO and be subject to possible disciplinary action.

***********************************************************************

 The objective of this Mini-Project is to .... < ? >


***********************************************************************

 List of project-specific success criteria (functionality that will be
 demonstrated):

 1.

 2.

 3.

 4.

 5.

***********************************************************************

  Date code started: < ? >

  Update history (add an entry every time a significant change is made):

  Date: < ? >  Name: < ? >   Update: < ? >

  Date: < ? >  Name: < ? >   Update: < ? >

  Date: < ? >  Name: < ? >   Update: < ? >


***********************************************************************
*/

#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include <mc9s12c32.h>
#include <float.h>
#include <math.h>

/* All functions after main should be initialized here */
char inchar(void);
void outchar(char x);

//SPI Utility Functions
void transmit(char x);
char receive();
char transfer(char x);

//Compass Utility Functions
char compassReadByte(char x);
void compassWriteByte(char address, char data);
void compassReadBytes(char address, char * dest, char count);
void readMag();
double getHeading(double xMag, double yMag);


/* Variable declarations */
double mRes = 2.0 / 32768.0;
char mx; char my; char mz;
char check;
   	   			 		  			 		       

/* Special ASCII characters */
#define CR 0x0D		// ASCII return 
#define LF 0x0A		// ASCII new line 

/* LCD COMMUNICATION BIT MASKS (note - different than previous labs) */
#define RS 0x10		// RS pin mask (PTT[4])
#define RW 0x20		// R/W pin mask (PTT[5])
#define LCDCLK 0x40	// LCD EN/CLK pin mask (PTT[6])

/* LCD INSTRUCTION CHARACTERS */
#define LCDON 0x0F	// LCD initialization command
#define LCDCLR 0x01	// LCD clear display command
#define TWOLINE 0x38	// LCD 2-line enable command
#define CURMOV 0xFE	// LCD cursor move instruction
#define LINE1 = 0x80	// LCD line 1 cursor position
#define LINE2 = 0xC0	// LCD line 2 cursor position

/* COMPASS REGISTERS */
#define COMPCS PTT_PTT2
#define WHO_AM_I_XM 0x0F //Check register
#define CTRL_REG0_XM 0x1F
#define CTRL_REG1_XM 0x20
#define CTRL_REG2_XM 0x21
#define CTRL_REG3_XM 0x22
#define CTRL_REG4_XM 0x23
#define CTRL_REG5_XM 0x24
#define CTRL_REG6_XM 0x25
#define CTRL_REG7_XM 0x26
#define INT_CTRL_REG_M 0x12
#define OUT_X_L_M 0x08
#define PI 3.14159


	 	   		
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

// SPI Initialization

  SPICR1 = 0x1C; //master, no interrupt, idle high, sample at odd edges, off initially
  SPICR2 = 0; //normal, non-bidirection
  SPIBR = 0x12; //Baud rate to 6.0 Megabits/second
  
  // Compass Initialization
  DDRT = 0x04;
  COMPCS = 1;
  compassWriteByte(CTRL_REG0_XM, 0x00);
  compassWriteByte(CTRL_REG1_XM, 0x00);
  compassWriteByte(CTRL_REG5_XM, 0x94);
  compassWriteByte(CTRL_REG6_XM, 0x00);
  compassWriteByte(CTRL_REG7_XM, 0x00);
  compassWriteByte(CTRL_REG4_XM, 0x04);
  compassWriteByte(INT_CTRL_REG_M, 0x09);
  
            
/* Initialize interrupts */
	      
	      
}

	 		  			 		  		
/*	 		  			 		  		
***********************************************************************
Main
***********************************************************************
*/
void main(void) {
  	DisableInterrupts;
	initializations(); 		  			 		  		
	EnableInterrupts;
  check = compassReadByte(WHO_AM_I_XM);
  readMag();
  getHeading((double) mx, (double) my);
 for(;;) {
  
/* < start of your main loop > */ 
  
  

  
   } /* loop forever */
   
}   /* do not leave main */
/*
****************************
SPI UTILITY FUNCTIONS
****************************
*/

void transmit(char x){
  while(SPISR_SPTEF == 0);
  SPIDR = x;
}

char receive(){
  while(SPISR_SPIF == 0);
  return SPIDR;
}

char transfer(char x){
  transmit(x);
  return receive();
}

/*
*****************************
COMPASS UTILITY FUNCTIONS
*****************************
*/

char compassReadByte(char x){
  char returnChar;
  x |= 0x80; //MSB for compass reads is 1
  COMPCS = 0;
  SPICR1 |= 0x40; //Enables SPI
  transfer(x); //Sends the instruction to compass
  returnChar = transfer(0x00); //Reads the result of instruction
  SPICR1 &= 0xBF; //Disables SPI
  COMPCS = 1;
  return returnChar;
}

void compassWriteByte(char address, char data){
  address &= 0x3F; //Only the last six bits are valid for write
  COMPCS = 0;
  SPICR1 |= 0x40; //Enables SPI
  transfer(address); //Gives the compass the address
  transfer(data); //Gives the compass the data to write
  SPICR1 &= 0xBF; //Disables SPI
  COMPCS = 1;
}

void compassReadBytes(char address, char * dest, char count){
  int i;
  COMPCS = 0;
  SPICR1 |= 0x40; //Enables SPI
  address &= 0x3F; //Only last six bits are valid address
  address |= 0xC0; //First two bits on for read and multibyte
  transfer(address); //First two bits on
  for(i = 0; i < count; i++){
    dest[i] = transfer(0x00); //Read into destination array
  }
  SPICR1 &= 0xBF; //Disables SPI
  COMPCS = 1;
}

void readMag(){
  char temp[6];
  compassReadBytes(OUT_X_L_M, temp, 6);
  mx = (temp[1] << 8) | temp[0];
  my = (temp[3] << 8) | temp[2];
  mz = (temp[5] << 8) | temp[4];
}

double getHeading(double xMag, double yMag){
  double heading;
  if(yMag > 0){
    heading = 90.0 - (atan(xMag/yMag) * (180/PI));
  } else if(yMag < 0){
    heading = -(atan(xMag/yMag) * (180/PI));
  } else{
    if(xMag < 0) heading = 180;
    else heading = 0;
  }
  return heading;
}
  


/*
***********************************************************************                       
 RTI interrupt service routine: RTI_ISR
************************************************************************
*/

interrupt 7 void RTI_ISR(void)
{
  	// clear RTI interrupt flagt 
  	CRGFLG = CRGFLG | 0x80; 
 

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
 

}

/*
***********************************************************************                       
  SCI interrupt service routine		 		  		
***********************************************************************
*/

interrupt 20 void SCI_ISR(void)
{
 


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