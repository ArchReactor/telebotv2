//	RobotCore.h - Library for robot essentials.
//	Created by Chris Weiss, May 2014.
//	Copyright (C) 2014  Chris Weiss	

#ifndef RobotCore_h
#define RobotCore_h

#include "Arduino.h"
#include "Servo.h"

class RobotCore {

public:
	void hbridge(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
	void servo(uint8_t, uint8_t);
	void servo(uint8_t, uint8_t, int, int);

	void drive(int speed, int steer);
	void drive(int speed, int steer, uint8_t smoothing);
	void speedCheck();

	int spdTgtR;  //target speed
	int spdTgtL;  //target speed
	int spdCurR;  //current speed
	int spdCurL;  //current speed
	
	char motorMode; //'H' for hbridge, 'S' for servo

	uint8_t spdRamp;
	unsigned long spdRampLastMillis;
	
	void setMotorTarget(int speed, int steer);
	void setMotors();
	
	//hbridge pins
	uint8_t fwdPinR;
	uint8_t bkdPinR; 
	uint8_t enbPinR;
	uint8_t fwdPinL;
	uint8_t bkdPinL; 
	uint8_t enbPinL;
	
	//servo objects
	Servo Rservo;
	Servo Lservo;

};

#endif
