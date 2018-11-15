//SDA - Pin A4
//SCL - Pin A5
#include <RH_ASK.h>
#include <SPI.h> // Not actually used but needed to compile
#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"

// Defines for Arduino PINs
#define LEFT_BLINKER 7
#define RIGHT_BLINKER 8
#define FRONT_LIGHT 10
#define BRAKE_LIGHT 11
#define LIGHTS_BUTTON 2
#define BRAKE_BUTTON 3

// Defines for message codes
#define RIGHT_MESSAGE 0
#define LEFT_MESSAGE 1
#define LIGHTS_MESSAGE 2
#define BRAKE_MESSAGE 3

// Variables for the MPU6050
MPU6050 accelgyro;

int ax, ay, az;
int gx, gy, gz;

long prev_time;
float dt;
float ang_y;
float ang_y_prev;

// Variables for the Radio
RH_ASK driver(2000, 2, 5, 10);

// Bool for lights status controlling
bool turn_on = false;
bool active = false;
bool active_left = false;
bool active_right = false;
bool lights = false;
bool brake = false;

void setup()
{
  // Serial for debugging
  Serial.begin(9600);

  // MPU6050 initialization
  /*Wire.begin();
    mpu.initialize();
    Serial.println(mpu.testConnection() ? F("IMU iniciado correctamente") : F("Error al iniciar IMU"));*/
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(400, true);
#endif

    // initialize device
    Serial.println("Initializing I2C devices...");
    accelgyro.initialize();

    // verify connection
    Serial.println("Testing device connections...");
    Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

  //Radio initialization
  if (!driver.init())
    Serial.println("init failed");

  // PIN initialization
  pinMode(LEFT_BLINKER, OUTPUT);
  pinMode(RIGHT_BLINKER, OUTPUT);
  pinMode(LIGHTS_BUTTON, INPUT_PULLUP);
  pinMode(FRONT_LIGHT, OUTPUT);
  pinMode(BRAKE_LIGHT, OUTPUT);

  // PIN initial values
  digitalWrite(LEFT_BLINKER, LOW);
  digitalWrite(RIGHT_BLINKER, LOW);
  digitalWrite(FRONT_LIGHT, LOW);
  digitalWrite(BRAKE_LIGHT, LOW);

  // Interrupt initialization
  attachInterrupt(digitalPinToInterrupt(LIGHTS_BUTTON), lightsSwitch, CHANGE);
  attachInterrupt(digitalPinToInterrupt(BRAKE_BUTTON), brakeActivation, CHANGE);
}

void lightsSwitch(){

  if(digitalRead(LIGHTS_BUTTON) == HIGH)
    lights = true;
  
}

void brakeActivation(){

  brake = true;
    
}


void updateFiltered()
{
  dt = (millis() - prev_time) / 1000.0;
  prev_time = millis();

  //Acelerometer angle calculation
  float accel_ang_y = atan(-ax / sqrt(pow(ay, 2) + pow(az, 2))) * (180.0 / 3.14);

  ang_y = 0.98 * (ang_y_prev + (gy / 131) * dt) + 0.02 * accel_ang_y;

  ang_y_prev = ang_y;
}

void Blinkers(int LED, bool on) {
  (on) ? digitalWrite(LED, HIGH) : digitalWrite(LED, LOW);
}

void sendMessage(int message) {
  char *msg;
  switch (message) {
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

  accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  updateFiltered();
  /*
  Serial.print(F("\t Rotacion en Y: "));
  Serial.println(ang_y);
  */
  ///////////////////////////////////////////////////////INTERMITENTES/////////
  if (ang_y > 45) {
    if (!active) {
      sendMessage(RIGHT_MESSAGE);
      active = true;
    }
  }
  else if (ang_y < -45) {
    if (!active) {
      sendMessage(LEFT_MESSAGE);
      active = true;
    }
  }
  else {
    active = false;
  }

  if(lights){
    sendMessage(LIGHTS_MESSAGE);
    lights = false;
  }
  
  if(brake){
    sendMessage(BRAKE_MESSAGE);
    brake = false;
  }

  delay(50);
}
