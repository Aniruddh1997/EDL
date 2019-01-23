#include<reg52.h>
#include<stdio.h>
#include<intrins.h>    // for using _nop_() function

#define LCD_data P2 

sbit trig = P3^5;
sbit echo = P3^2;

sbit entry_control = P1^2;

int entry_flag = 0;
int exit_flag = 0;


void delay(int k);
void delay_ms(unsigned int msec);

int count = 0;
int pre_entry_state = 0;
int pre_exit_state = 0;
int entry_state = 0;
int exit_state = 0;

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

 void close()
{ int sum=0;
  do
  { //if(sum==49)
		//break;
		sum++;
    P2=0x01; //0001
    delay(50);
    P2=0x03; //0011
    delay(50);
    P2=0x02; //0010
    delay(50);
    P2=0x06; //0110
    delay(50);
    P2=0x04; //0100
    delay(50);
    P2=0x0C; //1100
    delay(50);
    P2=0x08; //1000
    delay(50);
    P2=0x09; //1001
    delay(50);
  } while(sum<3);
  
}

void close1()
{ int sum=0;
  do
  { //if(sum==49)
		//break;
		sum++;
    P0=0x01; //0001
    delay(50);
    P0=0x03; //0011
    delay(50);
    P0=0x02; //0010
    delay(50);
    P0=0x06; //0110
    delay(50);
    P0=0x04; //0100
    delay(50);
    P0=0x0C; //1100
    delay(50);
    P0=0x08; //1000
    delay(50);
    P0=0x09; //1001
    delay(50);
  } while(sum<3);
  
}

void rotate(){
	int sum=0;
  do
  { //if(sum==49)
		//break;
		sum++;
    P2=0x09; //0001
    delay(50);
    P2=0x08; //0011
    delay(50);
    P2=0x0C; //0010
    delay(50);
    P2=0x04; //0110
    delay(50);
    P2=0x06; //0100
    delay(50);
    P2=0x02; //1100
    delay(50);
    P2=0x03; //1000
    delay(50);
    P2=0x01; //1001
    delay(50);
  } while(sum<3);
}

void rotate1(){
	int sum=0;
  do
  { //if(sum==49)
		//break;
		sum++;
    P0=0x09; //0001
    delay(50);
    P0=0x08; //0011
    delay(50);
    P0=0x0C; //0010
    delay(50);
    P0=0x04; //0110
    delay(50);
    P0=0x06; //0100
    delay(50);
    P0=0x02; //1100
    delay(50);
    P0=0x03; //1000
    delay(50);
    P0=0x01; //1001
    delay(50);
  } while(sum<3);
}


void delay(int k)
{
  int i,j;
  for(i=0;i<k;i++)
  {
    for(j=0;j<100;j++)
    {}
  }
}

void delay_ms(unsigned int msec)
{
	int i,j;
	for(i=0;i<msec;i++)
    for(j=0;j<1275;j++);
}
 
void main()
{
  P2 = 0x00;
	P0 = 0x00;
	
	ET0 = 1;
	EX0 = 1;
  EA = 1;
	IE0 = 1;
	IT0 = 1;

	
	TMOD=0x09;//timer0 in 16 bit mode with gate not enabled
  TH0=0x00;
	TL0=0x00;
 
  echo = 1;	
  
 while(1)
 { 
	 float range5, range6;
	 
	 //---------------------entry exit control-------------------------
	 
	 entry_control = 0;
	 
	 range5 = get_range();
	 
	 
	 pre_entry_state = entry_state;
	 if (range5 < 7){
			//LCD_WriteString("Y", 1);
			entry_state = 1;
		  entry_flag++;
			if(entry_flag == 1){
				rotate();
				delay(200);
			}
	 }
	 else{
		//LCD_WriteString("N", 1);
		 entry_state = 0;
		 entry_flag = 0;
		 if(pre_entry_state == 1) {
					count = count + 1;
					close();
			}
	 }
	 
	 delay_ms(20);
	 
	 entry_control = 1;
	
	 range6 = get_range();
	 
	 pre_exit_state = exit_state;
	 if (range6 < 7){
		 //LCD_WriteString("Y", 1);
		 exit_state = 1;
		 exit_flag++;
		 if(exit_flag == 1){
			rotate1();
			delay(200);
		 }
	 }
	 else{
		 //LCD_WriteString("N", 1);
		 exit_state = 0;
		 exit_flag = 0;
		 if(pre_exit_state == 1 ) {
					count = count - 1;
					close1();
			}
	 }
	 
	 delay_ms(20);
	 
	 //LCD_CmdWrite(0xcc);
	 delay_ms(20);
 }
}