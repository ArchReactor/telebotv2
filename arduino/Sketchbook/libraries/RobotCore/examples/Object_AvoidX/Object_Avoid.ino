#include <Carduino.h>

Carduino mycarduino;
int turnnum = 0;
int turnvalue = 0;
void setup ()
{
  mycarduino.begin();
  randomSeed(analogRead(3));
}

void loop ()
{
  mycarduino.goforward(0.1, 1023);
  if (mycarduino.proximity() < 25)
  {
    turnnum = random(2);
    turnvalue = random(900, 2300);
    if (turnnum == 0)
    {
      mycarduino.turnright(turnvalue);
    }
    if (turnnum == 1)
    {
      mycarduino.turnleft(turnvalue);
    }
  }
}