///#define NORECEIVE
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
#include <math.h>
#include <float.h>

/* All functions after main should be initialized here */
char inchar(void);
void outchar(char x);
void Conversion(void);
void get_Data(void);
void star_Calculate(void);

/* Stepper Motor Utility Functions */
void stepMotor(void);
void delay(void);
void bigDelay(void);
void setStepperDir(char dir);
void setStep(char stepSize);
void stepMotorMult(int steps);
void stepDegrees(double degrees);
void stepToDegree(double destination);

/* SPI Utility Functions */
void transmit(char x);
char receive(void);
char transfer(char x);

/* Compass Utility Functions */
char compassReadByte(char x);
void compassWriteByte(char address, char data);
void compassReadBytes(char address, char * dest, char count);
void readMag(void);
void getHeading(double xMag, double yMag);
double averageHeadings(int count);


/* Transmission variable declarations */
char Received = 0;
char Receive_Check = 0;
char Lat_Sgn = 0;
char Lat0 = 0;
char Lat1 = 0;
char Lat2 = 0;
char Lat3 = 0;
char Long_Sgn = 0;
char Long0 = 0;
char Long1 = 0;
char Long2 = 0;
char Long3 = 0;
char Long4 = 0;
char Month0 = 0;
char Month1 = 0;
char Day0 = 0;
char Day1 = 0;
char Year0 = 0;
char Year1 = 0;
char Year2 = 0;
char Year3 = 0;
char Hours0 = 0;
char Hours1 = 0;
char Min0 = 0;
char Min1 = 0;
char Star = 0;
char Junk = 0;
int Lat_Found = 0;
int Long_Found = 0;
int Month_Found = 0;
int Day_Found = 0;
int Year_Found = 0;
int Hours_Found = 0;
int Min_Found = 0;
int Star_Found = 0;
int Receive_Complete = 0;


// Star Algorithm
#define PI 3.14159265
#define RAD2DEG 180.0/PI
#define DEG2RAD PI/180.0
#define HRS2DEG 15.0

int i;
double LAT = 0;
double LONG = 0;
int year = 0;
int day = 0;
int month = 0;
int hour = 0;
int minute = 0;

double RA = 0; //hours
double DEC = 0; //degrees
  
double UTC;
double JDays; 
int yearsSince2000;
double GMSTHours;
double LHA;
double ALT;
double AZ;
  
int daysInMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
double RA_hours[8] = {5.92, 6.40, 3.79, 2.53, 5.24, 6.75, 2.51, 18.62};
double DEC_degrees[8] = {7.41, -52.70, 24.12, 89.26, -8.20, -16.72, 14.83, 38.78};

// Main Program Flags
int new_Data = 0;
int star_Calculate_Complete = 0;
int azimuth_Complete = 0;
int altitude_Complete = 0;		  			 		       

// Timing Flags
int mincnt = 0;
int minute_flag = 0;
int ten_min = 0;
int ten_min_flag = 0;

// Servo Calculation Variables
#define R_90 30
#define Z_00 54
double adjusted_ALT;
int final_ALT;

// Stepper Motor Status Variables
char currentStepSize = 0;
double currentPosition = 0;

// Compass Calculation Variables
int mx; int my; int mz;
char check;
double heading;

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
#define LINE1 0x80	// LCD line 1 cursor position
#define LINE2 0xC0	// LCD line 2 cursor position

/* STEPPER MOTOR CONTROL PINS */
#define MOTOREN PTAD_PTAD7
#define MOTORMS1 PTAD_PTAD6
#define MOTORMS2 PTAD_PTAD5
#define MOTORRST PTAD_PTAD4
#define MOTORSLP PTT_PTT0
#define MOTORDIR PTM_PTM1
#define MOTORSTEP PTM_PTM0

/* STEPPER MOTOR STEPS */
#define FULLSTEP 0
#define HALFSTEP 1
#define QUARTERSTEP 2
#define EIGHTHSTEP 3

/* STEPPER MOTOR DIRECTION */
#define CW 0
#define CCW 1
#define TOGGLEDIR 2

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
#define WHOAMICHECKSUM 0x49

/* GPS PINS */
#define GPSEN PTT_PTT5
#define GPSTX PTT_PTT6
#define GPSRX PTT_PTT7

	 	   		
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
  PORTB  =  0x00; //assert DTR pin on COM port

/* Initialize peripherals */

/* 
   Initialize TIM Ch 7 (TC7) for periodic interrupts every 10.0 ms  
    - Enable timer subsystem                         
    - Set channel 7 for output compare
    - Set appropriate pre-scale factor and enable counter reset after OC7
    - Set up channel 7 to generate 10 ms interrupt rate
    - Initially disable TIM Ch 7 interrupts	 	   			 		  			 		  		
*/

  TSCR1 = 0x00;
  TIOS = 0x80;
  TSCR2 = 0x0C; //prescale is 16
  TC7 = 15000;
  TIE_C7I = 0;
  
  /* Intialize the PWM to drive the servo */
  MODRR = 0x02; //PT1 used as output
  PWME = 0x00;  //off initially
  PWMPOL = 0xFF; //Active low polarity
  PWMCTL = 0x08; // 8-bit
  PWMCAE = 0; // left-aligned
  PWMPER1 = 0xFF; // max period
  PWMDTY1 = 0;   // initial zero duty cycle
  PWMSCLA = 59;
  PWMCLK = 0x02; //Clock SA for ch 1
  PWMPRCLK = 0x03; //Clock A =  3 MHz
  DDRT_DDRT0 = 1;
  
  // Data Direction Register and set for XBEE Reset Pin
  DDRAD_DDRAD2 = 1;
  PTAD_PTAD2 = 0; //XBEE off initially
  
  // Initialize Data Direction Registers for Stepper Motor
  DDRAD_DDRAD4 = 1;  // Motor Reset
  DDRAD_DDRAD5 = 1;  // Motor MS2
  DDRAD_DDRAD6 = 1;  // Motor MS1
  DDRAD_DDRAD7 = 1;  // Motor Enable
  DDRM_DDRM1 = 1;    // Motor Dir
  DDRM_DDRM0 = 1;    // Motor Step
  DDRT_DDRT0 = 1;    // Motor Sleep

  // Initial Pin Values for Stepper Motor
  MOTOREN = 0;
  setStep(QUARTERSTEP);
  MOTORRST = 1;
  MOTORSLP = 0; //Motor is originally asleep
  setStepperDir(CW);
  MOTORSTEP = 0;

  // SPI Initialization
  SPICR1 = 0x1C; //master, no interrupt, idle high, sample at even edges, off initially
  SPICR2 = 0; //normal, non-bidirection
  SPIBR = 0x10; //Baud rate to 6.0 Megabits/second
  
  // GPS Initialization
  DDRT_DDRT5 = 1;
  DDRT_DDRT6 = 1;
  DDRT_DDRT7 = 0;
  GPSEN = 0;
  
  // Compass Initialization
  DDRT_DDRT2 = 1;
  COMPCS = 1;
  compassWriteByte(CTRL_REG0_XM, 0x00);
  compassWriteByte(CTRL_REG1_XM, 0x00);
  compassWriteByte(CTRL_REG5_XM, 0xE0);
  compassWriteByte(CTRL_REG6_XM, 0x00);
  compassWriteByte(CTRL_REG7_XM, 0x00);
  compassWriteByte(CTRL_REG4_XM, 0x04);
  compassWriteByte(INT_CTRL_REG_M, 0x09);
  
  // Get initial compass heading
  if(compassReadByte(WHO_AM_I_XM)== WHOAMICHECKSUM){
    bigDelay();
    bigDelay();
    bigDelay();
    bigDelay();
    currentPosition = averageHeadings(1000);
    bigDelay();
    bigDelay();
    bigDelay();
  
    // Move stepper to position
    stepToDegree(0);
  }
  
  // Turn things back on
  PWME = 0x02; //PWM
  PTAD_PTAD2 = 1; //XBEE
  TSCR1 = 0x80; //TIM
            
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
  
/* < start of your main loop > */
      #ifndef NORECEIVE
      get_Data();
      #endif
      
      #ifdef NORECEIVE
      new_Data = 0;
      star_Calculate_Complete = 1;
      azimuth_Complete = 0;
      altitude_Complete = 0;
      #endif
     
      if(new_Data == 1)
      {
         new_Data = 0;
         star_Calculate_Complete = 0;
         azimuth_Complete = 0;
         altitude_Complete = 0;
         stepToDegree(0);
      }
      
      if(!star_Calculate_Complete&&Receive_Complete)
      {
         star_Calculate();
         //Change PWM Duty Cycle Here (I think...)
         TIE_C7I = 1;
      }
              
      #ifdef NORECEIVE        
      ALT = 40.87;
      AZ = 200;
      #endif
      
      if(star_Calculate_Complete && !azimuth_Complete) 
      {
         // move azimuth with stepper code
         stepToDegree(AZ);
         azimuth_Complete = 1; // put this in azimuth function
      }
      
      if(star_Calculate_Complete && azimuth_Complete && !altitude_Complete) 
      {
        adjusted_ALT = Z_00 - (ALT * (Z_00 - R_90 + 1) / 90.0); // get value for pwm
        final_ALT = floor(adjusted_ALT + 0.5); //rounding 
        PWMDTY1 = final_ALT;
        altitude_Complete = 1;
      }
      
      if(ten_min_flag)
      {
         ten_min_flag = 0;
         
        // Update time
        // Update minute
        minute += 10;
        // Adjust minute if necessary
        if (minute >= 60)
        {
          minute = minute - 60;
          hour++;  
        }
        // Adjust hour if necessary
        if (hour == 24)
        {
          hour = 0;
          day++;
        }
        // Adjust day and month if necessary
        if (day > daysInMonth[month])
        {
          day = 1;
          month++;
        }
        // Adjust year if necessary 
        if (month > 12) 
        {
          month = 1;
          year++; // HAPPY NEW YEAR!
        }

         new_Data = 1;
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
 	 
  mincnt++;
  if (mincnt == 6000)
  {
     mincnt = 0;
     minute_flag = 1;
     ten_min++;
     if(ten_min == 10)
     {
        ten_min = 0;
        ten_min_flag = 1;
     }
  }
  
  

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
Receive Data Function
***********************************************************************
*/
void get_Data(void)
{
     // Receive Latitude
     while(Received != 'p')
     {
        Received = inchar();
     }
     Receive_Check = inchar();
     if(Receive_Check != Lat_Sgn)
     {
        Lat_Sgn = Receive_Check;
     }
     Receive_Check = inchar();
     if(Receive_Check != Lat0)
     {
        Lat0= Receive_Check;
     }
     Receive_Check = inchar();
     if(Receive_Check != Lat1)
     {
        Lat1= Receive_Check;
     }
     Receive_Check = inchar();
     if(Receive_Check != Lat2)
     {
        Lat2= Receive_Check;
     }
     Receive_Check = inchar();
     if(Receive_Check != Lat3)
     {
        Lat3= Receive_Check;
     }
     Lat_Found = 1;
     
     //Receive Longitude
     while(Received != 'o')
     {
        Received = inchar();
     }
     Receive_Check = inchar();
     if(Receive_Check != Long_Sgn)
     {
        Long_Sgn= Receive_Check;
     }
     Receive_Check = inchar();
     if(Receive_Check != Long0)
     {
        Long0= Receive_Check;
     }
     Receive_Check = inchar();
     if(Receive_Check != Long1)
     {
        Long1= Receive_Check;
     }
     Receive_Check = inchar();
     if(Receive_Check != Long2)
     {
        Long2= Receive_Check;
     }
     Receive_Check = inchar();
     if(Receive_Check != Long3)
     {
        Long3= Receive_Check;
     }
     Receive_Check = inchar();
     if(Receive_Check != Long4)
     {
        Long4= Receive_Check;
     }
     Long_Found = 1;
        
         
     //Receive Month
     while(Received!= 't')
     {
        Received = inchar();
     }
     Receive_Check = inchar();
     if(Receive_Check != Month0)
     {
        Month0= Receive_Check;
     }
     Receive_Check = inchar();
     if(Receive_Check != Month1)
     {
        Month1= Receive_Check;
     }
     Month_Found = 1;

     //Receive Day
     while(Received!='d')
     {
        Received = inchar();
     }
     Receive_Check = inchar();
     if(Receive_Check != Day0)
     {
        Day0= Receive_Check;
     }
     Receive_Check = inchar();
     if(Receive_Check != Day1)
     {  
        Day1= Receive_Check;
     }
     Day_Found = 1;

     //Receive Year
     while(Received != 'y')
     {
        Received = inchar();
     }
     Receive_Check = inchar();
     if(Receive_Check != Year0)
     {
        Year0= Receive_Check;
     }
     Receive_Check = inchar();
     if(Receive_Check != Year1)
     {
        Year1= Receive_Check;
     }
     Receive_Check = inchar();
     if(Receive_Check != Year2)
     {
        Year2= Receive_Check;
     }
     Receive_Check = inchar();
     if(Receive_Check != Year3)
     {
        Year3= Receive_Check;
     }
     Year_Found = 1;
 
     //Receive Hours
     while(Received != 'h')
     {
        Received = inchar();
     }
     Receive_Check = inchar();
     if(Receive_Check != Hours0)
     {
        Hours0= Receive_Check;
     }
     Receive_Check = inchar();
     if(Receive_Check != Hours1)
     {
        Hours1= Receive_Check;
     }
     Hours_Found = 1;
     
     //Receive Minute 
     while(Received != 'm')
     {
        Received = inchar();
     }          
     Receive_Check = inchar();
     if(Receive_Check != Min0)
     {
        Min0= Receive_Check;
     }
     Receive_Check = inchar();
     if(Receive_Check != Min1)
     {
        Min1= Receive_Check;
     }
     Min_Found = 1;
     
     //Receive Star 
     while(Received != 's')
     {
        Received = inchar();
     }
     Receive_Check = inchar();
     if(Receive_Check != Star)
     {
        Star= Receive_Check;
        new_Data = 1;
     }
     Star_Found = 1;
 
     //else
     //{
     //   Junk = 1;
     //}
     
     if(Lat_Found&&Long_Found&&Month_Found&&Day_Found&&Month_Found&&Year_Found&&
        Hours_Found&&Min_Found&&Star_Found)
     {
        Conversion();
        Receive_Complete = 1;
        Junk = 0;
     } 
}

/*
***********************************************************************
Conversion from character to floating point
***********************************************************************
*/
void Conversion()
{
   LAT = (10.0*(int)(Lat0-'0') + (int)(Lat1-'0') + 
   (int)(Lat2-'0')/10.0 + (int)(Lat3-'0')/100.0);
    
   if(Lat_Sgn == '-')
   {
      LAT = LAT*-1.0;
   }
   
   LONG = (100.0*(int)(Long0-'0') + 10.0*(int)(Long1-'0') + (int)(Long2-'0') +
   (int)(Long3-'0')/10.0 + (int)(Long4-'0')/100.0);

   if(Long_Sgn == '-')
   {
      LONG = LONG*-1.0; 
   }
   
   month = 10*(int)(Month0-'0') + (int)(Month1-'0');
   
   day = 10*(int)(Day0-'0') + (int)(Day1-'0');
   
   year = (1000*(int)(Year0-'0') + 100*(int)(Year1-'0') + 10*(int)(Year2-'0')
    + (int)(Year3-'0'));
   
   hour = 10*(int)(Hours0-'0') + (int)(Hours1-'0');
   
   // Conversion to UTC from EST
    hour = hour + 4;
    if(hour > 23){
      hour-= 24;
      day++;
    }
   
   minute = 10*(int)(Min0-'0') + (int)(Min1-'0'); 
   
}
/*
***********************************************************************
Star Algorithm
***********************************************************************
*/
void star_Calculate()
{
 if(year < 2000){
    //terminate everything
  }
  
  // Get star Right Ascension and Left Declination
  RA = RA_hours[(int)(Star-'0')];
  DEC = DEC_degrees[(int)(Star-'0')]; 
   
  // Set UTC
  UTC = hour + minute/60.0;

  // Calculate days from J0
  JDays = 2451545.0; //Days from J0 to J2000
  yearsSince2000 = year-2000;
  JDays += (365*yearsSince2000); // Nunber of years since 2000
  for(i = 0; i < yearsSince2000; i++){
    if(i % 4 == 0) JDays++; // Accounting for leap years
  }

  for(i = 0; i < month-1; i++){
    JDays += daysInMonth[i]; //Accounting to the current month
  }

  JDays += (day-1); //Accounting for days
  JDays-= 0.5; // If UTC in morning, take us to midnight that day
  

  // Compute Greenwhich Mean Sidereal Time

  GMSTHours = 6.656306 + 0.0657098242*(JDays-2445700.5) + 1.0027379093*UTC;
  // In hours, so bring into 0 to 24 range
  while (GMSTHours >= 24){
    GMSTHours -= 24;
  }

  // Compute HA

  LHA = GMSTHours - RA + (LONG/HRS2DEG);

  // Get HA in 0-24 range

  while (LHA > 24 || LHA < 0)
  {
     if (LHA < 0) LHA = LHA + 24;
     else LHA = LHA - 24;
  }

  // Compute Altitude
  ALT = sin(LAT * DEG2RAD) * sin(DEC * DEG2RAD) + 
    cos(LAT * DEG2RAD) * cos(DEC * DEG2RAD) * cos(LHA * HRS2DEG * DEG2RAD);
  ALT = asin(ALT) * RAD2DEG;


  // Compute Azimuth
  AZ = (sin(DEC * DEG2RAD) - sin(LAT * DEG2RAD) * sin(ALT * DEG2RAD))/
    (cos(LAT * DEG2RAD) * cos(ALT * DEG2RAD));
  AZ = acos(AZ) * RAD2DEG;

  if(sin(LHA * HRS2DEG * DEG2RAD) * RAD2DEG >= 0.0) AZ = 360.0 - AZ;  
  star_Calculate_Complete = 1;
}

/*
***********************************************************************
STEPPER UTILITY FUNCTIONS
***********************************************************************
*/

void delay(void){
  int count;
  for(count = 0; count < 10000; count++){}
}

void bigDelay(void){
  int outer; int inner;
  for(outer = 0; outer < 1000; outer++){
    for(inner = 0; inner < 500; inner++){
    }
  }
}

void stepMotor(void){ // Pulses the step pin
  /* WARNING: Assumes the motor is already awake. */
  delay();
  MOTORSTEP = 1;
  delay();
  MOTORSTEP = 0;
  delay();
}

void setStepperDir(char dir){
  if(dir == CW || dir == CCW) MOTORDIR = dir;
  else MOTORDIR ^= 1; //Toggles direction if dir invalid
  delay();
}

void setStep(char stepSize){
  /*        MS1      MS2
  FULL       0        0
  HALF       1        0
  QUARTER    0        1
  EIGHTH     1        1
  */
  currentStepSize = stepSize;
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
  int count;
  MOTORSLP = 1; // Wakes up the motor
  for(count = 0; count < steps; count++){
    stepMotor();
  }
  MOTORSLP = 0; //Puts the motor back to sleep
}

void stepDegrees(double degrees){
  double degreesPerStep; int steps; 
  
  // A full step moves the motor 1.8 degrees
  if(currentStepSize == FULLSTEP) degreesPerStep = 1.8;
  else if(currentStepSize == HALFSTEP) degreesPerStep = 0.9;
  else if(currentStepSize == QUARTERSTEP) degreesPerStep = 0.45;
  else degreesPerStep = 0.225;
  steps = degrees/degreesPerStep;
  stepMotorMult(steps);
  
  // Update current Position after rotation
  if(MOTORDIR == CW) currentPosition += (steps * degreesPerStep);
  else currentPosition -= (steps * degreesPerStep);
  
  // Brings current position into the 0 <= degrees < 360.0 range
  while(currentPosition >= 360.0 || currentPosition < 0.0){
    currentPosition = currentPosition >= 360.0 ? currentPosition - 360.0:currentPosition + 360.0;
  }
}

void stepToDegree(double destination){
  /* WARNING: ASSUMES THAT A CLOCKWISE OR COUNTERCLOCKWISE ROTATION THROUGH 0
   * WILL NEVER OCCUR */
   
  // Calculate difference in angle
  double difference;
  difference = destination - currentPosition; // Calculates degrees to move
  
  // Determine direction to move
  if(difference > 0.0){
    setStepperDir(CW);
  } else{
    setStepperDir(CCW);
  }
  difference = fabs(difference);
  if(difference > 180.0){
    difference = 360.0 - difference;
    setStepperDir(TOGGLEDIR);
  }
  
  // Moves required degrees
  stepDegrees(difference);
  
}

/*
**********************************************************************
SPI UTILITY FUNCTIONS
**********************************************************************
*/

void transmit(char x){
  while(SPISR_SPTEF == 0);
  SPIDR = x;
}

char receive(void){
  while(SPISR_SPIF == 0);
  return SPIDR;
}

char transfer(char x){
  transmit(x);
  return receive();
}

/*
**********************************************************************
COMPASS UTILITY FUNCTIONS
**********************************************************************
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
  int compassByte;
  COMPCS = 0;
  SPICR1 |= 0x40; //Enables SPI
  address &= 0x3F; //Only last six bits are valid address
  address |= 0xC0; //First two bits on for read and multibyte
  transfer(address); //First two bits on
  for(compassByte = 0; compassByte < count; compassByte++){
    dest[compassByte] = transfer(0x00); //Read into destination array
  }
  SPICR1 &= 0xBF; //Disables SPI
  COMPCS = 1;
}

void readMag(void){
  char temp[6];
  compassReadBytes(OUT_X_L_M, temp, 6);
  mx = (temp[1] << 8) | temp[0];
  my = (temp[3] << 8) | temp[2];
  mz = (temp[5] << 8) | temp[4];
}

void getHeading(double xMag, double yMag){
  if(yMag > 0){
    heading = 90.0 - (atan(xMag/yMag) * (180/PI));
  } else if(yMag < 0){
    heading = 270-(atan(xMag/yMag) * (180/PI));
  } else{
    if(xMag < 0) heading = 180;
    else heading = 0;
  }
  heading += 90;
  while(heading >= 360 || heading < 0){
    if(heading >= 360) heading -= 360;
    else heading += 360;
  }
}

double averageHeadings(int count){
  int index;
  double headingSum = 0;
  for(index = 0; index < count; index++){
    readMag();
    getHeading((double) mx, (double) my);
    headingSum += heading;
  }
  return headingSum / (double) count;
}

/*
***********************************************************************                             
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