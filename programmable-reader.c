/**
* Microchip's PIC based programmable IR Reader/Trigger.
*
* To use,
* 1. Change the settings and header file as per your device.
* 2. Add your logic in doStuff()
* 3. Compile using MPLab-C18 compiler and burn to your device.
* 4. Connect a TSOP IR Reciever to your device's CCP pin.
* 5. Connect a read-complete indicator led at  PORTC -> Pin 4 or change the pin.
* 6. Follow the instructions on main page.
*/

#pragma config OSC = HS, OSCS = OFF
#pragma config PWRT = OFF, BOR = ON, BORV = 42
#pragma config WDT = OFF
#pragma config DEBUG = OFF, LVP = OFF, STVR = OFF

// Change as per your device
#include <P18F458.h>
#include <string.h>

void Delay(unsigned int);
void remote(void);
void training(void);

// Increase this value if some other buttons are triggering the action as well.
unsigned char max_code_length = 50;

unsigned char changed = 0;
unsigned int new_data = 0x0;
unsigned int current_data = 0x0;
unsigned char flag = 0;
unsigned char digit = 0;
unsigned char a[max_code_length];
unsigned char temp[max_code_length];
unsigned char count = 0;
unsigned char ton = 0;
unsigned char IntOff=0;

// Mark this as an an Assembly routine lable: chk_isr 
#pragma interrupt chk_isr
void chk_isr(void)
{
	if((PIR1bits.CCP1IF==1)&&(ton==0))
		remote();
	else if((PIR1bits.CCP1IF==1)&&(ton==1))
		training();
}

// The following is burnt at the interrupt vector table (IVT)
#pragma code Interrupted = 0x0008
void Interrupted(void)
{

	// Since the IVT space is small we keep our interrupt service routine elsewhere
	// and directly give an assembly reference to it. Only 1 line added in IVT block.
	_asm
	GOTO chk_isr
	_endasm
}
#pragma code


void main(void)
{
  
  // Initialize timer and Capture/Compare/PWM (CCP) module
  CCP1CON = 0X05;
  T3CON = 0X0;
  T1CON = 0X0;

  // Enable Interrupts
  PIE1bits.CCP1IE=1;
  INTCONbits.PEIE=1;
  INTCONbits.GIE=1;
  PIR1bits.CCP1IF=0;

  // The Read complete LED is PORTC -> Pin 4
  TRISCbits.TRISC4 = 0;
  PORTCbits.RC4 = 0;		
  
  
  ton=1;			// intitiate traning
  while(digit!=max_code_length); // wait till training is complete
  ton=0;			// switch off training
  
  digit=0;
  
  memset (temp,0,max_code_length);
  
  while(1)
  {	
		if((strcmp(a,temp))==0)
		{
			doStuff();
			digit=0;
			memset (temp,0,max_code_length);
     	}
		else if(digit==max_code_length)
		{
			digit=0;
			memset (temp,0,max_code_length);
		}
	
  }
}
	
void training(void)
{
		int i,k=0;
		if(flag==0)
		{
			TMR1H=0;
			TMR1L=0;
			T1CONbits.TMR1ON=1;
			CCPR1H=0;
			CCPR1L=0;
			flag=1;
		}
		else
		{
			T1CONbits.TMR1ON=0;
			temp[digit++] = CCPR1H;
			TMR1H=0;
			TMR1L=0;
			T1CONbits.TMR1ON=1;
			CCPR1H=0;
			CCPR1L=0;
		}
		if(digit==max_code_length)
		{
			T1CONbits.TMR1ON=0;
			flag=0;
			memcpy ( a,temp,max_code_length);
			for(i=0;i<max_code_length;i++)
				if(a[i]!=temp[i])
					k++;
			if(k==0)
				PORTCbits.RC4 = 1;	// Indicate Training complete
			TMR1H=0;
			TMR1L=0;
			CCPR1H=0;
			CCPR1L=0;
			ton=0;
			memset (temp,0,max_code_length);
		}
	PIR1bits.CCP1IF=0;
}
			


void remote(void)
{
 	if(flag==0)
	{
		TMR1H=0;
		TMR1L=0;
		T1CONbits.TMR1ON=1;
		CCPR1H=0;
		CCPR1L=0;
		flag=1;
	}
		else
		{
			T1CONbits.TMR1ON=0;
			temp[digit++] = CCPR1H;
			TMR1H=0;
			TMR1L=0;
			T1CONbits.TMR1ON=1;
			CCPR1H=0;
			CCPR1L=0;
		}
		if(digit==max_code_length)
		{
			T1CONbits.TMR1ON=0;
			flag=0;
			TMR1H=0;
			TMR1L=0;
			CCPR1H=0;
			CCPR1L=0;
		}
			PIR1bits.CCP1IF=0;
}		

// There is no inbuilt delay() here so, writing our own.
// 165 was for crystal ocillator(XTAL) rated 48MHz
void Delay(unsigned int itime)
{
		unsigned int i;
		unsigned char j;
		for(i=0;i<itime;i++)
			for(j=0;j<165;j++);
}

void doStuff() {
	// do whatever you want here
}
