#include <Servo.h>
#include <RobotCore.h>

const int motor1Pin1 = 3;    // 1A
const int motor1Pin2 = 5;    // 2A
const int motor2Pin1 = 6;    // 1B
const int motor2Pin2 = 9;    // 2B
const int enable1Pin = 10;    // A enable pin
const int enable2Pin = 11;    // B enable pin

RobotCore myBot;

void setup ()
{
  myBot.hbridge(motor1Pin1, motor1Pin2, enable1Pin, motor2Pin1, motor2Pin2, enable2Pin); //configure H-Bridge
}

void loop ()
{
  uint16_t i;
  myBot.drive(255, 0, 5); //Forward
  for(i=0; i<5000; i++) { //ramp up speed, 5 seconds
    myBot.speedCheck();
    delay(1);
  }
  myBot.drive(0, 0, 5); //stop
  for(i=0; i<5000; i++) {  //ramp down speed, 5 seconds
    myBot.speedCheck();
    delay(1);
  }
}
