/*
 * Coded by: 
 *  Robin Costas del Moral 
 *  Antonio Jaimez Jimenez
 *  
 *    TE < ETSIIT < UGR
 *    
 */
 
#include <RH_ASK.h>
#include <SPI.h> // Not actually used but needed to compile
#include <I2Cdev.h>
#include <MPU6050.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Defines for Arduino PINs
#define LIGHTS_BUTTON 5
#define BRAKE_BUTTON 4
#define OUT 4
#define IN 3

// Defines for message codes
#define RIGHT_MESSAGE 0
#define LEFT_MESSAGE 1
#define LIGHTS_MESSAGE 2
#define BRAKE_MESSAGE 3

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16

// Variables for the MPU6050
MPU6050 accelgyro;

int ax, ay, az;
int gx, gy, gz;
long prev_time;
float dt;
float ang_x;
float ang_x_prev;

// Radio
RH_ASK driver(2000, 7, 6, 10);

// Bool for lights status controlling
bool active_lights = false;
bool active_brake = false;
bool active_left = false;
bool active_right = false;


Adafruit_SSD1306 display = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

 /*
 0- 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
 1- 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
 2- 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
 3- 0 0 0 0 X X X 0 X X X X X 0 0 0
 4- 0 0 X X X 0 0 0 X X X X X X 0 0
 5- X X X 0 0 0 0 X X X X X X X X 0
 6- 0 0 0 0 X X 0 X X X X X X X X X
 7- 0 0 X X X 0 0 X X X X X X X X X
 8- X X X 0 0 0 0 X X X X X X X X X
 9- 0 0 0 0 X X 0 X X X X X X X X X
 A- 0 0 X X X 0 0 X X X X X X X X 0
 B- X X X 0 0 0 0 0 X X X X X X 0 0
 C- 0 0 0 0 X X X 0 X X X X X 0 0 0
 D- 0 0 X X X 0 0 0 0 0 0 0 0 0 0 0
 E- X X X 0 0 0 0 0 0 0 0 0 0 0 0 0
 F- 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0

 */
static const unsigned char PROGMEM LIGHTS_LOGO[] =
{ B00000000, B00000000,
  B00000000, B00000000,
  B00000000, B00000000,
  B00001110, B11111000,
  B00111000, B11111100,
  B11100001, B11111110,
  B00001101, B11111111,
  B00111001, B11111111,
  B11100001, B11111111,
  B00001101, B11111111,
  B00111001, B11111110,
  B11100000, B11111100,
  B00001110, B11111000,
  B00111000, B00000000,
  B11100000, B00000000,
  B00000000, B00000000 };
  /*
 0- 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
 1- 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
 2- 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
 3- 0 0 0 0 0 X X 0 0 0 0 0 0 0 0 0
 4- 0 0 0 0 X X X 0 0 0 0 0 0 0 0 0
 5- 0 0 0 X X X X 0 0 0 0 0 0 0 0 0
 6- 0 0 X X X X X X X X X X X X 0 0
 7- 0 X X X X X X X X X X X X X 0 0
 8- 0 X X X X X X X X X X X X X 0 0
 9- 0 0 X X X X X X X X X X X X 0 0
 A- 0 0 0 X X X X 0 0 0 0 0 0 0 0 0
 B- 0 0 0 0 X X X 0 0 0 0 0 0 0 0 0
 C- 0 0 0 0 0 X X 0 0 0 0 0 0 0 0 0
 D- 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
 E- 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
 F- 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0

 */
static const unsigned char PROGMEM LEFT_LOGO[] =
{ B00000000, B00000000,
  B00000000, B00000000,
  B00000000, B00000000,
  B00000110, B00000000,
  B00001110, B00000000,
  B00011110, B00000000,
  B00111111, B11111100,
  B01111111, B11111100,
  B01111111, B11111100,
  B00111111, B11111100,
  B00011110, B00000000,
  B00001110, B00000000,
  B00000110, B00000000,
  B00000000, B00000000,
  B00000000, B00000000,
  B00000000, B00000000 };
  
static const unsigned char PROGMEM RIGHT_LOGO[] =
{ B00000000, B00000000,
  B00000000, B00000000,
  B00000000, B00000000,
  B00000000, B01100000,
  B00000000, B01110000,
  B00000000, B01111000,
  B00111111, B11111100,
  B00111111, B11111110,
  B00111111, B11111110,
  B00111111, B11111100,
  B00000000, B01111000,
  B00000000, B01110000,
  B00000000, B01100000,
  B00000000, B00000000,
  B00000000, B00000000,
  B00000000, B00000000 };

static const unsigned char PROGMEM NULL_LOGO[] =
{ B00000000, B00000000,
  B00000000, B00000000,
  B00000000, B00000000,
  B00000000, B00000000,
  B00000000, B00000000,
  B00000000, B00000000,
  B00000000, B00000000,
  B00000000, B00000000,
  B00000000, B00000000,
  B00000000, B00000000,
  B00000000, B00000000,
  B00000000, B00000000,
  B00000000, B00000000,
  B00000000, B00000000,
  B00000000, B00000000,
  B00000000, B00000000};
/**********************************************************
 * Setup
 * Initialization of the main components and variables
 **********************************************************/
void setup()
{
  Serial.begin(9600);
  /*if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }*/
  // MPU6050 initialization
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    Wire.begin();
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
    Fastwire::setup(400, true);
  #endif
  
  Serial.println("Initializing I2C devices...");
  accelgyro.initialize();
  // verify connection
  Serial.println("Testing device connections...");
  Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
  
  // Radio initialization
  if (!driver.init())
    Serial.println("init failed");

  display.display();
  delay(2000);
  
  // PIN initialization
  pinMode(LIGHTS_BUTTON, INPUT);
  pinMode(BRAKE_BUTTON, INPUT);
  
}



void drawbitmap(int logo) {
  int16_t p_x,p_y;
  p_y = 64/2;

  if(logo == 0){
    p_x=(128/3)-(128/6);
      display.drawBitmap(p_x,p_y,LEFT_LOGO, LOGO_WIDTH, LOGO_HEIGHT, 1);
  }
  else if(logo == 2){
    p_x=((128/3)*2)-(128/6);
      display.drawBitmap(p_x,p_y,RIGHT_LOGO, LOGO_WIDTH, LOGO_HEIGHT, 1);
  }
  else if(logo == 3){
    p_x=((128/3)*2)-(128/6);
      display.drawBitmap(p_x,p_y,LIGHTS_LOGO, LOGO_WIDTH, LOGO_HEIGHT, 1);
  }
}

void erasebitmap(int logo) {
  int p_x,p_y;
  p_y = 64/2;
  unsigned char PROGMEM logo_bmp;
  if(logo == 0){
    p_x=(128/3)-(128/6);
    logo_bmp = NULL_LOGO;
  }
  else if(logo == 2){
    p_x=((128/3)*2)-(128/6);
    logo_bmp = NULL_LOGO;
  }
  else if(logo == 3){
    p_x=((128/3)*2)-(128/6);
    logo_bmp = NULL_LOGO;
  }

  display.drawBitmap(p_x,p_y,(uint8_t *) logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);
}

/******************************************************
 *  updateFiltered
 *  Update MPU 'y' angle using complementary filter
 ******************************************************/
void updateFiltered()
{
  dt = (millis() - prev_time) / 1000.0;
  prev_time = millis();

  //Acelerometer angle calculation
  float accel_ang_x = atan(ay / sqrt(pow(ax, 2) + pow(az, 2))) * (180.0 / 3.14);
//  float accel_ang_y = atan(-ax / sqrt(pow(ay, 2) + pow(az, 2))) * (180.0 / 3.14);

  //Gyroscope and complementary filter rotation angle calculation
  ang_x = 0.98 * (ang_x_prev + (gx / 131) * dt) + 0.02 * accel_ang_x;

  ang_x_prev = ang_x;
}

/******************************************************
 * sendMessage(message to be sent)
 * Sends the message given as a parameter via the driver
 ******************************************************/
void sendMessage(char * message) {
  Serial.print("Enviando");
  Serial.println(message);
  driver.send((uint8_t *)message, strlen(message));
  driver.waitPacketSent();
}

/*******************************************************
 * Main loop
 * Check for MPU and button status and send messages 
 * accordingly
 *******************************************************/
void loop()
{
  //Serial.print("Angulo = ");
  //Serial.println(ang_y);
  accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  updateFiltered();

///////////////////////////////////////////////////////INTERMITENTE IZQ/////////
  if (ang_x > 45) {
    if(!active_left){
      sendMessage("0");
      active_left = true;
      drawbitmap(0);
    }
  }
  else {
    erasebitmap(0);
    active_left = false;
  }

///////////////////////////////////////////////////////INTERMITENTE DER/////////
  if (ang_x < -45) {
    if(!active_right){
      sendMessage("1");
      active_right = true;
      drawbitmap(1);
    }
  }
  else {
    erasebitmap(1);
    active_right = false;
  }

///////////////////////////////////////////////////////LUCES///////////////////
  if (digitalRead(LIGHTS_BUTTON)==LOW) {
    if(!active_lights){
      sendMessage("2");
      active_lights = true;
      drawbitmap(2);
    }
  }
  else {
    erasebitmap(2);
    active_lights = false;
  }


///////////////////////////////////////////////////////FRENO///////////////////
  if (digitalRead(BRAKE_BUTTON)==LOW) {
    if(!active_brake){
      sendMessage("3");
      active_brake = true;
    }
  }
  else {
    if(active_brake){
      sendMessage("3");
      active_brake = false;
    }
  }
  display.display();
  delay(1);

}
