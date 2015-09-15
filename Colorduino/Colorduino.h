/*
  Colorduino - Colorduino Library for Arduino V1.1
  Copyright (c) 2011 Sam C. Lin lincomatic@hotmail.com ALL RIGHTS RESERVED
  based on C code by zzy@iteadstudio
    Copyright (c) 2010 zzy@IteadStudio.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#ifndef _COLORDUINO_H_
#define _COLORDUINO_H_
#include "WProgram.h"
#include <avr/pgmspace.h> 
#include <avr/io.h>
#include <avr/interrupt.h>

#define ColorduinoBitsPerColor 8

#define ColorduinoScreenWidth 8
#define ColorduinoScreenHeight 8

/*****************************
define the IO
*****************************/
#define RST_BIT 0x04
#define LAT_BIT 0x02
#define SLB_BIT 0x01
#define SCL_BIT 0x40
#define SDA_BIT 0x80

#define RST PORTC
#define LAT PORTC
#define SLB PORTC
#define SDA PORTD
#define SCL PORTD

#define open_line0	{PORTB=0x01;}
#define open_line1	{PORTB=0x02;}
#define open_line2	{PORTB=0x04;}
#define open_line3	{PORTB=0x08;}
#define open_line4	{PORTB=0x10;}
#define open_line5	{PORTB=0x20;}
#define open_line6	{PORTD=0x08;}
#define open_line7	{PORTD=0x10;}
#define close_all_lines	{PORTD=0x00;PORTB=0x00;}

#define LED_RST_SET RST|=RST_BIT
#define LED_RST_CLR RST&=~RST_BIT
#define LED_SDA_SET SDA|=SDA_BIT
#define LED_SDA_CLR SDA&=~SDA_BIT
#define LED_SCL_SET SCL|=SCL_BIT
#define LED_SCL_CLR SCL&=~SCL_BIT
#define LED_LAT_SET LAT|=LAT_BIT
#define LED_LAT_CLR LAT&=~LAT_BIT
#define LED_SLB_SET SLB|=SLB_BIT
#define LED_SLB_CLR SLB&=~SLB_BIT

typedef struct pixelRGB {
  unsigned char r;
  unsigned char g;
  unsigned char b;
} PixelRGB;

class ColorduinoObject {
public:
  PixelRGB frameBuffer0[ColorduinoScreenWidth*ColorduinoScreenHeight];
  PixelRGB frameBuffer1[ColorduinoScreenWidth*ColorduinoScreenHeight];
  PixelRGB *curDrawFrame;
  PixelRGB *curWriteFrame;
  unsigned char line;

  ColorduinoObject() {
    line = 0;
    curWriteFrame = frameBuffer0;
    curDrawFrame = frameBuffer1;
  }

  void _IO_Init()
  {
    DDRD = 0xff; // set all pins direction of PortD
    DDRC = 0xff; // set all pins direction of PortC
    DDRB = 0xff; // set all pins direction of PortB
    
    PORTD = 0x00; // set all pins output is low of PortD
    PORTC = 0x00; // set all pins output is low of PortC
    PORTB = 0x00; // set all pins output is low of PortB
  }
  
  void LED_Delay(unsigned char i);
  
  void SetWhiteBal(unsigned char wbval[3]);

  void _LED_Init()
  {
    LED_RST_SET;
    LED_Delay(1);
    LED_RST_CLR;
    LED_Delay(1);
    LED_RST_SET;
    LED_Delay(1);
    
    line = 0;
  }

  void _TC2_Init()
  {
    TCCR2A |= (1 << WGM21) | (1 << WGM20);   
    TCCR2B |= ((1<<CS22)|(1<<CS20));   // by clk/64
    TCCR2B &= ~((1<<CS21));   // by clk/64
    TCCR2A &= ~((1<<WGM21) | (1<<WGM20));   // Use normal mode
    ASSR |= (1<<AS2);       // Use internal clock - external clock not used in Arduino
    TIMSK2 |= (1<<TOIE2) | (0<<OCIE2B);   //Timer2 Overflow Interrupt Enable
    TCNT2 = 0xff;
    sei();
  }
  
  void open_line(unsigned char x)
  {
    switch (x)
      {  
      case 0 :open_line0;
	break;
      case 1 :open_line1;
	break;
      case 2 :open_line2;
	break;
      case 3 :open_line3;
	break;
      case 4 :open_line4;
	break;
      case 5 :open_line5;
	break;
      case 6 :open_line6;
	break;
      case 7 :open_line7;
	break;
      default: close_all_lines;
	break;
      }
  }

  void Init() {
    _IO_Init();           //Init IO
    _LED_Init();          //Init LED Hardware
    _TC2_Init();          //Init Timer/Count2
  }

  void FlipPage() {
    cli();
    // swap frame buffers
    PixelRGB *tmp = curDrawFrame;
    curDrawFrame = curWriteFrame;
    curWriteFrame = tmp;
    sei();
  }

  // get a pixel for writing in the offscreen framebuffer
  PixelRGB *GetPixel(unsigned char x,unsigned char y) {
    return curWriteFrame + (y * ColorduinoScreenWidth) + x;
  }

  // get a pixel from the active framebuffer
  PixelRGB *GetDrawPixel(unsigned char x,unsigned char y) {
    return curDrawFrame + (y * ColorduinoScreenWidth) + x;
  }

  // set a pixel in the offscreen frame buffer
  void SetPixel(unsigned char x, unsigned char y, unsigned char r, unsigned char g, unsigned char b)
  {
    PixelRGB *p = GetPixel(x,y);
    p->r = r;
    p->g = g;
    p->b = b;
  }

  void run();
};

extern ColorduinoObject Colorduino;

#endif // _COLORDUINO_H_
