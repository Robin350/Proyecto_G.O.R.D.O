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

// Defines for Arduino PINs
#define LIGHTS_BUTTON 2
#define BRAKE_BUTTON 3
#define LEFT_BLINKER 8
#define RIGHT_BLINKER 9

// Defines for message codes
#define RIGHT_MESSAGE 0
#define LEFT_MESSAGE 1
#define LIGHTS_MESSAGE 2
#define BRAKE_MESSAGE 3

// Variables for the MPU6050
const int mpuAddress = 0x68;
MPU6050 mpu(mpuAddress);
int ax, ay, az;
int gx, gy, gz;
long prev_time;
float dt;
float ang_y;
float ang_y_prev;

// Radio
RH_ASK driver(2000, 2, 5, 10);

// Bool for lights status controlling
bool active_lights = false;
bool active_brake = false;
bool active_left = false;
bool active_right = false;

/**********************************************************
 * Setup
 * Initialization of the main components and variables
 **********************************************************/
void setup()
{
  // MPU6050 initialization
  MPU6050 initialization
  Wire.begin();
  mpu.initialize();

  // Radio initialization
  if (!driver.init())
    Serial.println("init failed");

  // PIN initialization
  pinMode(LEFT_BLINKER, INPUT);
  pinMode(RIGHT_BLINKER, INPUT);
  pinMode(LIGHTS_BUTTON, INPUT);
  pinMode(BRAKE_BUTTON, INPUT);
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
  float accel_ang_y = atan(-ax / sqrt(pow(ay, 2) + pow(az, 2))) * (180.0 / 3.14);

  ang_y = 0.98 * (ang_y_prev + (gy / 131) * dt) + 0.02 * accel_ang_y;

  ang_y_prev = ang_y;
}

/******************************************************
 * sendMessage(message to be sent)
 * Sends the message given as a parameter via the driver
 ******************************************************/
void sendMessage(int message) {
  driver.send((uint8_t *)message, strlen(message));
  diver.waitPacketSent();
}

/*******************************************************
 * Main loop
 * Check for MPU and button status and send messages 
 * accordingly
 *******************************************************/
void loop()
{

  mpu.getAcceleration(&ax, &ay, &az);
  mpu.getRotation(&gx, &gy, &gz);

  updateFiltered();

///////////////////////////////////////////////////////INTERMITENTE IZQ/////////
  if (ang_y > 45) {
    if(!active_left){
      sendMessage(LEFT_MESSAGE);
      active_left = true;
    }
  }
  else {
    active_left = false;
  }

///////////////////////////////////////////////////////INTERMITENTE DER/////////
  if (ang_y < 45) {
    if(!active_right){
      sendMessage(RIGHT_MESSAGE);
      active_right = true;
    }
  }
  else {
    active_right = false;
  }

///////////////////////////////////////////////////////LUCES///////////////////
  if (digitalRead(LIGHTS_BUTTON)==LOW) {
    if(!active_lights){
      sendMessage(LIGHTS_MESSAGE);
      active_lights = true;
    }
  }
  else {
    active_lights = false;
  }


///////////////////////////////////////////////////////FRENO///////////////////
  if (digitalRead(BRAKE_BUTTON)==LOW) {
    if(!active_brake){
      sendMessage(BRAKE_MESSAGE);
      active_brake = true;
    }
  }
  else {
    if(active_brake){
      sendMessage(BRAKE_MESSAGE);
      active_brake = false;
    }
  }
}
