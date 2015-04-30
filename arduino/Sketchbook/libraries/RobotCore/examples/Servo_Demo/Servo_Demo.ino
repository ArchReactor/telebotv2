#include <Servo.h>
#include <RobotCore.h>

const int motor1Pin = 9;    // right servo
const int motor2Pin = 10;    // left servo

RobotCore myBot;

void setup ()
{
  myBot.servo(motor1Pin, motor2Pin, 1000, 2000); //configure servo
}

void loop ()
{
  myBot.drive(255, 0); //Forward
  delay(1000);
  myBot.dirSet(0, 0); //stop
  delay(1000); 
}
