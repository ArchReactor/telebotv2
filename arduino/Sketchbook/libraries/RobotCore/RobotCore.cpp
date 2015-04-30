//	RobotCore.cpp - Library for robot essentials.
//	Created by Chris Weiss, May 2014.
//	Copyright (C) 2014  Chris Weiss	

#include "RobotCore.h"

void RobotCore::hbridge(uint8_t fPinR, uint8_t bPinR, uint8_t ePinR, uint8_t fPinL, uint8_t bPinL, uint8_t ePinL) {
	motorMode = 'H';
	
	fwdPinR = fPinR;
	bkdPinR = bPinR;
	enbPinR = ePinR;
	
	fwdPinL = fPinL;
	bkdPinL = bPinL;
	enbPinL = ePinL;
	
	pinMode (fwdPinR, OUTPUT); 
	pinMode (bkdPinR, OUTPUT); 
	pinMode (enbPinR, OUTPUT); 
	
	pinMode (fwdPinL, OUTPUT); 
	pinMode (bkdPinL, OUTPUT); 
	pinMode (enbPinL, OUTPUT); 
}

void RobotCore::servo(uint8_t Rpin, uint8_t Lpin) {
	motorMode = 'S';
	Rservo.attach(Rpin);
	Lservo.attach(Lpin);	
}
void RobotCore::servo(uint8_t Rpin, uint8_t Lpin, int min, int max) {
	motorMode = 'S';
	Rservo.attach(Rpin, min, max);
	Lservo.attach(Lpin, min, max);	
}

void RobotCore::drive(int speed, int steer) {
	spdRamp = 0;
	setMotorTarget(speed, steer);
	//no smoothing, apply in full
	spdCurR = spdTgtR;
	spdCurL = spdTgtL;
	setMotors();
}

void RobotCore::drive(int speed, int steer, uint8_t smoothing) {
	spdRamp = smoothing;
	setMotorTarget(speed, steer);
	setMotors();
}

void RobotCore::setMotorTarget(int speed, int steer) {
	if (speed == 0 && steer !=0) {
		spdTgtL = steer;
		spdTgtR = -steer;
	} else {
		spdTgtL = speed * ((-255 - steer) / -255.0);
		spdTgtR = speed * ((255 - steer) / 255.0);
		
		if (speed > 0 && spdTgtL > speed)
			spdTgtL = speed;
		if (speed > 0 && spdTgtR > speed)
			spdTgtR = speed;
		if (speed < 0 && spdTgtL < speed)
			spdTgtL = speed;
		if (speed < 0 && spdTgtR < speed)
			spdTgtR = speed;

	}
}

void RobotCore::speedCheck() {
	if (spdRamp && (millis() - spdRampLastMillis >= spdRamp || spdTgtR != spdCurR || spdTgtL != spdCurL)) {
		if(spdTgtR != spdCurR) {
			if(spdTgtR < spdCurR) {
//				if(spdCurR - spdTgtR <= spdRamp) { //final step
//					spdCurR = spdTgtR;
//				} else { //step down
					spdCurR--;
//				}
			} else if (spdTgtR > spdCurR) {
//				if(spdTgtR - spdCurR <= spdRamp) { //final step
//					spdCurR = spdTgtR;
//				} else { //step up
					spdCurR++;
//				}
			}
		}
		if(spdTgtL != spdCurL) {
			if(spdTgtL < spdCurL) {
//				if(spdCurL - spdTgtL <= spdRamp) { //final step
//					spdCurL = spdTgtL;
//				} else { //step down
					spdCurL--;
//				}
			} else if (spdTgtL > spdCurL) {
//				if(spdTgtL - spdCurL <= spdRamp) { //final step
//					spdCurL = spdTgtL;
//				} else { //step up
					spdCurL++;
//				}
			}
		}
		spdRampLastMillis = millis();
		setMotors();
	}
}

void RobotCore::setMotors() {
	if (motorMode == 'S') {
		Rservo.writeMicroseconds(map(spdCurR, -255, 255, 2000, 1000)); //invert since 0=full speed FWD and 90=stop and 180=full REV
		Lservo.writeMicroseconds(map(spdCurL, -255, 255, 2000, 1000)); //invert since 0=full speed FWD and 90=stop and 180=full REV
	}
	if (motorMode == 'H') {
		if (      spdCurR < 0) { //reverse
			analogWrite(enbPinR,  0 - spdCurR);
			digitalWrite (fwdPinR, LOW); 
			digitalWrite (bkdPinR, HIGH);
		} else if(spdCurR > 0) { //forward
			analogWrite(enbPinR,  spdCurR);
			digitalWrite (fwdPinR, HIGH); 
			digitalWrite (bkdPinR, LOW);
		} else if(spdCurR == 0) { //stop
			analogWrite(enbPinR,  spdCurR);
			digitalWrite (fwdPinR, LOW); 
			digitalWrite (bkdPinR, LOW);
		}
		if (      spdCurL < 0) { //reverse
			analogWrite(enbPinL,  0 - spdCurL);
			digitalWrite (fwdPinL, LOW); 
			digitalWrite (bkdPinL, HIGH);
		} else if(spdCurL > 0) { //forward
			analogWrite(enbPinL,  spdCurL);
			digitalWrite (fwdPinL, HIGH); 
			digitalWrite (bkdPinL, LOW);
		} else if(spdCurL == 0) { //stop
			analogWrite(enbPinL,  spdCurL);
			digitalWrite (fwdPinL, LOW); 
			digitalWrite (bkdPinL, LOW);
		}
	}
}

