/*------------------------------------------------------------
 	ORT Braude College of Engineering, Carmiel.
	Practical Engineering School, Department of Electronics
	Final Project by Zaidin Ilia, 2008
-------------------------------------------------------------------------------*/
#include <AT89C5131.h>
#define UP 1
#define DOWN 0
#define maximumSteps 100
#define startPosition 249	//~1msec
#define endPosition	  229	//~2-3mscec
#define LCDdata	P2
unsigned char controlArrey[maximumSteps]; //the arrey contains data for step-by-step operation
unsigned int  arreyIndex;	 //each element of the arrey refers to one step of the system
unsigned char keyPadInput;	//input from pins P0.0-P0.3
unsigned char  operationMode; //'1'-manual, '2'-automatic, '3' - programming
sbit magnet=P1^6;			//electromagnet is controlled via pin P1.6
sbit RS=P0^4;   			//LCD controlls
sbit EN=P0^5;			
//----functions declarations------------
void keyPadSetUp (void);
void PWMinitialSetUp (void);
void servoDCadjust (char, char);
void automaticRun (void);
void delay1msec(unsigned int);
void resetInitialPosition(void);
void printString2LCD (unsigned char*);
void clearLCD (void);
//----main-------------------------------
void main() 
{	keyPadSetUp();
	PWMinitialSetUp();
	clearLCD();
	printString2LCD("SYSTEM READY");
	while(1)
		if (operationMode==2)
			automaticRun ();
}
//-----bring the system to initial position
void resetInitialPosition(void)
{	while (CCAP0H<startPosition || CCAP1H<startPosition || CCAP2H<startPosition)
	{	if (CCAP0H<startPosition)
			CCAP0H++;
		if (CCAP1H<startPosition)
			CCAP1H++;
		if (CCAP2H<startPosition)	
			CCAP2H++;
		delay1msec(50);
	}
	magnet=1;
}
//----Outomatic Run. The function performs automatic operation of the system--------
void automaticRun (void)
{	while (1)
	{	resetInitialPosition(); 
		delay1msec(500);
		for (arreyIndex=0; arreyIndex<maximumSteps; arreyIndex++)
		{	if (operationMode!=2 || controlArrey[arreyIndex]==0xFF) 
				return;
			switch (controlArrey[arreyIndex])
				{	case (0x01):
						servoDCadjust(1,UP);
						break;
					case (0x04):
						servoDCadjust(1,DOWN);
						break;
					case (0x02):
						servoDCadjust(2,UP);
						break;
					case (0x05):
						servoDCadjust(2,DOWN);
						break;
					case (0x03):
						servoDCadjust(3,UP);
						break;
					case (0x06):
						servoDCadjust(3,DOWN);
						break;
					case (0x08):
						delay1msec(500);
						magnet=~magnet;
						delay1msec(500);
						break;
					case (0x09):
						delay1msec(1000);
						break;
					default:
						break;
				}
			delay1msec(50);
		}
	}
}
//----Keypad SetUp Function-----
//----The function performs preset operations for 4x4 keypad
//----The keypad is connected to ports P0.0, P0.1, P0.2, P0.3 via 74C922 (8->4 decoder).
void keyPadSetUp (void)
{	IEN0=0x81;		//enable external interrupt
	IT0=1;			//set to edge trigging
}
//----External Interrupt Function----
//----The function performs operations for the keypad during the interrupt
void keyPadInterrupt (void) interrupt 0
{	P0=0xFF;
	keyPadInput=(P0 & 0x0F);			//input from pins P0.0-P0.3
	switch (keyPadInput)
	{	case (0x00):					//key '1' in = servo1 rotate right
			servoDCadjust(1,UP);
			if (operationMode==3 && CCAP0H<startPosition && CCAP0H>endPosition)
			{	controlArrey[arreyIndex]=0x01;
				if(arreyIndex<maximumSteps)
					arreyIndex++;
			}
			break;
		case (0x04):					//key '4' in = servo1 rotate left
			servoDCadjust(1,DOWN);
			if (operationMode==3 && CCAP0H<startPosition && CCAP0H>endPosition)
			{	controlArrey[arreyIndex]=0x04;
				if(arreyIndex<maximumSteps)
					arreyIndex++; //message needed!!!
			}
			break;
		case (0x01):					//key '2' in = servo2 rotate forward
			servoDCadjust(2,UP);
			if (operationMode==3 && CCAP1H<startPosition && CCAP1H>endPosition)
			{	controlArrey[arreyIndex]=0x02;
				if(arreyIndex<maximumSteps)
					arreyIndex++;
			}
			break;
		case (0x05):					//key '5' in = servo2 rotate backward
			servoDCadjust(2,DOWN);
			if (operationMode==3 && CCAP1H<startPosition && CCAP1H>endPosition)
			{	controlArrey[arreyIndex]=0x05;
				if(arreyIndex<maximumSteps)
					arreyIndex++; //message needed!!!
			}
			break;
		case (0x02):					//key '3' in = servo3 rotate up
			servoDCadjust(3,UP);
			if (operationMode==3 && CCAP2H<startPosition && CCAP2H>endPosition)
			{	controlArrey[arreyIndex]=0x03;
				if(arreyIndex<maximumSteps)
					arreyIndex++;
			}
			break;
		case (0x06):					//key '6' in = servo3 rotate down
			servoDCadjust(3,DOWN);
			if (operationMode==3 && CCAP2H<startPosition && CCAP2H>endPosition)
			{	controlArrey[arreyIndex]=0x06;
				if(arreyIndex<maximumSteps)
					arreyIndex++;
			}
			break;
		case (0x08):					//key '7' in = toggle magnet
			magnet=~magnet;
			if (operationMode==3)
			{	controlArrey[arreyIndex]=0x08;
				if(arreyIndex<maximumSteps)
					arreyIndex++;
			}
			break;
		case (0x09):					//key '8' in = 1sec delay
			if (operationMode==3)
			{	controlArrey[arreyIndex]=0x09;
				if(arreyIndex<maximumSteps)
					arreyIndex++;
			}
			break;
		case (0x03):					//key 'A' in = manual operation
			operationMode=1;			//free manipulation, arrey is not initiated
			resetInitialPosition();
			clearLCD();
			printString2LCD("SYSTEM READY");
			break;
		case (0x07):					//key 'B' in = automatic operation
			operationMode=2;
			clearLCD();
			printString2LCD("AUTOMATIC MODE");
			break;
		case (0x0B):					//key 'C' in = programming mode
			if (operationMode!=3)		//begin programming
			{	operationMode=3;
				arreyIndex=0;			//reset the arrey to beginning
				resetInitialPosition();
				clearLCD();
				printString2LCD("PROGRAMMING MODE");
			}
			else 
			{	operationMode=1;				//return to manual mode
				controlArrey[arreyIndex]=0xFF;	//end of programm
				clearLCD();
				printString2LCD("SYSTEM READY");
			}
			break;
		default:
			break;
	}
}
//----Servos SetUp Function-----
//----The function performs preset operations for PCA moduls 0,1,2, Timer0, Timer1
void PWMinitialSetUp (void)
{	TMOD=0x12;		//Timer0_Mode2, Timer1_Mode1
	TL0=(-156);
	TH0=(-156);		//set Timer0 reload value to 156 = 78usec
	TF0=0;			//clear Timer0 overflow flag
	CMOD=0x04;		//set the PCA to Timer0 overflow mode (256 cycles x 78usec = 20msec)
	CCAPM0=0x42;	//set PCA Module0 to PWM mode (output to servo #1 via P1_3)
	CCAPM1=0x42;	//set PCA Module1 to PWM mode (output to servo #2 via P1_4)
	CCAPM2=0x42;	//set PCA Module2 to PWM mode (output to servo #3 via P1_5)
	CCAP0H=startPosition;		
	CCAP1H=startPosition;	//set initial values to 243 = Pulse Duty Cycle 1 msec
	CCAP2H=startPosition;
	TR0=0x01;		//turn Timer 0 on
	CCON=0x40;		//turn the PCA counter ON
}
//--PWM function. The function receives a number of a servo motor and a direction
//--of rotation. It changes Duty Cycle by 1 accordingly, within limits:
//-- 1msec to 2msec approximately.
void servoDCadjust (char servoNum, char direction)
{	switch (servoNum)
	{	case (1):	//adjust servo1 position
			if ((direction==1 && CCAP0H>endPosition)||(direction==0 && CCAP0H<startPosition))
				{	switch (direction)
					{	case (1):
							CCAP0H--;
							break;
						case (0):
							CCAP0H++;
							break;
						default:
							break;
					}
				}
			break;
		case (2):	//adjust servo2 position
			if ((direction==1 && CCAP1H>endPosition)||(direction==0 && CCAP1H<startPosition))
			{	switch (direction)
				{	case (1):
						CCAP1H--;
						break;
					case (0):
						CCAP1H++;
					default:
						break;
				}
			}
			break;
		case (3):	//adjust servo3 position
			if ((direction==1 && CCAP2H>endPosition)||(direction==0 && CCAP2H<startPosition))
			{	switch (direction)
				{	case (1):
						CCAP2H--;
						break;
					case (0):
						CCAP2H++ ;
						break;
					default:
						break;
				}
			}
			break;
		default:
			break;
	}
}
//-------1 msec delay x times-------
void delay1msec(unsigned int delayTime)
{	unsigned int i;
	for (i=0; i<delayTime; i++)
		{	TL1=-2000;
			TH1=(-2000)>>8; //1 timer clock = 2 usec
			TR1=1;			//start counting
			while (TF1==0);
			TF1=0;
			TR1=0;			//stop counting
		}
	/*unsigned int i,j;
	for (i=0; i<delayTime; i++)
		for (j=0; j<152; j++);	//1msec delay
	*/
}
//the function prints string of characters to LCD via P2 & controll via P0
void printString2LCD (unsigned char *a) 
{	unsigned char i;
	RS=1;				//write to LCD
	for (i=0; (a[i]!='\0'); i++)
	{	EN=1;
		LCDdata=a[i];
		delay1msec(2);
		EN=0;
	}
}
//The function clears the LCD via P2 & controll via P0
void clearLCD (void) 
{	RS=0;				//control LCD
	EN=1;
	LCDdata=0x38;		//data length 8 bit, 2 lines, large font
	delay1msec(2);
	EN=0;
	EN=1;
	LCDdata=0x0C;		//LCD on, cursor off
	delay1msec(2);
	EN=0;
	EN=1;
	delay1msec(2);
	LCDdata=0x01;		//clear LCD
	EN=0;
}
