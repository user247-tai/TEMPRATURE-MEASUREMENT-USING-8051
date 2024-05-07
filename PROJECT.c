							  //
//  TEMPRATURE MEASUREMENT PROJECT //
//

#include <REGX51.H>
#include <string.h>
#include <stdio.h>
// LCD
char LCD_buff[20];
sbit LCD_RS = P2^0;
sbit LCD_E = P2^2;
sbit LCD_RW = P2^1;
#define	LCD_DATA P3
// ADC
sbit ADC_CS = P2^3;
sbit ADC_RD = P2^4;
sbit ADC_WR = P2^5;
sbit ADC_INTR = P2^6;
#define ADC_DATA P1
// BUTTON
sbit BUTTON_C = P0^5;
sbit BUTTON_F = P0^6;
sbit BUTTON_K = P0^7;
//
void delay(unsigned long time){		   //delay 1ms
	unsigned long i, j;
	for(i = 0; i < time; i ++){
		for(j = 0; j < 20; j ++);
	}
}
void LCD_cmd(unsigned char cmd){	   //thuc hien lenh
	LCD_RW = 0;	  		//cho phep ghi
	LCD_RS = 0;	  		//ghi lenh
	LCD_DATA = cmd;	    //thuc hien lenh cmd
	//tao xung cho E
	LCD_E = 1;
	LCD_E = 0;
	//					
	delay(2);
}
void LCD_char_cp(char ch){	// in ra 1 ki tu tai vi tri hien tai
	LCD_RW = 0;			//cho phep ghi
	LCD_RS = 1;		   	//ghi du lieu
	LCD_DATA = ch;		//ghi ki tu ch
	//tao xung cho E
	LCD_E = 1;
	LCD_E = 0;
	//					
	delay(2);
}
void LCD_string_cp(char *str){	//in string
	unsigned char i =0;
	while (str[i]!=0){
		LCD_char_cp(str[i]);
		i++;
	}
}
void LCD_init(){   //khoi tao LCD
	//khoi dong lcd
	LCD_cmd(0x30);
	delay(5);
	LCD_cmd(0x30);
	delay(1);
	LCD_cmd(0x30);
	//
	LCD_cmd(0x38);		   //so dong la 2, 5x8, che do giao tiep 8bit	
	LCD_cmd(0x01);		   //clear screen
	LCD_cmd(0x0C);		   //bat hien thi, tat con tro
	LCD_cmd(0x06); 		   //che do tu dong dich con tro
}
unsigned char ADC_read(){
	unsigned char kq;
	ADC_CS = 0;	 // chon chip
	// tao xung bat dau chuyen doi
	ADC_WR = 0;
	ADC_WR = 1;
	//
	while (ADC_INTR);	 // doi den khi chuyen doi xong
	// doc gia tri sau khi chuyen doi
	ADC_RD = 0;	
	kq = ADC_DATA;
	ADC_RD = 1;	  
	//
	return kq;
}
void printv(float t){		//in voltage
 	sprintf(LCD_buff,"%0.2f", t);
	LCD_string_cp(LCD_buff);
}
void printtemp(float t){	//in nhiet do
	int i;
	sprintf(LCD_buff,"%0.2f", t);
	i = strlen(LCD_buff);
	while (i<6)
	{
		LCD_char_cp(' ');
		i++;
	}
	LCD_string_cp(LCD_buff);
}
void printa(unsigned int t){ //in ADC
	if (t/100!=0){ // Neu t >= 100
		LCD_char_cp(t/100+0x30); // In ra hang tram
		LCD_char_cp(t%100/10+0x30);	 // In ra hang chuc 
	}
	else if(t/100==0) // Neu t < 100
		if (t%100/10!=0) // Neu t > 9
			LCD_char_cp(t%100/10+0x30); //In ra hang chuc  
	LCD_char_cp(t%10+0x30); // In ra hang don vi
}

void main(){
	unsigned char adc_value;
	float t, v;
	LCD_init();
	LCD_cmd(0x80); //dua con tro ve dau hang 1
	LCD_string_cp("TEMP MEASUREMENT");
	
	LCD_cmd(0xC0); //dua con tro ve dau hang 2
	LCD_string_cp("CHOOSE TEMP UNIT");
	
	while(1){
		if(BUTTON_C==0){
			LCD_cmd(0x01); //clear screen
			LCD_string_cp("TEMP IN CELCIUS");
			delay(100);
			LCD_cmd(0x01);
			while(BUTTON_C==1 && BUTTON_F==1 && BUTTON_K ==1){
				adc_value = ADC_read();	 //doc output cua ADC
				LCD_cmd(0x80);//dua con tro ve dau hang 1
				LCD_string_cp("ADC= "); printa(adc_value);  //in adc output
				LCD_string_cp(" "); LCD_string_cp("V= ");
				v = (float)adc_value * 1.56/256;
				printv(v); //in voltage
				if (BUTTON_C==0 || BUTTON_F==0 || BUTTON_K ==0) break;	//giam delay nut bam
				LCD_cmd(0xC0);//dua con tro ve dau hang 2	
				LCD_string_cp("TEMP=");
				adc_value = ADC_read();
				t = (float)adc_value * 1.56/256 * 100;
				printtemp(t);
				LCD_cmd(0xCB);	//dua con tro den o thu 11 hang 2
				LCD_char_cp(0xDF);//in ki hieu nhiet do
				LCD_char_cp('C');
			}
		}  
		if(BUTTON_F==0){
			LCD_cmd(0x01);	//clear screen
			LCD_string_cp("TEMP IN FAHRENHEIT");
			delay(100);
			LCD_cmd(0x01);
			while(BUTTON_C==1 && BUTTON_F==1 && BUTTON_K ==1){
				adc_value = ADC_read();	 //doc output cua ADC
				LCD_cmd(0x80);//dua con tro ve dau hang 1
				LCD_string_cp("ADC= "); printa(adc_value);  //in adc output
				LCD_string_cp(" "); LCD_string_cp("V= ");
				v = (float)adc_value * 1.56/256;
				printv(v); //in voltage
				if (BUTTON_C==0 || BUTTON_F==0 || BUTTON_K ==0) break;	  //giam delay nut bam
				LCD_cmd(0xC0);	//dua con tro ve dau hang 2
				LCD_string_cp("TEMP=");
				adc_value = ADC_read();
				t = (float)adc_value * 1.56/256 * 100 * 1.8 + 32;	 //do F = do C*1.8+32
				printtemp(t);
				LCD_cmd(0xCB);	//dua con tro den o thu 11 hang 2
				LCD_char_cp(0xDF); //in ki hieu nhiet do
				LCD_char_cp('F');
			}
		}
		if(BUTTON_K==0){
			LCD_cmd(0x01); //clear screen
			LCD_string_cp("TEMP IN KELVIN");
			delay(100);
			LCD_cmd(0x01);
			while(BUTTON_C==1 && BUTTON_F==1 && BUTTON_K ==1){
				adc_value = ADC_read();	 //doc output cua ADC
				LCD_cmd(0x80);//dua con tro ve dau hang 1
				LCD_string_cp("ADC= "); printa(adc_value);  //in adc output
				LCD_string_cp(" "); LCD_string_cp("V= ");
				v = (float)adc_value * 1.56/256;
				printv(v); //in voltage
				if (BUTTON_C==0 || BUTTON_F==0 || BUTTON_K ==0) break;	 //giam delay nut bam
				LCD_cmd(0xC0);//dua con tro ve dau hang 2	
				LCD_string_cp("TEMP=");
				adc_value = ADC_read();
				t = (float)adc_value * 1.56/256 * 100 + 273.15;  //do K = do C + 273.15
				printtemp(t);
				LCD_cmd(0xCB);	//dua con tro den o thu 11 hang 2
				LCD_char_cp(0xDF);//in ki hieu nhiet do
				LCD_char_cp('K');
			}
		}
	}

}
