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

/* All functions after main should be initialized here */
char inchar(void);
void outchar(char x);

/* Stepper Motor Utility Functions */
void stepMotor();
void delay();
void setDir(char dir);
void setStep(char stepSize);
void stepMotorMult(int steps);


/* Variable declarations */
int flag = 0;
   	   			 		  			 		       

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

/* MOTOR CONTROL PINS */
#define MOTOREN PTAD_PTAD7
#define MOTORMS1 PTAD_PTAD6
#define MOTORMS2 PTAD_PTAD5
#define MOTORRST PTAD_PTAD4
#define MOTORSLP PTT_PTT0
#define MOTORDIR PTM_PTM1
#define MOTORSTEP PTM_PTM0

/* MOTOR STEPS */
#define FULLSTEP 0
#define HALFSTEP 1
#define QUARTERSTEP 2
#define EIGHTHSTEP 3

/* MOTOR DIRECTION */
#define CW 1
#define CCW 0
#define TOGGLEDIR 2

	 	   		
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


// Initialize Data Direction Registers for Motors
DDRAD_DDRAD4 = 1;  // Motor Reset
DDRAD_DDRAD5 = 1;  // Motor MS2
DDRAD_DDRAD6 = 1;  // Motor MS1
DDRAD_DDRAD7 = 1;  // Motor Enable
DDRM_DDRM1 = 1;    // Motor Dir
DDRM_DDRM0 = 1;    // Motor Step
DDRT_DDRT0 = 1;    // Motor Sleep

MOTOREN = 0;
setStep(FULLSTEP);
MOTORRST = 1;
MOTORSLP = 1;
setDir(CW);
MOTORSTEP = 0;   
/* Initialize interrupts */
	      
	      
}

	 		  			 		  		
/*	 		  			 		  		
***********************************************************************
Main
***********************************************************************
*/
void main(void) {
  	DisableInterrupts
	initializations(); 		  			 		  		
	EnableInterrupts;

 for(;;) {
  stepMotorMult(100); 
  
   } /* loop forever */
   
}   /* do not leave main */

/*
******************
STEPPER UTILITY FUNCTIONS
******************
*/
void delay(){
  int i;
  for(i = 0; i < 10000; i++){}
}

void stepMotor(){
  delay();
  MOTORSTEP = 1;
  delay();
  MOTORSTEP = 0;
  delay();
}

void setDir(char dir){
  if(dir == CW || dir == CCW) MOTORDIR = dir;
  else MOTORDIR ^= 1; //Toggles direction if dir invalid
  delay();
}

void setStep(char stepSize){
  if(stepSize == FULLSTEP || stepSize == HALFSTEP){
    MOTORMS2 = 0;
  } else{
    MOTORMS2 = 1;
  }
  
  if(stepSize == FULLSTEP || stepSize == QUARTERSTEP){
    MOTORMS1 = 0;
  } else{
    MOTORMS1 = 1;
  }
  delay();
}

void stepMotorMult(int steps){
  int i;
  for(i = 0; i < steps; i++){
    stepMotor();
  }
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