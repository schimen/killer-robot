#ifndef Motor_h
#define Motor_h

#include <Arduino.h>
#include "SimpleTimer.h"

class Motor {
    public:
        Motor(uint8_t  motorID, // <- i am very angry at this `motorID`
        uint8_t      pin1, 
        uint8_t      pin2, 
        uint8_t      enablePin, 
        uint16_t     stepDelay, 
        uint8_t      stepSize, 
        SimpleTimer* timer);

        void set(int8_t speed);
        void changeSpeed();

        // i have not found a good for using class members in the callback function, 
        // which is why i need this abomination
        static void   updateMotor1();
        static Motor* ptrMotor1;
        static void   updateMotor2();
        static Motor* ptrMotor2;

    private:
        void          writeSpeed(int8_t speed);
        void          writeDirection(int8_t speed);

        uint8_t       _pin1;
        uint8_t       _pin2;
        uint8_t       _enablePin;
        uint16_t      _stepDelay;
        uint8_t       _stepSize;
        int8_t        _stepDirection;
        int8_t        _currentSpeed;
        SimpleTimer*  _timer;
        int           _timerID;
        uint8_t       _motorID; // smh :(
};

Motor* Motor::ptrMotor1;
Motor* Motor::ptrMotor2;

Motor::Motor(uint8_t      motorID,
    uint8_t      pin1, 
    uint8_t      pin2, 
    uint8_t      enablePin, 
    uint16_t     stepDelay, 
    uint8_t      stepSize,
    SimpleTimer* timer) {

    // pin setup
    pinMode(pin1,      OUTPUT);
    pinMode(pin2,      OUTPUT);
    pinMode(enablePin, OUTPUT);

    // private variables
    _pin1          = pin1;
    _pin2          = pin2;
    _enablePin     = enablePin;
    _stepDelay     = stepDelay;
    _stepSize      = stepSize;
    _stepDirection = 1;
    _currentSpeed  = 0;
    _timer         = timer;
    _timerID       = 0;

    // set forward direction and 0 speed at the start
    writeDirection(1);
    writeSpeed(0);

    // i hate this, but i still haven't found a better solution
    if (motorID == 1) {
        ptrMotor1 = this;
        _motorID = motorID;
    }
    else if (motorID == 2) {
        ptrMotor2 = this;
        _motorID = motorID;
    }
}

void Motor::set(int8_t speed) {
    // calculate direction of acceleration
    if (speed >= _currentSpeed) {
        _stepDirection = 1;
    }
    else {
        _stepDirection = -1;
    }
    // number of steps equals: difference in speed divided by step size
    uint8_t nSteps = abs(speed - _currentSpeed) / _stepSize;
    // stop already running timers
    if (_timer -> isEnabled(_timerID)) {
        _timer -> disable(_timerID);
        _timer -> deleteTimer(_timerID);
    }
    // function to be called `n` times with a set interval

    // start a timer to call `changeSpeed`, `nSteps` times with `_stepDelay` interval
    // (the motorID thing is a bad solution, but it works)
    if (_motorID == 1) {
        _timer -> setTimer(_stepDelay, Motor::updateMotor1, nSteps);
    }
    else if (_motorID == 2) {
        _timer -> setTimer(_stepDelay, Motor::updateMotor2, nSteps);
    }
}

void Motor::changeSpeed() {
    // function to be called `n` times with a set interval
    writeSpeed(_currentSpeed + (_stepSize*_stepDirection));    
}

void Motor::writeSpeed(int8_t speed) {
    // write direction of motors, if direction has changed from last time
    if (((_currentSpeed >= 0) and (speed < 0)) or ((_currentSpeed < 0) and speed >= 0)) {
        writeDirection(speed);
    }
    // set speed
    analogWrite(_enablePin, abs(speed)*2);
    _currentSpeed = speed;
}

void Motor::writeDirection(int8_t speed) {
    // write direction of motor
    if (speed >= 0) {		// forward direction
        digitalWrite(_pin1, HIGH);
        digitalWrite(_pin2, LOW);
    } 
    else {                  // reverse direction
        digitalWrite(_pin1, LOW);
        digitalWrite(_pin2, HIGH);
    }
}    

// big oof
void Motor::updateMotor1() {
    Motor::ptrMotor1 -> changeSpeed();
}
// i need to actually learn c++ at some point
void Motor::updateMotor2() {
    Motor::ptrMotor2 -> changeSpeed();
}

#endif
