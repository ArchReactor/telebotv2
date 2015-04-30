RobotCore will (eventually) provide all the core things you'll need to 
make an arduino based robot using various commonparts.

Features:
- Generic H-Bridge support:  works with any H-Bridge that uses 3 control
	wires per motor.
- RC-Servo emulating speed controll support
- abstracts the motor code from the movement and speed logic.  same 
	movement code will work with servo or h-bridge bots with only 1 line
	of code change
- Speed ramp support:  set target speed and step and the bot will 
	smoothly ramp and down to the target speed

Documentation:

void hbridge(uint8_t fPinR, uint8_t bPinR, uint8_t ePinR, uint8_t fPinL, 
				uint8_t bPinL, uint8_t ePinL)
	setup H-Bridge pins
	fPinR = "forward" right motor
	bPinR = "backward" right motor
	ePinR = "enable" right motor - requires PWM pin for speed control
	fPinL = "forward" left motor
	bPinL = "backward" left motor
	ePinL = "enable" left motor - requires PWM pin for speed control

void servo(uint8_t Rpin, uint8_t Lpin, int min, int max)
	setup servo pins
	Rpin = right servo
	Lpin = left servo
	min = override Servo Library min pulse, optional
	max = override Servo Library max pulse, optional

void drive(int speed, int steer, uint8_t smoothing);
	set direction to travel
	speed = -255 to 255.  -255 is reverse, 0 is stop, 255 is forward
	steer = -255 to 255.  -255 is left, 0 is strait, 255 is right
	smoothing = 1 to 255.  optional.  when not passed, speeds apply 
			immediatly.  larger number gets to target speed slower

void speedCheck()
	perform the next speed smoothing.  when not smoothing, no need to call this.

