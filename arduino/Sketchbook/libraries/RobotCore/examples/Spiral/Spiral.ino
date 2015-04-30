#include <Servo.h>
#include <RobotCore.h>

const int motor1Pin1 = 3;    // 1A
const int motor1Pin2 = 5;    // 2A
const int motor2Pin1 = 6;    // 1B
const int motor2Pin2 = 9;    // 2B
const int enable1Pin = 10;    // A enable pin
const int enable2Pin = 11;    // B enable pin

RobotCore myBot;

float distance = 0;

void setup()
{
  myBot.hbridge(motor1Pin1, motor1Pin2, enable1Pin, motor2Pin1, motor2Pin2, enable2Pin); //configure H-Bridge
}

void loop()
{
  distance += 0.1;
  myBot.drive(255, 0); //Forward
  delay(1000 * distance);
  myBot.drive(0, 127); //stopped, 127=spin in place to the right at half speed
  delay(250);
}
