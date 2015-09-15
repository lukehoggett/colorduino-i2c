/*
  Colorduino - Colorduino DEMO for Arduino.
  Copyright (c) 2010 zzy@IteadStudio.  All right reserved.

  This DEMO is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This DEMO is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <Wire.h>                //wire library for I2C
#include <Colorduino.h>          //colorduino library

#define I2C_DEVICE_ADDRESS 0x05  //I2C address for this device 
#define START_OF_DATA 0x10       //data markers
#define END_OF_DATA 0x20         //data markers

void setup()
{
  Colorduino.Init();
  // compensate for relative intensity differences in R/G/B brightness
  // array of 6-bit base values for RGB (0~63)
  // whiteBalVal[0]=red
  // whiteBalVal[1]=green
  // whiteBalVal[2]=blue
  unsigned char whiteBalVal[3] = {33,63,63}; // for LEDSEE 6x6cm round matrix
  Colorduino.SetWhiteBal(whiteBalVal);

  Wire.begin(I2C_DEVICE_ADDRESS); // join i2c bus as slave
  Wire.onReceive(receiveEvent);   // define the receive function for receiving data from master
}

/****************************************************
Main Functions zone
****************************************************/

void loop()
{ 
   
  //1 pixel = 3 bytes B00000000 B00000000 B00000000. 
  //We send R then G then B bytes as 3 separate transfers
  //This is because if we make the I2C buffer too large, we run out of SRAM for other things on our master Arduino
  
  if (Wire.available()>66) { //when buffer full, process data. 66 =  start byte + colour + 64 pixel data + end byte
    
    // error check - make sure our data starts with the right byte   
    if (Wire.read() != START_OF_DATA) {
      //else handle error by reading remaining data until end of data marker (if available)
      while (Wire.available()>0 && Wire.read()!=END_OF_DATA) {}      
      return;
    }

    byte c = Wire.read(); //read our color byte so we know if these are the R, G or B pixels.
    
    //depeding on c read pixels in as R G or B
    //read red display data
    if (c == 0){
      for (byte x = 0; x < 8; x++){
        for (byte y = 0; y < 8; y++){
           PixelRGB *p = Colorduino.GetPixel(x,y);
           p->r = Wire.read();
        }
      }
    }
    
    //read green display data
    if (c == 1){
      for (byte x = 0; x < 8; x++){
        for (byte y = 0; y < 8; y++){
          PixelRGB *p = Colorduino.GetPixel(x,y);
           p->g = Wire.read(); 
        }
      }
    }
    
    //read blue display data
    if (c == 2){
      for (byte x = 0; x < 8; x++){
        for (byte y = 0; y < 8; y++){
           PixelRGB *p = Colorduino.GetPixel(x,y);
           p->b = Wire.read(); 
        }
      }
    }
    
    //read end of data marker
    if (Wire.read()==END_OF_DATA) {
      //if colour is blue, then update display
      if (c == 2){Colorduino.FlipPage();}
    } 
  }
}


//=============HANDLERS======================================

//get data from master - HINT: this is a ISR call!
//HINT2: do not handle stuff here!! this will NOT work
//collect only data here and process it in the main loop!
void receiveEvent(int numBytes) {
  //do nothing here
}
