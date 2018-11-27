//SDA - Pin A4
//SCL - Pin A5
#include <RH_ASK.h>
#include <SPI.h> // Not actually used but needed to compile
#include <I2Cdev.h>
#include <MPU6050.h>
#include <Wire.h>

// Defines for Arduino PINs
#define LIGHTS_BUTTON 7
#define BRAKE_BUTTON 6
#define LEFT_BLINKER 8
#define RIGHT_BLINKER 9

// Defines for message codes
#define RIGHT_MESSAGE 0
#define LEFT_MESSAGE 1
#define LIGHTS_MESSAGE 2
#define BRAKE_MESSAGE 3

// Variables for the MPU6050
//const int mpuAddress = 0x68;
//MPU6050 mpu(mpuAddress);

//int ax, ay, az;
//int gx, gy, gz;

//long prev_time;
//float dt;
//float ang_y;
//float ang_y_prev;

// Variables for the Radio
RH_ASK driver(2000,2,5,10);

// Bool for lights status controlling
bool turn_on = false;
bool active = false;
bool active_left = false;
bool active_right = false;
bool lights = false;

void setup()
{
  // Serial for debugging
  Serial.begin(9600);

  // MPU6050 initialization
//  Wire.begin();
//  mpu.initialize();
//  Serial.println(mpu.testConnection() ? F("IMU iniciado correctamente") : F("Error al iniciar IMU"));

  //Radio initialization
  if (!driver.init())
    Serial.println("init failed");
    
  // PIN initialization
  pinMode(LEFT_BLINKER, INPUT);
  pinMode(RIGHT_BLINKER, INPUT);
  pinMode(LIGHTS_BUTTON, INPUT);
  pinMode(BRAKE_BUTTON, INPUT);

  // Interrupt initialization
  attachInterrupt(digitalPinToInterrupt(LIGHTS_BUTTON), lightsSwitch, FALLING);
  attachInterrupt(digitalPinToInterrupt(BRAKE_BUTTON), brakeToggle, CHANGE);

}

// Update MPU angle using complementary filter
/*void updateFiltered()
{
  dt = (millis() - prev_time) / 1000.0;
  prev_time = millis();

  //Acelerometer angle calculation
  float accel_ang_y = atan(-ax / sqrt(pow(ay, 2) + pow(az, 2))) * (180.0 / 3.14);

  ang_y = 0.98 * (ang_y_prev + (gy / 131) * dt) + 0.02 * accel_ang_y;

  ang_y_prev = ang_y;
}*/

void lightsSwitch(){
  sendMessage(LIGHTS_MESSAGE);
}

void brakeToggle(){
  sendMessage(LIGHTS_MESSAGE);
}

void sendMessage(int message){
  char *msg;
  switch (message){
    case RIGHT_MESSAGE:
      msg = "0";
      driver.send((uint8_t *)msg, strlen(msg));
      driver.waitPacketSent();
      Serial.print("Enviamos: ");
      Serial.println(msg);
      break;
      
    case LEFT_MESSAGE:
      msg = "1";
      driver.send((uint8_t *)msg, strlen(msg));
      driver.waitPacketSent();
      Serial.print("Enviamos: ");
      Serial.println(msg);
      break;
      
    case LIGHTS_MESSAGE:
      msg = "2";
      driver.send((uint8_t *)msg, strlen(msg));
      driver.waitPacketSent();
      Serial.print("Enviamos: ");
      Serial.println(msg);
      break;
      
    case BRAKE_MESSAGE:
      msg = "3";
      driver.send((uint8_t *)msg, strlen(msg));
      driver.waitPacketSent();
      Serial.print("Enviamos: ");
      Serial.println(msg);
      break;
  }
}

void loop()
{

  /*mpu.getAcceleration(&ax, &ay, &az);
  mpu.getRotation(&gx, &gy, &gz);

  updateFiltered();

  Serial.print(F("\t Rotacion en Y: "));
  Serial.println(ang_y);*/

  ///////////////////////////////////////////////////////INTERMITENTES/////////
  if (digitalRead(RIGHT_BLINKER)==LOW) {
    if(!active){
      sendMessage(RIGHT_MESSAGE);
      active = true;
    }
  }
  else if (digitalRead(LEFT_BLINKER)==LOW) {
    if(!active){
      sendMessage(LEFT_MESSAGE);
      active = true;
    }
  }
  else{
    active = false;
  }

  delay(50);
}
