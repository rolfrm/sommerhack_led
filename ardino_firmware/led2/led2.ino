#include <Adafruit_NeoPixel.h>

/*
Display_Dingo - Used by the event sommerhack.dk
Based on ESPOPC -- Open Pixel Control server for ESP8266.

The MIT License (MIT)

Copyright (c) 2015 by bbx10node@gmail.com 
Copyright (C) 2016 Georg Sluyterman <georg@sman.dk>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <Adafruit_NeoPixel.h>

const char* ssid = "Vammen Camping";
const char* password = "";

MDNSResponder mdns;
// Actual name will be "espopc.local"
const char myDNSName[] = "displaydingo1";

WiFiUDP Udp;
#define OSCDEBUG    (0)

//#include <NeoPixelBus.h>
const int PixelCount = 300;
const int PixelPin = D8;
#define BRIGHTNESS 100

// three element pixels, in different order and speeds
//NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin);
//NeoPixelBus<NeoRgbFeature, Neo400KbpsMethod> strip(PixelCount, PixelPin);

// You can also use one of these for Esp8266,
// each having their own restrictions
//
// These two are the same as above as the DMA method is the default
// NOTE: These will ignore the PIN and use GPI03 pin
//NeoPixelBus<NeoGrbFeature, NeoEsp8266Dma800KbpsMethod> strip(PixelCount, PixelPin);
//NeoPixelBus<NeoRgbFeature, NeoEsp8266Dma400KbpsMethod> strip(PixelCount, PixelPin);

// Uart method is good for the Esp-01 or other pin restricted modules
// NOTE: These will ignore the PIN and use GPI02 pin
////NeoPixelBus<NeoGrbFeature, NeoEsp8266Uart800KbpsMethod> strip(PixelCount, PixelPin);
//NeoPixelBus<NeoRgbFeature, NeoEsp8266Uart400KbpsMethod> strip(PixelCount, PixelPin);

// The bitbang method is really only good if you are not using WiFi features of the ESP
// It works with all but pin 16
//NeoPixelBus<NeoGrbFeature, NeoEsp8266BitBang800KbpsMethod> strip(PixelCount, PixelPin);
//NeoPixelBus<NeoRgbFeature, NeoEsp8266BitBang400KbpsMethod> strip(PixelCount, PixelPin);

// four element pixels, RGBW
//NeoPixelBus<NeoRgbwFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin);

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PixelCount, PixelPin, NEO_GRB + NEO_KHZ800);

// Gamma correction 2.2 look up table
uint8_t GammaLUT[256];

void fillGammaLUT(float gamma)
{
  int i;

  for (i = 0; i < 256; i++) {
    float intensity = (float)i / 255.0;
    GammaLUT[i] = (uint8_t)(pow(intensity, gamma) * 255.0);
    //Serial.printf("GammaLUT[%d] = %u\r\n", i, GammaLUT[i]);
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println();

  // this resets all the neopixels to an off state
  strip.setBrightness(255);
  strip.begin();
  strip.show();
  

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Print the IP address
  Serial.println(WiFi.localIP());

  // Set up mDNS responder:
  if (!mdns.begin(myDNSName, WiFi.localIP())) {
    Serial.println("Error setting up MDNS responder!");
  }
  else {
    Serial.println("mDNS responder started");
    Serial.printf("My name is [%s]\r\n", myDNSName);
  }

  fillGammaLUT(2.2);

  for(int i = 0; i < 256; i++)
  {
    Serial.printf("Gamma: %i = %i\n", i,  GammaLUT[i]);
  
  }
  
  Udp.begin(7890);
  for(int i = 0; i < PixelCount; i++)
    strip.setPixelColor(i, 255, 255, 255);
  strip.show();
}

WiFiClient client;

int min(int x, int y){
  if(x < y)
   return x;
  return y;
}

bool dirtyshow = false;

void blit_cmd(uint8_t * data, uint32_t size){
  if(size < sizeof(int16_t))
    return;
  int16_t * cmdptr = (int16_t *) data;
  size -= sizeof(int16_t);
  uint8_t * pixrgb = data + sizeof(int16_t);
  int offset = *cmdptr;
  //Serial.printf("Offset: %i, Size: %i\n", offset, size / 3);
  for (int i = offset; i < min(size / 3 + offset, PixelCount); i++) {
    strip.setPixelColor(
      i, GammaLUT[*pixrgb++], GammaLUT[*pixrgb++],
      GammaLUT[*pixrgb++]
    );
  }
  dirtyshow = true;
}


#define minsize(x,y) (((x)<(y))?(x):(y))
uint8_t rcv[10 + 300 * 3];
void clientEventUdp(){
  int32_t packetSize = 0;
  while((packetSize = Udp.parsePacket())){
    int32_t len = Udp.read(rcv, sizeof(rcv));
    uint16_t * cmdptr = (uint16_t *)rcv;
    uint16_t cmd = cmdptr[0];
    if(cmd == 1111)
      blit_cmd(rcv + sizeof(int16_t), len - sizeof(int16_t)); 
  }
}

void loop() {
  clientEventUdp();

  if(dirtyshow && strip.canShow())
  {
    strip.show();
    dirtyshow = false;
  }
}
