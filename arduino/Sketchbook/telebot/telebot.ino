/*
This is for the Sabertooth motor controller in Servo mode

serial protocol info:
inputs, no delimiters:
  M - motor commands
    1-9 - directions match keyboard numberpad directions, 5 is STOP
    Example:  "M7" - go forward and left
  P - program
    0 - manual drive
    1 - automatic
    2 - follow
  S - set speed.  does not take effect until the next motor move target is evaluated
    1 - full speed
    2 - 75%
    3 - 50%
    4 - 25%
  D - debug
    0 - none
    1 - ping sensor info
    Example: "D1" - enable ping debug

outputs, ":" delimited for everything, newline terminated
  I - information
    string - textual information
    Example: "I:done!" - startup done
  S - Stop info
    integer - sensor index (0-based)
    integer - distance cm
    Example: "S:2:71" - stop condition, 3rd sensor, at 71cm
  W - info for web interface
    char - command
    char - value
  P - power info
    integer - direction
    integer - left target
    integer - right target
    integer - left actual
    integer - right actual
    Example: "P:8:255:255:128:0" - dir is forward, target is 255/255 (fwd strait) current is 128/0 (turning right hard)
  E - Error
    string - textual information
    Example: "E:Invalid Move" - input something not 1-9 after M
  D - debug info
    P - ping sensors
      integer - sensor index (0-based)
      integer - distance cm
      Example: "D:P:1:123" - debug ping, 2nd sensor at 123cm
    
*/
#include <SPI.h>  
//#include <Pixy.h>
#include <Servo.h>
#include <RobotCore.h>
//#include <NewPing.h>
#include <IRremote.h>

#define MAX_DISTANCE 250 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
#define MODE0_LIMIT 10      // max distance that it's OK to move
#define MODE1_TURN 110

#define X_CENTER    160L  //pixy center line

#define NUM_PINGS 4

//direction constants
#define BACK_LEFT 	1
#define BACK		2
#define BACK_RIGHT	3
#define SPIN_LEFT	4
#define STOP		5
#define SPIN_RIGHT	6
#define FWD_LEFT	7
#define FWD		8
#define FWD_RIGHT	9

/*enum SonarNames {
  ping0 = 0,
  ping45 = 1,
  ping90 = 2,
  ping135 = 3,
  ping180 = 4
};

NewPing sonar[NUM_PINGS] = {
  NewPing(25, 23, MAX_DISTANCE), // 60 degree
  NewPing(29, 27, MAX_DISTANCE), // 90 degree
  NewPing(33, 31, MAX_DISTANCE), // 120 degree
  NewPing(37, 35, MAX_DISTANCE) // 160 degree
};
uint8_t pingCurrent = 255; //255 means not pinging
volatile uint8_t pingNext = 255;
boolean pingDebug = false;
uint8_t lastTrigger = 255;
*/

uint8_t maxPwr = 255;
int lastPwr = 0;
int curPwr = 0;
int curDir = 0;
uint8_t roverMode = 0;
boolean powerEnable = true;
unsigned long lastCommand = 0;
RobotCore bot;
//Pixy pixy;

const int RECV_PIN = 21;
IRrecv irrecv(RECV_PIN);
decode_results IRresults;

void setup() {
  Serial.begin(115200);
  Serial.println("I:starting...");

  bot.servo(52, 53, 1000, 2000);

  irrecv.enableIRIn(); // Start the receiver
  
  delay(500);
  Serial.println("I:done!");

  setDir(STOP);
  setSpd('2');
}

void loop() 
{
//  pingCheck();
  
  switch(roverMode) {
   case 0:
    checkMode0();
    break;
   case 1:
    checkMode1();
    break;
   case 2:
    checkMode2();
    break;
  }
  
  checkSerial();
  
  checkIR();
  
  applyPower();
}
/*
void echoCheck() { // timer's up, do next sensor
  if(sonar[pingCurrent].check_timer() != 0) {
    pingNext = pingCurrent + 1;
    if (pingNext >= NUM_PINGS)
      pingNext = 0;
  }
}

void pingCheck() {
  if(pingNext < NUM_PINGS) {
    if (pingCurrent < NUM_PINGS) {
      sonar[pingCurrent].timer_stop();
    }
    pingCurrent = pingNext;
    pingNext = 255;
    sonar[pingCurrent].ping_timer(echoCheck); // Do the ping (processing continues, interrupt will call echoCheck to look for echo).
  }
}
*/
void checkMode0() { //simply halt if too close
/*  lastTrigger = 255;
  for (uint8_t i = 0; i < NUM_PINGS; i++) { // Loop through the sensors to see what's triggered
    if(pingDebug) {  //ping debug packet:  D:P:sensor index:distance cm
      Serial.print("D:P:"); Serial.print(i); Serial.print(":");Serial.println(sonar[i].ping_result / US_ROUNDTRIP_CM);
    }
    
    if (sonar[i].ping_result / US_ROUNDTRIP_CM > 0 && sonar[i].ping_result / US_ROUNDTRIP_CM < MODE0_LIMIT) {
      //S is e-stop, format is S:sensor index:distance cm
      Serial.print("S:"); Serial.print(i); Serial.print(":");Serial.println(sonar[i].ping_result / US_ROUNDTRIP_CM);
      switch(curDir) {
        case 7: 
        case 8:
        case 9: //only when going forward
          lastTrigger = i;
          powerEnable = false;
          bot.drive(0, curDir);
          break;
      }
      break; //only need one to be over
    }
  }
  if(lastTrigger == 255 && !powerEnable) { //it passed after a failure
    powerEnable = true;
    Serial.println("S::"); //empty estop packet to clear it
  }
  */
}

void setDir(uint8_t dir) {
  char cmd = ' ', val = ' ';

  switch(dir) { //validate
    case STOP:
      cmd = 'D'; val = '5';
      curDir = 0;
      curPwr = 0;  //need to set speed for a stop
      break;
    case BACK_LEFT:
      cmd = 'D'; val = '1';
      curDir = 0-lastPwr;
      curPwr = 0-lastPwr;
      break;
    case BACK:
      cmd = 'D'; val = '2';
      curDir = 0;
      curPwr = 0-lastPwr;
      break;
    case BACK_RIGHT:
      cmd = 'D'; val = '3';
      curDir = 0-lastPwr;
      curPwr = lastPwr;
      break;
    case SPIN_LEFT:
      cmd = 'D'; val = '4';
      curDir = 0-(lastPwr/1.5);
      curPwr = 0;
      break;
    case SPIN_RIGHT:
      cmd = 'D'; val = '6';
      curDir = (lastPwr/1.5);
      curPwr = 0;
      break;
    case FWD_LEFT:
      cmd = 'D'; val = '7';
      curDir = 0-lastPwr;
      curPwr = lastPwr;
      break;
    case FWD:
      cmd = 'D'; val = '8';
      curDir = 0;
      curPwr = lastPwr;
      break;
    case FWD_RIGHT:
      cmd = 'D'; val = '9';
      curDir = lastPwr;
      curPwr = lastPwr;
      break;
    default: //same as stop
      cmd = 'D'; val = '5';
      curDir = 0;
      curPwr = 0;
      Serial.println("E:Invalid Move");
  }
  if(cmd != ' ') {
    Serial.print("W:"); 
    Serial.print(cmd); 
    Serial.print(":"); 
    Serial.println(val); 
  }
//  Serial.print("E:dir:"); Serial.println(curDir);
//  Serial.print("E:pwr:"); Serial.println(curPwr);
  powerEnable = true;
}

void setSpd(char spd) {
  char cmd = ' ', val = ' ';

  switch(spd) {
    case '1': //25%
      cmd = 'S'; val = '1';
      curPwr = maxPwr * 0.25;
      lastPwr = curPwr;
      break;
    case '2': //50%
      cmd = 'S'; val = '2';
      curPwr = maxPwr * 0.50;
      lastPwr = curPwr;
      break;
    case '3': //75%
      cmd = 'S'; val = '3';
      curPwr = maxPwr * 0.75;
      lastPwr = curPwr;
      break;
    case '4': //100%
      cmd = 'S'; val = '4';
      curPwr = maxPwr;
      lastPwr = curPwr;
      break;
  }
  if(cmd != ' ') {
    Serial.print("W:"); 
    Serial.print(cmd); 
    Serial.print(":"); 
    Serial.println(val); 
  }
  powerEnable = true;
}


void checkSerial() {
  if(Serial.available()) {            // Is data available from Internet
    char cmd;
    //keys match number pad directions
    cmd = Serial.read();
    switch(cmd) { //read 1st byte
      case 'M': //move
        while(!Serial.available());
        setDir(Serial.read() - '0');;  //read 2nd byte, convert char number to int
        //Serial.print("E:dir:"); Serial.println(curDir);
        break;
      case 'P': //program
        switch(Serial.read()) { //read 2nd byte
          case '0': //drive
            roverMode = 0;
            curDir = 0;
            curPwr = 0;
            maxPwr = 255;
            break;
          case '1': //auto
            roverMode = 1;
            maxPwr = 255;
            curPwr = maxPwr * 0.20;
            lastPwr = curPwr;
//            pingDebug = true;
            break;
          case '2': //follow
            roverMode = 2;
            maxPwr = 255;
            curPwr = maxPwr * 0.25;
            lastPwr = curPwr;
//            pingDebug = true;
            break;
        }
        break;
      case 'S': //speed
        while(!Serial.available());
        setSpd(Serial.read());
        break;
      case 'D': //debug?
        switch(Serial.read()) { //read 2nd byte
          case '0':
//            pingDebug = false;
            break;
          case '1': //enable
//            pingDebug = true;
            break;
        }
        break;
      default:
        Serial.print("E:Invalid command "); Serial.println(cmd);
        Serial.flush();
    }
  }
}

void checkIR() {
  if (irrecv.decode(&IRresults)) {
    //"curtis" remote
    switch(IRresults.value) {
      case 0xFF00906F: //enter
        setDir(STOP);
        break;
      case 0xFF0050AF: //up 
        setDir(FWD);
        break;
      case 0xFF00B04F: //right 
        setDir(SPIN_RIGHT);
        break;
      case 0xFF0010EF: //down 
        setDir(BACK);
        break;
      case 0xFF00A05F: //left 
        setDir(SPIN_LEFT);
        break;
      case 0xFF008877: //rev
        setSpd('1');
        break;
      case 0xFF00A857: //fwd 
        setSpd('2');
        break;
      case 0xFF0030CF: //stop 
        setSpd('3');
        break;
      case 0xFF0048B7: //prev 
        break;
      case 0xFF006897: //next 
        break;
      case 0xFF00708F: //play 
        break;
      case 0xFF0008F7: //rep 
        setDir(BACK_LEFT);
        break;
      case 0xFF00609F: //tft 
        setDir(BACK_RIGHT);
        break;
      case 0xFF0020DF: //menu 
        setDir(FWD_LEFT);
        break;
      case 0xFF0028D7: //zoom 
        setSpd('4');
        break;
      case 0xFF0040BF: //power 
        setDir(STOP);
        break;
      case 0xFF00B847: //16:9
        break;
      case 0xFF0018E7: //snooze 
        break;
      case 0xFF0038C7: //rotate 
        break;
      case 0xFF0058A7: //vol+ 
        break;
      case 0xFF009867: //vol- 
        setDir(FWD_RIGHT);
        break;
      case 0xFFFFFFFF: //key repeat
        break;
    }
    irrecv.resume(); // Receive the next value
  }
}
void applyPower() {
  if (powerEnable) {
    //Serial.print("E:pwr:"); Serial.println(curPwr);
    bot.drive(curPwr, curDir, 5);
    powerEnable = false;
    lastCommand = millis();
  }
  bot.speedCheck();
  if(lastCommand + 5000 < millis()) {
    if(!(curPwr==0 && curDir==0)) {
      setDir(STOP);
    }
    lastCommand = millis();
  }
//  Serial.print("D:speed ramp L "); Serial.print(bot.spdTgtL); Serial.print(":"); Serial.println(bot.spdCurL);
//  Serial.print("D:speed ramp R "); Serial.print(bot.spdTgtR); Serial.print(":"); Serial.println(bot.spdCurR);
  //power info format: P:direction:left target:right target:left actual:right actual
//  Serial.print("P:"); Serial.print(curDir); 
//  Serial.print(":"); Serial.print(bot.lPower(curPwr)); Serial.print(":"); Serial.print(bot.rPower(curPwr)); 
//  Serial.print(":"); Serial.print(bot.lPower()); Serial.print(":"); Serial.println(bot.rPower()); 
}

void checkMode1() {
/*  boolean bAllClear = true;
  lastTrigger = 255;
  for (uint8_t i = 0; i < NUM_PINGS; i++) { // Loop through the sensors to see what's triggered
    if(pingDebug) {  //ping debug packet:  D:P:sensor index:distance cm
      Serial.print("D:P:"); Serial.print(i); Serial.print(":");Serial.println(sonar[i].ping_result / US_ROUNDTRIP_CM);
    }
    
    if (sonar[i].ping_result / US_ROUNDTRIP_CM > 0) {
      if(sonar[i].ping_result / US_ROUNDTRIP_CM < MODE0_LIMIT) {
        //gentle not working, evasive action!
        //spin until clear?
        switch(i) {
          case 0: //left side, turn right
          case 1: //left center, turn right
            Serial.println("I:Turn Right Hard");
            curDir = SPIN_RIGHT;
            bAllClear = false;
            break;
          case 2: //right center, turn left
          case 3: //right side, turn left
            Serial.println("I:Turn Left Hard");
            curDir = SPIN_LEFT;
            bAllClear = false;
            break;
        }
      } else if (bAllClear && sonar[i].ping_result / US_ROUNDTRIP_CM > 0 && sonar[i].ping_result / US_ROUNDTRIP_CM < MODE1_TURN) {
        //figure out what direction to gentle turn
        switch(i) {
          case 0: //left side, turn right
          case 1: //left center, turn right
            Serial.println("I:Turn Right");
            curDir = FWD_RIGHT;
            bAllClear = false;
            break;
          case 2: //right center, turn left
          case 3: //right side, turn left
            Serial.println("I:Turn Left");
            curDir = FWD_LEFT;
            bAllClear = false;
            break;
        }
        //map the turn ratio between the turning range to a 0 to 1 float, so it turns harder to closer to the spin limit
        turnRatio = map(sonar[i].ping_result / US_ROUNDTRIP_CM, MODE0_LIMIT, MODE1_TURN, 0, 100) / 100.0;
      } else if (bAllClear) {
       //clear, go forward
        Serial.println("I:Forward");
        curDir = FWD;
      }
    }
  }
  */
}

void checkMode2() {
/*  boolean bAllClear = true;
  uint8_t distance = MAX_DISTANCE;
  uint16_t blocks;
  int8_t panError;
  uint16_t i=0, j;
  
  lastTrigger = 255;
  for (uint8_t i = 0; i < NUM_PINGS; i++) { // Loop through the sensors to see what's triggered
    if(pingDebug) {  //ping debug packet:  D:P:sensor index:distance cm
      Serial.print("D:P:"); Serial.print(i); Serial.print(":");Serial.println(sonar[i].ping_result / US_ROUNDTRIP_CM);
    }
    
    if (sonar[i].ping_result / US_ROUNDTRIP_CM > 0) {
      if (bAllClear && sonar[i].ping_result / US_ROUNDTRIP_CM > 0 && sonar[i].ping_result / US_ROUNDTRIP_CM < MODE0_LIMIT) {
        bAllClear = false;
        lastTrigger = i;
        powerEnable = false;
        bot.estop();
        Serial.println("I:Too Close");
      } 
    }
    if (distance > sonar[i].ping_result / US_ROUNDTRIP_CM) {
      distance = sonar[i].ping_result / US_ROUNDTRIP_CM;
    }
  }

  blocks = pixy.getBlocks();
  
  if (blocks)
  {
    //find biggest
    for (j=1; j<blocks; j++)
    {
      if (pixy.blocks[j].width > pixy.blocks[i].width) {
        i = j;
      }
    }

    panError = X_CENTER - (pixy.blocks[i].x + (pixy.blocks[i].width/2));
    
    if(distance > MODE0_LIMIT) {
      if(panError > -10 && panError < 10) { //near center, go forward
        curDir = FWD;
        if(distance > MODE1_TURN) {
          curPwr = maxPwr;
        } else {
          curPwr = map(distance, MODE0_LIMIT, MODE1_TURN, 0, maxPwr);
        }
      } else if(panError < 0) { //veer right, 
        if(panError < -100 || distance < MODE1_TURN) { //spin it
          curDir = SPIN_RIGHT;
        } else {
          curDir = FWD_RIGHT;
          curPwr = map(distance, MODE0_LIMIT, MODE1_TURN, 0, maxPwr);
          turnRatio = map(panError, 0, 0-X_CENTER, 0, 100) / 100.0;
        }
      } else { //veer left, slow right motor down
        if(panError > 100 || distance < MODE1_TURN) { //spin it
          curDir = SPIN_LEFT;
        } else {
          curDir = FWD_LEFT;
          curPwr = map(distance, MODE0_LIMIT, MODE1_TURN, 0, maxPwr);
          turnRatio = map(panError, 0, X_CENTER, 0, 100) / 100.0;
        }
      }
      powerEnable = true;
    }
  } else { //no blocks, stop
    curDir = STOP;
    curPwr = 0;
    powerEnable = true;
  }
  */
}


