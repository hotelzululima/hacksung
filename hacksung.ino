/*********************************************************************
Hacksung, control unit for Washing Machine
Roni Bandini @RoniBandini
Buenos Aires, Dec-2020
Note: this is a work in progress. 
*********************************************************************/

#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <SPI.h>  
#include <WiFiNINA.h>

Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);
// pin 7 - Serial clock out (SCLK) // pin 6 - Serial data out (DIN) // pin 5 - Data/Command select (D/C) // pin 4 - LCD chip select (CS) // pin 3 - LCD reset (RST)

// Debounce buttons
const int BUTTON_PIN = 0;         
const int BUTTON_PIN1 = 1;        
const int BUTTON_PIN2 = 2;        
const int DEBOUNCE_DELAY = 75;   
int lastSteadyState = HIGH;        
int lastSteadyState1 = HIGH;        
int lastSteadyState2 = HIGH;       
int lastFlickerableState = HIGH;   
int lastFlickerableState1 = HIGH;   
int lastFlickerableState2 = HIGH;  
int currentState;                
int currentState1;               
int currentState2;                
unsigned long lastDebounceTime = 0;   
unsigned long lastDebounceTime1 = 0;  
unsigned long lastDebounceTime2 = 0;  

// Relay pins
int relayMotor1   = 8;
int relayWaterIn  = 9;
int relayWaterOut = 10;
int relayMotor2   = 11;


// state variables
int waterIn=0;
int waterOut=0;
int motor1=0;
int screenCursor=1;
int menuLevel=0;

// WiFi
char ssid[] = "SSID";       
char pass[] = "PASSWORD"; 
int status = WL_IDLE_STATUS;
char server[] = "api.telegram.org";
char server2[] = "yourPHPServer.com";      
WiFiClient client;   

String serverAnswer="";   
String quote="";

const unsigned char logo [] PROGMEM = {
// 'hacksung', 84x48px
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x38, 0xf0, 0x38, 0x73, 0x8e, 
0x1c, 0xee, 0x30, 0xe0, 0x00, 0x07, 0x39, 0xf0, 0xfc, 0x73, 0xbf, 0x9c, 0xef, 0x31, 0xf8, 0x00, 
0x07, 0x39, 0xf8, 0xfe, 0x73, 0xbf, 0x9c, 0xef, 0x33, 0xf8, 0x00, 0x07, 0x39, 0xf9, 0xfe, 0x73, 
0xbf, 0x9c, 0xef, 0x33, 0xfc, 0x00, 0x07, 0x39, 0xf9, 0xee, 0x77, 0x3b, 0x9c, 0xef, 0x33, 0xbc, 
0x00, 0x07, 0x39, 0xf9, 0xce, 0x77, 0x3b, 0x9c, 0xef, 0x73, 0xbc, 0x00, 0x07, 0x39, 0xf9, 0xce, 
0x77, 0x3b, 0x9c, 0xef, 0x73, 0xbc, 0x00, 0x07, 0x39, 0xd9, 0xce, 0x7e, 0x3c, 0x1c, 0xef, 0xf3, 
0xbc, 0x00, 0x07, 0x39, 0x99, 0xce, 0x7e, 0x3e, 0x1c, 0xef, 0xf3, 0x80, 0x00, 0x07, 0xf9, 0x99, 
0xc0, 0x7e, 0x3e, 0x1c, 0xef, 0xf3, 0x80, 0x00, 0x07, 0xf9, 0x99, 0xc0, 0x7e, 0x1f, 0x1c, 0xef, 
0xf3, 0xbc, 0x00, 0x07, 0xf9, 0x99, 0xc0, 0x7e, 0x0f, 0x9c, 0xef, 0xf3, 0xbc, 0x00, 0x07, 0xfb, 
0x9d, 0xce, 0x7f, 0x0f, 0x9c, 0xef, 0xf3, 0xbc, 0x00, 0x07, 0x3b, 0x9d, 0xce, 0x7f, 0x07, 0xdc, 
0xef, 0xf3, 0x9c, 0x00, 0x07, 0x3b, 0xfd, 0xce, 0x7f, 0x3b, 0xdc, 0xee, 0xf3, 0x9c, 0x00, 0x07, 
0x3b, 0xfd, 0xce, 0x77, 0x3b, 0xdc, 0xee, 0xf3, 0x9c, 0x00, 0x07, 0x3b, 0xfd, 0xce, 0x77, 0x39, 
0xdc, 0xee, 0xf3, 0x9c, 0x00, 0x07, 0x3b, 0xfd, 0xee, 0x77, 0xbb, 0xdd, 0xee, 0x73, 0xbc, 0x00, 
0x07, 0x3b, 0x9d, 0xfe, 0x73, 0xbf, 0xdf, 0xee, 0x73, 0xfc, 0x00, 0x07, 0x3b, 0x9c, 0xfe, 0x73, 
0xbf, 0x9f, 0xce, 0x73, 0xfc, 0x00, 0x07, 0x3b, 0x9c, 0xfc, 0x73, 0x9f, 0x8f, 0xce, 0x71, 0xfc, 
0x00, 0x07, 0x3b, 0x9e, 0x38, 0x73, 0x8e, 0x07, 0x8e, 0x70, 0xcc, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

const unsigned char wifilogo [] PROGMEM = {
// 'wifi', 84x48px
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 
0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x03, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xfe, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x03, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xf0, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 
0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x03, 0xc0, 0x01, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xfc, 
0x00, 0x7f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0x00, 0x3f, 0xe0, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xc0, 0x1f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 
0xff, 0xf0, 0x0f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xf8, 0x07, 0xf8, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xfc, 0x07, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x1f, 0xfe, 0x03, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0x01, 0xfc, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0x81, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0xff, 0xc0, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xc0, 
0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xe0, 0x7f, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x1f, 0xe0, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 
0xf0, 0x3f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x07, 0xf0, 0x3f, 0x80, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x01, 0xfc, 0x07, 0xf8, 0x3f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xfc, 
0x03, 0xf8, 0x3f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xfe, 0x03, 0xf8, 0x1f, 0x80, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x03, 0xfe, 0x01, 0xf8, 0x1f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 
0xfe, 0x01, 0xfc, 0x1f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xfc, 0x01, 0xfc, 0x1f, 0xc0, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xfc, 0x01, 0xfc, 0x1f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x70, 0x01, 0xfc, 0x1f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char doom1 [] PROGMEM = {
// 'doom1', 84x48px
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xa5, 0x44, 0x4f, 0x89, 
0x29, 0xf5, 0x12, 0xbe, 0x57, 0xa4, 0xb0, 0xc0, 0x00, 0x03, 0x00, 0x00, 0x60, 0x00, 0x08, 0x07, 
0x00, 0x30, 0xf8, 0x00, 0x00, 0x40, 0x02, 0x50, 0x00, 0x04, 0x07, 0x80, 0xe0, 0xf8, 0x00, 0x01, 
0x00, 0x00, 0x21, 0x00, 0x04, 0x03, 0x00, 0xf0, 0xf1, 0x00, 0x02, 0x20, 0x00, 0x60, 0x00, 0x04, 
0x07, 0x20, 0xf0, 0xf8, 0x08, 0x01, 0x00, 0x00, 0x20, 0x00, 0x04, 0x02, 0x00, 0xf0, 0xf0, 0x00, 
0x22, 0x00, 0x00, 0x40, 0x00, 0x08, 0x03, 0x00, 0xf0, 0xf0, 0x24, 0x01, 0x02, 0x40, 0x20, 0x28, 
0x04, 0x00, 0x10, 0xf0, 0xf8, 0x1f, 0x00, 0x03, 0xe0, 0x20, 0x7c, 0x04, 0x00, 0x00, 0xf0, 0xf0, 
0x1f, 0x03, 0x03, 0xe0, 0x42, 0x3c, 0x04, 0x00, 0x00, 0xf0, 0xf8, 0x1f, 0x00, 0x01, 0xe0, 0x20, 
0x7c, 0x04, 0x00, 0x00, 0xf0, 0xf0, 0x1f, 0x03, 0x03, 0xe0, 0x20, 0x7c, 0x05, 0x00, 0x00, 0x70, 
0xf0, 0x1f, 0x00, 0x03, 0xe0, 0x20, 0x7c, 0x00, 0x00, 0x11, 0xf0, 0xf8, 0x1f, 0x03, 0x03, 0xe0, 
0x48, 0x3d, 0x04, 0x00, 0x00, 0xe0, 0xf0, 0x1f, 0x00, 0x03, 0xe0, 0x20, 0x7c, 0x04, 0x00, 0x00, 
0xf0, 0xf8, 0x1f, 0x03, 0x01, 0xe0, 0x00, 0x7c, 0x04, 0x02, 0x00, 0xf0, 0xf0, 0x0f, 0x01, 0x03, 
0xe0, 0x60, 0x7c, 0x04, 0x00, 0x00, 0xf0, 0xf0, 0x1f, 0x00, 0x03, 0xe0, 0x00, 0x7c, 0x04, 0x20, 
0x00, 0xf0, 0xf8, 0x1f, 0x03, 0x03, 0xe0, 0x20, 0x3c, 0x04, 0x00, 0x00, 0xf0, 0xf8, 0x1f, 0x01, 
0x03, 0xe0, 0x20, 0x7c, 0x04, 0x00, 0x00, 0x70, 0xf0, 0x1f, 0x00, 0x03, 0xe0, 0x20, 0x7c, 0x04, 
0x00, 0x01, 0xe0, 0xf8, 0x1f, 0x03, 0x03, 0xe0, 0x40, 0x7c, 0x04, 0x00, 0x00, 0xf0, 0xf0, 0x1f, 
0x00, 0x01, 0xf0, 0x20, 0x3c, 0x00, 0x00, 0x00, 0xf0, 0xf8, 0x0f, 0x03, 0x03, 0xe8, 0x20, 0x7c, 
0x44, 0x04, 0x40, 0xf0, 0xf0, 0x1c, 0x00, 0x00, 0xe0, 0x00, 0x38, 0x00, 0x80, 0x00, 0xf0, 0x70, 
0x18, 0x02, 0x80, 0x60, 0x40, 0x60, 0x0c, 0x08, 0x80, 0xf0, 0xf8, 0x10, 0x01, 0x00, 0x00, 0x20, 
0x00, 0x04, 0x0c, 0xc0, 0xf0, 0xf8, 0x00, 0x01, 0x00, 0x00, 0x20, 0x80, 0x04, 0x04, 0x80, 0x70, 
0xf0, 0x00, 0x01, 0xc0, 0x00, 0x20, 0x08, 0x18, 0x0c, 0x81, 0xb0, 0xf0, 0x00, 0x03, 0xc0, 0x00, 
0x40, 0x00, 0x3c, 0x87, 0xc0, 0xf0, 0xf8, 0x00, 0x0f, 0xf0, 0x00, 0x20, 0x00, 0xff, 0x0d, 0x80, 
0xf0, 0xf8, 0x14, 0x0f, 0xf8, 0x00, 0x60, 0x00, 0xff, 0x8f, 0xa0, 0xf0, 0xf0, 0x80, 0x7f, 0xfe, 
0x20, 0xf0, 0x07, 0xff, 0xcf, 0x80, 0xf0, 0xf8, 0x00, 0x7f, 0xff, 0x03, 0xf9, 0x27, 0xff, 0xef, 
0x80, 0xe0, 0xd8, 0x13, 0xff, 0xff, 0x87, 0xfe, 0x9f, 0xff, 0xff, 0xc0, 0xf0, 0xf8, 0x03, 0xff, 
0xff, 0xff, 0xfd, 0xff, 0xff, 0xff, 0x80, 0xf0, 0xf0, 0x07, 0xfb, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xc0, 0xf0, 0xf8, 0x1f, 0xff, 0xff, 0xf7, 0xff, 0x7e, 0xf7, 0xfd, 0x90, 0xf0, 0xf0, 0x3f, 
0xdf, 0xff, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xc2, 0xf0, 0xf8, 0xff, 0xff, 0xef, 0xff, 0xff, 0xbf, 
0xfd, 0xff, 0xf0, 0xf0, 0xf9, 0xff, 0xfb, 0xff, 0xff, 0xff, 0xff, 0xff, 0xdf, 0xf0, 0xf0, 0xf3, 
0xff, 0xff, 0xfd, 0xff, 0xff, 0xff, 0x7f, 0xff, 0xfd, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 
0xf7, 0xff, 0xff, 0xff, 0xb0, 0xff, 0xf7, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xd0
};

const unsigned char doom2 [] PROGMEM = {
// 'doom2', 84x48px
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 
0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x01, 0xff, 0xf0, 0x03, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 
0xc0, 0x3f, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0x03, 0xff, 0xff, 0xfc, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 
0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 
0x80, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 
0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 
0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 
0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 
0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xf0, 0xff, 0xff, 0xe1, 0xff, 0x80, 0x00, 0x00, 
0x00, 0x00, 0x1f, 0xe0, 0x3f, 0xff, 0x80, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xe0, 0x0f, 
0xfe, 0x00, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xe0, 0x01, 0xf8, 0x07, 0xff, 0x80, 0x00, 
0x00, 0x00, 0x00, 0x1f, 0xc0, 0x00, 0x00, 0x3f, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 
0x00, 0x00, 0x7f, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xe6, 0x0c, 0xff, 0xff, 0xc0, 
0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x7f, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xf0, 0xff, 0xff, 0xc7, 0xff, 
0xc0, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xf0, 0xff, 0x1f, 0xe1, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 
0x7e, 0x39, 0xfe, 0x0f, 0xf1, 0x8f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x07, 0xc0, 0x00, 0x7c, 
0x0f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xc0, 0x00, 0x00, 0x00, 
0x00, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xf0, 0x00, 0x00, 
0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xf0, 0x0e, 0x0e, 0x01, 0xff, 0x80, 0x00, 0x00, 
0x00, 0x00, 0x0f, 0xe0, 0x1f, 0xff, 0x00, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xe0, 0x3f, 
0xff, 0xc0, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xe0, 0x0f, 0xfe, 0x00, 0xec, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x07, 0x60, 0x00, 0xe0, 0x00, 0x6c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xe0, 
0x00, 0x00, 0x00, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xe0, 0xf0, 0xe0, 0xf0, 0xf0, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x01, 0xe1, 0xff, 0xff, 0xf0, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 
0xf0, 0x00, 0x00, 0x01, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x00, 0x00, 0x03, 0xe0, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x1f, 0xff, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x1c, 0x06, 0x04, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x80, 0x00, 0x1e, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xc0, 0x00, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x7e, 0x07, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

const unsigned char doom3 [] PROGMEM = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 
0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 
0xff, 0xbf, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0x17, 0xff, 0xfe, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xfc, 0x03, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 
0xff, 0xe0, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xf3, 0xff, 0xff, 0xff, 
0x80, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 
0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 
0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 
0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 
0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 
0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 
0xfe, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 
0x00, 0x00, 0x00, 0x1f, 0xf8, 0x1f, 0xff, 0xf0, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xf0, 
0x03, 0xfe, 0x00, 0x7f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xe0, 0x00, 0xf8, 0x00, 0x3f, 0xc0, 
0x00, 0x00, 0x00, 0x00, 0x3f, 0xf0, 0x00, 0xc0, 0x00, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x3f, 
0xff, 0x81, 0x00, 0x1f, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xc7, 0x8f, 0x3f, 0xff, 
0xe0, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 
0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xf8, 0x7f, 0x9f, 0xf0, 0xff, 0xe0, 0x00, 0x00, 0x00, 
0x00, 0x1f, 0x18, 0x03, 0x06, 0x01, 0x87, 0x80, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x18, 0x04, 0x01, 
0x01, 0x87, 0x80, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xf0, 0x3c, 0x03, 0xe0, 0xff, 0x80, 0x00, 0x00, 
0x00, 0x00, 0x07, 0xf0, 0x7c, 0x03, 0xf0, 0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xe0, 0xfc, 
0x00, 0xf0, 0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xe0, 0xc3, 0x04, 0x10, 0x7e, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x03, 0xe0, 0xc0, 0x00, 0x38, 0x6e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x70, 
0xf8, 0xf1, 0xf8, 0x6e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf1, 0xff, 0xff, 0xf8, 0x70, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0xf1, 0xfc, 0x03, 0xf8, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0xf1, 0xf8, 0x00, 0xf8, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x39, 0xe0, 0x00, 0x78, 0xc0, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0xe0, 0x00, 0x39, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x3c, 0xff, 0xff, 0xf1, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x7f, 0x07, 0xe7, 
0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x78, 0x01, 0xef, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x03, 0xe0, 0x00, 0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 
0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

const unsigned char doom4 [] PROGMEM = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 
0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x1f, 0x80, 0x1f, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xf8, 
0x00, 0x07, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x67, 0xff, 0xf8, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x07, 0x03, 0xc3, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 
0x0f, 0xf3, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xef, 0xff, 0xff, 0xff, 0xff, 
0x80, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xef, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 
0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 
0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xf0, 0xcf, 0xff, 0xff, 0xf0, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x1f, 0xe0, 0x0f, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x08, 0x1f, 0xc0, 
0x07, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x03, 0xff, 0xff, 0xf0, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x01, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x70, 0x10, 
0x00, 0x01, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x7f, 0x10, 0x43, 0xfc, 0xff, 0xff, 0xf0, 
0x00, 0x00, 0x00, 0x00, 0x1e, 0x78, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x1e, 
0xff, 0xff, 0xc7, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0x1f, 0xff, 0xff, 
0xf0, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0x19, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 
0x60, 0xf1, 0xff, 0x10, 0x7f, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x60, 0xe1, 0xfe, 0x00, 0x3f, 
0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x60, 0xe1, 0xc0, 0x00, 0x3f, 0xff, 0x80, 0x00, 0x00, 0x00, 
0x00, 0x60, 0xe1, 0xe0, 0x00, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x60, 0xc1, 0x90, 0x1f, 
0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x61, 0xe3, 0x18, 0x1f, 0xff, 0xff, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x61, 0xff, 0xfc, 0x1f, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x73, 0xff, 0xfe, 
0x0f, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x77, 0xff, 0xfe, 0x0f, 0x3f, 0xf8, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x7f, 0xc0, 0x0f, 0x0e, 0x3f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x77, 0xe0, 
0x1f, 0x0e, 0x3f, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x77, 0xff, 0xff, 0x0e, 0xfe, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x07, 0xe0, 0x3f, 0x1f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 
0xc0, 0x3f, 0x1f, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x3e, 0x3f, 0xc0, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xfe, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x3f, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0xff, 0xfc, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x02, 0x00, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xdf, 0xfc, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


void setup()   {
  
  Serial.begin(9600);
  Serial.println("Hacksung started...");   
  Serial.println("Roni Bandini - Argentina");  

  // Output pins
  pinMode(relayMotor1, OUTPUT);
  pinMode(relayWaterIn, OUTPUT);
  pinMode(relayWaterOut, OUTPUT);
  pinMode(relayMotor2, OUTPUT);

  // button Pins
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUTTON_PIN1, INPUT_PULLUP);
  pinMode(BUTTON_PIN2, INPUT_PULLUP);

  // Start closed
  digitalWrite(relayMotor1, HIGH);
  digitalWrite(relayMotor2, HIGH);
  digitalWrite(relayWaterIn, HIGH);
  digitalWrite(relayWaterOut, HIGH);


  display.begin();  
  display.setContrast(60);
  display.clearDisplay();    
  display.drawBitmap(0, 0,  logo, 84, 48, 1); 
  display.display();  
  delay(2000);
  
  display.clearDisplay();    
  display.drawBitmap(5, 0,  wifilogo, 84, 48, 1); 
  display.display();  

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Connecting to SSID: ");
    Serial.println(ssid);    
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection
    delay(10000);
  }
  
  Serial.println("Connected");

  
   
}


void loop() {

     // print menu   
     
     if (menuLevel==0){

      switch (screenCursor) {
        case 1:      
          myMessage("> Manual"," Auto "," Extra", " Info");         
          break;
        
        case 2:    
          myMessage(" Manual","> Auto"," Extra"," Info");        
          break;
          
         case 3:    
          myMessage(" Manual"," Auto","> Extra"," Info");        
          break;
          
         case 4:    
          myMessage(" Manual"," Auto"," Extra", " > Info");        
          break;        
        
      }    
    } // menu level 0
   
    
    if (menuLevel==1){

      // Manual
      switch (screenCursor) {
        
        case 1:      
          myMessage("> Water In"," Motor on"," Water out", " Back");         
          break;
        
        case 2:    
          myMessage(" Water In","> Motor on"," Water out"," Back");        
          break;
          
         case 3:    
          myMessage(" Water In"," Motor on","> Water out"," Back");        
          break;
          
         case 4:    
          myMessage(" Water In"," Motor on"," Water out", "> Back");        
          break;        

       }
      } // menu level 1

     if (menuLevel==2){

       // Auto
       switch (screenCursor) {
       
        case 1:      
          myMessage("> Mansilla"," Gombro "," Bioy", " Back");         
          break;
        
        case 2:    
          myMessage(" Mansilla","> Gombro"," Bioy"," Back");        
          break;
          
         case 3:    
          myMessage(" Mansilla"," Gombro","> Bioy"," Back");        
          break;
          
         case 4:    
          myMessage(" Mansilla"," Gombro"," Bioy", "> Back");        
          break;        
      } 

      }// menu level 2

      if (menuLevel==3){

       // Extra
        switch (screenCursor) {
       
        case 1:      
          myMessage("> Doom"," Dollar "," Stats", " Back");         
          break;
        
        case 2:    
          myMessage(" Doom","> Dollar"," Stats"," Back");        
          break;
          
         case 3:    
          myMessage(" Doom"," Dollar","> Stats"," Back");        
          break;
          
         case 4:    
          myMessage(" Doom"," Dollar"," Stats", "> Back");        
          break;        
      }                            
       
      } // menu level 3

      
      if (menuLevel==4){

       // Info

       myMessage("Roni Bandini","Dec 2020"," @RoniBandini", "Argentina");     
       delay(4000);   
       menuLevel=0;
                                
       
      } // menu level 3
          

  // read buttons

  currentState = digitalRead(BUTTON_PIN);
  currentState1 = digitalRead(BUTTON_PIN1);
  currentState2 = digitalRead(BUTTON_PIN2);  

  // button up
  
 if (currentState != lastFlickerableState) {    
    lastDebounceTime = millis();    
    lastFlickerableState = currentState;
  }

  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {    

    // if the button state has changed:
    if(lastSteadyState == HIGH && currentState == LOW)
      //Serial.println("");
      int dummy=1;
    else if(lastSteadyState == LOW && currentState == HIGH)

      {               
        
        screenCursor--;
        
        if (screenCursor==0) {screenCursor=4;}            
      
        }

    // save the the last steady state
    lastSteadyState = currentState;
  }


  // Button down
  
 if (currentState1 != lastFlickerableState1) {    
    lastDebounceTime1 = millis();    
    lastFlickerableState1 = currentState1;
  }

  if ((millis() - lastDebounceTime1) > DEBOUNCE_DELAY) {    
    
    if(lastSteadyState1 == HIGH && currentState1 == LOW)
      int dummy=1;
    else if(lastSteadyState1 == LOW && currentState1 == HIGH)
      {              
      
       screenCursor++;
      
      if (screenCursor==5) {screenCursor=1;}
      
        }
    
    lastSteadyState1 = currentState1;
  }

 
 // Button enter

  if (currentState2 != lastFlickerableState2) {    
    lastDebounceTime2 = millis();    
    lastFlickerableState2 = currentState2;
  }

  if ((millis() - lastDebounceTime2) > DEBOUNCE_DELAY) {    
    
    if(lastSteadyState2 == HIGH && currentState2 == LOW)
      int dummy=1;
    else if(lastSteadyState2 == LOW && currentState2 == HIGH)
      {            

     if (menuLevel==0){

        switch (screenCursor) {
        
        case 1:      
          menuLevel=1;    
          screenCursor=1;               
          break;

        case 2:      
          menuLevel=2;                   
          screenCursor=1;
          break;   

        case 3:      
          menuLevel=3;   
          screenCursor=1;                
          break;  
       
       case 4:      
          menuLevel=4;   
          screenCursor=1;                
          break;      
          
      }
     }
     else if (menuLevel==1){
       
       switch (screenCursor) {
        
        case 1:      

            if (waterIn==0){              
              digitalWrite(relayWaterIn, LOW);
              waterIn=1;        
      
              }
             else{              
              digitalWrite(relayWaterIn, HIGH);
              waterIn=0;        
             }
         
          break;

          case 2:    
      
             if (motor1==0){              
              digitalWrite(relayMotor1, LOW);
              motor1=1;        
              }
             else{              
              digitalWrite(relayMotor1, HIGH);
              motor1=0;        
             }
         
          break;  
        
        case 3:    

              if (waterOut==0){              
              digitalWrite(relayWaterOut, LOW);
              waterOut=1;        
              }
             else{              
              digitalWrite(relayWaterOut, HIGH);
              waterOut=0;        
             }
               
          break;
                           
         case 4:   
                  
           menuLevel=0;
         
          break;        
      }

      } // menu level 1
      
      else if (menuLevel==2){
        
          switch (screenCursor) {
        
              case 1:      
                // programa lavarropas 1, mansilla
                display.clearDisplay();
                myMessage("Lucio Mansilla","Prewash: 5 min","Wash: 5 min", "Cent: 400 rpm");                    
                display.display();
                delay(2000); 

                // Washing cycle example
                digitalWrite(relayWaterIn, LOW);
                delay(20000);
                digitalWrite(relayWaterIn, HIGH);

                digitalWrite(relayMotor1, LOW);
                delay(20000);
                digitalWrite(relayMotor1, HIGH);

                digitalWrite(relayWaterOut, LOW);
                delay(20000);
                digitalWrite(relayWaterOut, HIGH);

                digitalWrite(relayMotor1, LOW);
                delay(20000);
                digitalWrite(relayMotor1, HIGH);

                // to read motor RPM 
                //motorRpm=analogRead(A0);

                // to adjust motor speed accoridng to RPM
                //analogWrite(A1, 100);
                                    
                if (client.connectSSL(server, 443)) {                                   
                  client.println("GET /XXXXXXXXXXXXXXXXXYYYYYYYYYYYYYYYY/sendMessage?chat_id=-ZZZZZZZZZZZZZZZZ&text=Finished HTTP/1.1");
                  client.println("Host: api.telegram.org");
                  client.println("Connection: close");
                  client.println();
                }
                              
                              
                break;
    
              case 2:      
              // programa lavarropas 2 gombro
              display.clearDisplay();
              myMessage("Gombrowicz","Prewash: 15 min","Wash: 30 min", "Cent: 800 rpm");                                  
              display.display();
              delay(2000);   
              break; 
    
              case 3:
               // programa lavarropas 3 Bioy
               display.clearDisplay();
               myMessage("Bioy Casares","Prewash: 30 min","Wash: 60 min", "Cent: 1000 rpm");                                  
               display.display();
               delay(2000);   
               break;
    
              case 4:      
              // return
              menuLevel=0;                   
              break;     
          
        }// switch
        
     } // menu level 2

     else if (menuLevel==3){
        
          switch (screenCursor) {
        
              case 1:      
                // display doom animation

               display.clearDisplay();    
               display.drawBitmap(0, 0,  doom1, 84, 48, 1); 
               display.display();
               delay(1500);    
        
               display.clearDisplay();    
               display.drawBitmap(0, 0,  doom2, 84, 48, 1); 
               display.display();
               delay(1500);    
        
               display.clearDisplay();    
               display.drawBitmap(0, 0,  doom3, 84, 48, 1); 
               display.display();
               delay(1000);    
        
               display.clearDisplay();    
               display.drawBitmap(0, 0,  doom4, 84, 48, 1); 
               display.display();
               delay(1000);    
                     
               menuLevel=0;               
                              
                break;
    
              case 2:      
              // Dolar

              if (client.connect(server2, 80)) {                  
                  Serial.println("connected to server");
                  // HTTP request
                  
                  client.println("GET /server/dolar.php HTTP/1.1");
                  client.println("Host: YourPHPServer.com");
                  client.println("Connection: close");
                  client.println();
                }

              waitServer();

              serverAnswer="";
              
              while (client.available()) {
                
                    char c = client.read();

                    serverAnswer=serverAnswer+c;
                
                    Serial.write(c);
                
                  }
              
              quote=getValue(serverAnswer,'~',1);                         
              myMessage("Dollar quote","Sell $ ",quote, " ");
              delay(4000);                         
              menuLevel=0;
                           
              break; 
    
              case 3:
               // Stats
               myMessage("Stats","Manual: 12","Auto: 0", "Run: 8 hrs");
               delay(4000);                         
               menuLevel=0; 
               break;
    
              case 4:      
              // return
              menuLevel=0;                   
              break;     
          
        }// switch
        
     } // menu level 3
        
    } // button pressed
    
    lastSteadyState2 = currentState2;
  }                   
      
  
  delay(50);


  
}// loop


void myMessage(String line1, String line2, String line3, String line4){

      display.clearDisplay();            
      display.setCursor(0,0);
      display.setTextSize(1);
      display.setTextColor(WHITE, BLACK);
      display.println("Hacksung");
      display.setTextColor(BLACK);
      display.setCursor(0,10);      
      display.println(line1);
      display.setCursor(0,20);
      display.println(line2);
      display.setCursor(0,30);
      display.println(line3);
      display.setCursor(0,40);
      display.println(line4);

      // add icons      
      if (waterIn==1){        
        display.setTextSize(2);
        display.setCursor(70,0);
        display.write(30); // up
      }

       if (motor1==1){
        display.setTextSize(2);
        display.setCursor(70,15);
        display.write(236); // infinite
      }

      if (waterOut==1){
        display.setTextSize(2);
        display.setCursor(70,32);
        display.write(31); // down
      }
        
      display.display();
      
  }

void waitServer() {
  int timeout = 0;
  while (!client.available() && timeout < 5000) {
    timeout++;
    delay(1);
    if (timeout >= 5000) {
      Serial.println(F("Error, max timeout reached"));
      break;
    }
  }
}  

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}