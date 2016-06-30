/*
 * This class can be linked to an Adafruit tft screen
 * an allows to draw on the screen a bitmap stored in
 * the sd card reader attached to the tft.
 * 
 * Adapted from the example code of Adafruit library.
 * 
 * Author: Giacomo Del Rio
*/

#ifndef DRAWBITMAP_H_
#define DRAWBITMAP_H_

#include "SPI.h"
#include "SD.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

// Size of the buffer to draw image, the bigger, the faster
#define BUFFPIXEL 20

class BitmapDrawer {
  public:
    BitmapDrawer(Adafruit_ILI9341* tft, int bufsize);
    void draw(char const *filename, uint8_t x, uint16_t y);
  
  private:
    Adafruit_ILI9341* tft;
    int bufsize;
    
    uint16_t read16(File &f);
    uint32_t read32(File &f);
};

#endif  //DRAWBITMAP_H_
