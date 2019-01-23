#include<reg52.h>
#include<stdio.h>
#include<intrins.h>    // for using _nop_() function

#define LCD_data P2 

sbit trig = P3^5;
//sbit trig1 = P3^6;
sbit echo1 = P3^2;
//sbit echo2 = P3^3;

sbit control1 = P1^0;
sbit control2 = P1^1;
sbit control3 = P1^2;

sbit Flag_A = P3^0; 
sbit Flag_B = P3^1;
sbit Flag_C = P3^4;
sbit Flag_D = P3^6;
sbit Flag_E = P3^7;
sbit Flag_F = P0^5;
sbit Flag_G = P0^6;
sbit Flag_H = P0^7;

sbit CS_BAR = P1^4;									// Chip Select for the ADC
sbit LCD_rs = P0^0;  								// LCD Register Select
sbit LCD_rw = P0^1;  								// LCD Read/Write
sbit LCD_en = P0^2;  								// LCD Enable
sbit LCD_busy = P2^7;								// LCD Busy Flag

void LCD_Init();
void LCD_DataWrite(char dat);
void LCD_CmdWrite(char cmd);
void LCD_WriteString(char * str, unsigned char len);
void LCD_Ready();
void sdelay(int delay);
void delay_ms(unsigned int delay);
void delay(int k);

 
void delay_ms(unsigned int msec)
{
	int i,j;
	for(i=0;i<msec;i++)
    for(j=0;j<1275;j++);
}

void sdelay(int delay)
{
	char d=0;
	while(delay>0)
	{
		for(d=0;d<5;d++);
		delay--;
	}
}

void LCD_Init()
{
  sdelay(100);
  LCD_CmdWrite(0x38);   	// LCD 2lines, 5*7 matrix
  LCD_CmdWrite(0x0E);			// Display ON cursor ON  Blinking off
  LCD_CmdWrite(0x01);			// Clear the LCD
  LCD_CmdWrite(0x80);			// Cursor to First line First Position
}

/**
 * FUNCTION_PURPOSE: Write Command to LCD
 * FUNCTION_INPUTS: cmd- command to be written
 * FUNCTION_OUTPUTS: none
 */
void LCD_CmdWrite(char cmd)
{
	LCD_Ready();
	LCD_data=cmd;     			// Send the command to LCD
	LCD_rs=0;         	 		// Select the Command Register by pulling LCD_rs LOW
  LCD_rw=0;          			// Select the Write Operation  by pulling RW LOW
  LCD_en=1;          			// Send a High-to-Low Pusle at Enable Pin
  sdelay(5);
  LCD_en=0;
	sdelay(5);
}

/**
 * FUNCTION_PURPOSE: Write Command to LCD
 * FUNCTION_INPUTS: dat- data to be written
 * FUNCTION_OUTPUTS: none
 */
void LCD_DataWrite( char dat)
{
	LCD_Ready();
  LCD_data=dat;	   				// Send the data to LCD
  LCD_rs=1;	   						// Select the Data Register by pulling LCD_rs HIGH
  LCD_rw=0;    	     			// Select the Write Operation by pulling RW LOW
  LCD_en=1;	   						// Send a High-to-Low Pusle at Enable Pin
  sdelay(5);
  LCD_en=0;
	sdelay(5);
}
/**
 * FUNCTION_PURPOSE: Write a string on the LCD Screen
 * FUNCTION_INPUTS: 1. str - pointer to the string to be written, 
										2. length - length of the array
 * FUNCTION_OUTPUTS: none
 */
void LCD_WriteString( char * str, unsigned char length)
{
    while(length>0)
    {
        LCD_DataWrite(*str);
        str++;
        length--;
    }
}

/**
 * FUNCTION_PURPOSE: To check if the LCD is ready to communicate
 * FUNCTION_INPUTS: void
 * FUNCTION_OUTPUTS: none
 */
void LCD_Ready()
{
	LCD_data = 0xFF;
	LCD_rs = 0;
	LCD_rw = 1;
	LCD_en = 0;
	sdelay(5);
	LCD_en = 1;
	while(LCD_busy == 1)
	{
		LCD_en = 0;
		LCD_en = 1;
	}
	LCD_en = 0;
}
 
void lcd_number(int val,unsigned int field_length)			// Function to display number 
{
	char str[5]={0,0,0,0,0};
	int i=4,j=0;
	while(val)
	{
	str[i]=val%10;
	val=val/10;
	i--;
	}
	if(field_length==-1)
		while(str[j]==0) j++;
	else
		j=5-field_length;

	if(val<0) LCD_DataWrite('-');
	for(i=j;i<5;i++)
	{
	LCD_DataWrite(48+str[i]);
	}
}

void send_pulse(void) //to generate 10 microseconds delay
{ TR0 = 1;
	trig=1;
	_nop_();_nop_();_nop_();_nop_();_nop_();	//each _nop_() generates 1u sec of delay 
	_nop_();_nop_();_nop_();_nop_();_nop_();
	trig=0;
    
} 

 
float get_range(void)
{
  float range=0;
  int timerval;
 
  send_pulse();
	
	while(!INT0); 
	while(INT0); 	
	timerval = TH0;
	timerval = (timerval << 8) + TL0;
	TH0=0x00;
	TL0=0x00;
  
  delay_ms(2);
  
  if(timerval<35000)  //actually you need to use 38000 but the sensor may not work at higher levels 
  {
   range = timerval/59;
	 range = range*0.46 + 2.76;
	 
  }
  
	return range; 
 }
 
 
void main()
{
  P2 = 0x00;
	LCD_Init();
	
	EA = 1;
	EX0 = 1;
	ET0 = 1;
	EX0 = 1;
	IE0 = 1;
	IT0 = 1;
	
	TMOD=0x09;//timer0 in 16 bit mode with gate not enabled
  TH0=0x00;
	TL0=0x00;
	
 
  echo1 = 1;
	
 while(1)
 { 
	 float range1, range2, range3, range4, range5, range6, range7, range8;
	 //LCD_CmdWrite(0x01);
	 
	  LCD_CmdWrite(0x80);
	 LCD_WriteString("A", 1);
	 
	 control1 = 0;
	 control2 = 0;
	 control3 = 0;
	 range1 = get_range();
	 //lcd_number(range1, 2);
	 
	 if (range1 < 7){
		 LCD_WriteString("Y", 1);
		 Flag_A = 1;
	 }
	 else{
		 LCD_WriteString("N", 1);
		 Flag_A = 0;
	 }
	 
	 delay_ms(20);
	 
	 LCD_CmdWrite(0x84);
	 LCD_WriteString("B", 1);
	 
	 control1 = 0;
	 control2 = 0;
	 control3 = 1;
	 range2 = get_range();
	 //lcd_number(range2, 2);
	 
	 if (range2 < 7){
		 LCD_WriteString("Y", 1);
		 Flag_B = 1;
	 }
	 else{
		 LCD_WriteString("N", 1);
		 Flag_B = 0;
	 }
	 
	 delay_ms(20);
	 
	 LCD_CmdWrite(0x88);
	 LCD_WriteString("C", 1);
	 
	 control1 = 0;
	 control2 = 1;
	 control3 = 0;
	 range3 = get_range();
	 //lcd_number(range3, 2);
	 
	 if (range3 < 7){
		 LCD_WriteString("Y", 1);
		 Flag_C = 1;
	 }
	 else{
		 LCD_WriteString("N", 1);
		 Flag_C = 0;
	 }
	 
	 delay_ms(20);
	 
	 LCD_CmdWrite(0x8C);
	 LCD_WriteString("D", 1);
	 
	 control1 = 0;
	 control2 = 1;
	 control3 = 1;
	 range4 = get_range();
	 //lcd_number(range4, 2);
	 
	 if (range4 < 7){
		 LCD_WriteString("Y", 1);
		 Flag_D = 1;
	 }
	 else{
		 LCD_WriteString("N", 1);
		 Flag_D = 0;
	 }
	 
	 LCD_CmdWrite(0xC0);
	 LCD_WriteString("E", 1);
	 
	 control3 = 1;
	 control1 = 0;
	 control2 = 0;
	 range5 = get_range();
	 //lcd_number(range5, 2);
	 
	 if (range5 < 7){
		 LCD_WriteString("Y", 1);
			Flag_E = 1;
	 }
	 else{
		 LCD_WriteString("N", 1);
		 Flag_E = 0;
	 }
	 
	 delay_ms(20);
	 
	 LCD_CmdWrite(0xC4);
	 LCD_WriteString("F", 1);
	 
	 control1 = 1;
	 control2 = 0;
	 control3 = 1;
	 range6 = get_range();
	 //lcd_number(range6, 2);
	 
	 if (range6 < 7){
		 LCD_WriteString("Y", 1);
		 Flag_F = 1;
	 }
	 else{
		 LCD_WriteString("N", 1);
		 Flag_F = 0;
	 }
	 
	 delay_ms(20);
	 
	 LCD_CmdWrite(0xC8);
	 LCD_WriteString("G", 1);
	 
	 control1 = 1;
	 control2 = 1;
	 control3 = 0;
	 range7 = get_range();
	 //lcd_number(range7, 2);
	 
	 if (range7 < 7){
		 LCD_WriteString("Y", 1);
		 Flag_G = 1;
	 }
	 else{
		 LCD_WriteString("N", 1);
		 Flag_G = 0;
	 }
	 
	 delay_ms(20);
	 
	 LCD_CmdWrite(0xCC);
	 LCD_WriteString("H", 1);
	 
	 control1 = 1;
	 control2 = 1;
	 control3 = 1;
	 range8 = get_range();
	 //lcd_number(range8, 2);
	 
	 if (range8 < 7){
		 LCD_WriteString("Y", 1);
		 Flag_H = 1;
	 }
	 else{
		 LCD_WriteString("N", 1);
		 Flag_H = 0;
	 }
 }
} 