#include <RH_ASK.h>
#include <SPI.h> // Not actually used but needed to compile

#include "I2Cdev.h"
RH_ASK driver(2000,2,9,10);
#define BOTON 12
#define RIGHT_BLINKER 0
#define LEFT_BLINKER 1
#define LIGHTS 2
#define BRAKE 3
#define LIGHTS_BUTTON 5
#define BRAKE_BUTTON 6


void setup()
{
  Serial.begin(9600); // Debugging only
 
  /* RADIO AND GYROSCPE INITIALIZATION */
  if (!driver.init())
    Serial.println("init failed");
  Wire.begin();
  mpu.initialize();
  Serial.println(mpu.testConnection() ? F("IMU iniciado correctamente") : F("Error al iniciar IMU"));
  
  
  /* PIN CONFIG */
  pinMode(LIGHTS_BUTTON,INPUT);
  digitalWrite(LIGHTS_BUTTON, LOW);

  pinMode(BRAKE_BUTTON,INPUT);
  digitalWrite(BRAKE_BUTTON, LOW);
  
  /* INTERRUPTION FUNCTS DECLARATION */
  attachInterrupt(digitalPinToInterrupt(5),interruptBrakeHandler,CHANGE);
  attachInterrupt(digitalPinToInterrupt(6),interruptLightsHandler,CHANGE);
}


void updateFiltered()
{
  dt = (millis() - prev_time) / 1000.0;
  prev_time = millis();

  //Acelerometer angle calculation
  float accel_ang_x = atan(ay / sqrt(pow(ax, 2) + pow(az, 2)))*(180.0 / 3.14);
  float accel_ang_y = atan(-ax / sqrt(pow(ay, 2) + pow(az, 2)))*(180.0 / 3.14);

  //Gyroscope and complementary filter rotation angle calculation
  ang_x = 0.98*(ang_x_prev + (gx / 131)*dt) + 0.02*accel_ang_x;
  ang_y = 0.98*(ang_y_prev + (gy / 131)*dt) + 0.02*accel_ang_y;

  ang_x_prev = ang_x;
  ang_y_prev = ang_y;
}

void interruptBrakeHandler(){
  send_message(BRAKE);
}

void interruptLightsHandler(){
  send_message(LIGHTS);
}

 /* Message sending handler
 * Args: -id , the type of message that will be sent
 * Return void;
 */
void sendMessage(int id){
   switch(id){
    
    /* Activate right blinker */
    case RIGHT_BLINKER:
      msg = "0";
      driver.send((uint8_t *)msg, strlen(msg));
      driver.waitPacketSent();
    break;
   
    /* Activate left blinker */
    case LEFT_BLINKER:
      msg = "1";
      driver.send((uint8_t *)msg, strlen(msg));
      driver.waitPacketSent();
    break;
   
    /* Activate lights */
    case LIGHTS:
      msg = "2";
      driver.send((uint8_t *)msg, strlen(msg));
      driver.waitPacketSent();
    break;

    /* Activate brake light */
    case BRAKE:
      msg = "3";
      driver.send((uint8_t *)msg, strlen(msg));
      driver.waitPacketSent();
    break;
    
   }
   msg="";
}

void loop()
{

  /*MPU Handler */

  
  mpu.getAcceleration(&ax, &ay, &az);
  mpu.getRotation(&gx, &gy, &gz);

  updateFiltered();

  //Serial.print(F("Rotacion en X: "));
  //Serial.print(ang_x);
  //Serial.print(F("\t Rotacion en Y: "));
  //Serial.println(ang_y);

  //////////////////BUTTONS AND MOVEMENT DETECTION///////////////////////////////////////////////////////
  if(ang_y>45){
    send_message(RIGHT_BLINKER); 
  }
  else if(ang_y<-45){
    send_message(LEFT_BLINKER);   
  }
 
delay(50);

}
