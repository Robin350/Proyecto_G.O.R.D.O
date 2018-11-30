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

// Defines for arduino pins
#define RIGHT 12
#define LEFT 10
#define BRAKE_LIGHT 11
#define FRONT_LIGHT 13
#define DATA_R 4

// Defines for message codes
#define RIGHT_BLINKER 0
#define LEFT_BLINKER 1
#define LIGHTS 2
#define BRAKE 3

// Radio
RH_ASK driver(2000,DATA_R,2,10);

// Buffer to be recieved
uint8_t buf[5];
uint8_t buflen = sizeof(buf);

// Bools for controlling updates on lights
bool fr,br,bl; 

// True when a message is available
bool thereIsMessage; 

// Light status controlling
bool lights,brake;
char blinker = '0'; 

/**********************************************************
 * Setup
 * Initialization of the main components and variables
 **********************************************************/
void setup()
{
    // Radio initialization
    driver.init();

    // Pin initialization
    pinMode(LEFT,OUTPUT);
    pinMode(RIGHT,OUTPUT);
    pinMode(BRAKE_LIGHT,OUTPUT);
    pinMode(FRONT_LIGHT,OUTPUT);

    // Timer interrupt initialization
    SREG = (SREG & 0b01111111); //Desabilitar interrupciones
    TIMSK2 = TIMSK2|0b00000001; //Habilita la interrupcion por desbordamiento
    TCCR2B = 0b00000111; //Configura preescala para que FT2 sea de 7812.5Hz
    SREG = (SREG & 0b01111111) | 0b10000000; //Habilitar interrupciones //Desabilitar interrupciones

    thereIsMessage=lights=brake= false;
    digitalWrite(LEFT,LOW);
    digitalWrite(RIGHT,LOW);
    digitalWrite(BRAKE_LIGHT,LOW);
    digitalWrite(FRONT_LIGHT,LOW);
}

/**********************************************************
 * Interrupt handler for Timer 2
 * It checks if theres and message and if so, saves it in 
 * buf and puts thereIsMessage to true
 **********************************************************/
ISR(TIMER2_OVF_vect){

  if(driver.available()){
    driver.recv(buf, &buflen);
    thereIsMessage = true;
  }
}

/**********************************************************
 * Brake handling function
 * Turns on or off the brakes
 **********************************************************/
void Brake(){
    if(brake){
      digitalWrite(BRAKE_LIGHT,HIGH);
    }
    else{
      digitalWrite(BRAKE_LIGHT,LOW);
    }
    br = false;
}

/**********************************************************
 * Front lights handling function
 * Turns on or off the lights
 **********************************************************/
void Front(){
    if(lights){
      digitalWrite(FRONT_LIGHT,HIGH);
    }
    else{
      digitalWrite(FRONT_LIGHT,LOW);
    }
    fr = false;
}


/**********************************************************
 * Blinker handling
 * Turns on or off the blinker 'blinker' indicates
 **********************************************************/
void Blink(){
    
  if(blinker == 'l'){
    digitalWrite(LEFT,HIGH);
    digitalWrite(RIGHT,LOW);
  }
  else if(blinker == 'r'){
    digitalWrite(RIGHT,HIGH);
    digitalWrite(LEFT,LOW);
  }
  else{
    digitalWrite(LEFT,LOW);
    digitalWrite(RIGHT,LOW);
  }
  bl = false;
  
}

/**********************************************************
 * Lights updating
 * Checks if the state of the lights have been changed and 
 * updates them
 **********************************************************/
void updateLights(){
  if(fr)
    Front();
  if(br)
    Brake();
  if(bl);
    Blink();
}

/**********************************************************
 * Message updating
 * Proccess the message currently saved on 'buf'
 **********************************************************/
void newMessage(){
  int num = atoi((const char*) buf);
  switch(num){
     case RIGHT_BLINKER:          
      if(blinker != 'r'){
        blinker = 'r';
      }
      else{
        blinker = '0';
      }
      bl =true;
      break;

     case LEFT_BLINKER:
      if(blinker != 'l'){
        blinker = 'l';
      }
      else{
        blinker = '0';
      }
      bl = true;
      break;

     case LIGHTS:
      lights = !lights;
      fr = true;
      break;

     case BRAKE:
      brake = !brake;
      br = true;
      break;
  }
}

/**********************************************************
 * Main loop
 * Check for message updates and updates lights.
 * If there is a message we desactivate interrupts so
 * the timer handler doesnt change 'buf' while we read it 
 **********************************************************/
void loop()
{
    if(thereIsMessage){
      cli();
      thereIsMessage = false;
      newMessage();
      updateLights();
      sei();      
    }
}
