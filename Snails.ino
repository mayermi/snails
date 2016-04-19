//Servo.h needs to be included in order to use servos
#include <Servo.h>

//This servo adjusts the angle of the turing wheel to either have contact to the snails' tracks or not
Servo angleServo;
//This servo moves the snails' tracks
Servo snailServo;

//The position motor moves the turning wheel to the right track. To move forward and backward it needs a PWM pin
int positionMotorPin1 = 10;
int positionMotorPin2 = 11;

//With these pins the current roll of the color wheel can be checked. As pin 3 and pin 5 are PWM pins any change of their state can be tracked.
int wheelPin0 = 3;
int wheelPin1 = 5;
int wheelPin2 = 12;

//This pin is for the position motor. Depending on its state the motor can check how many 'steps' it has taken.
int positionPin = 7;

//This defines the position motor's speed. Currently the maximum.
int positionSpeed = 255;

//Those numbers are for checking whether the color wheel has been turned by comparing the old states of the pins with the current ones.
int oldStateWheel0 = 0;
int oldStateWheel1 = 0;
int oldStateWheel2 = 0;

//This array holds the current positions of all snails.
int snails[] = {0,0,0,0,0,0};

void setup() {
//  Here the servos are attached to pins -again with PWM
  angleServo.attach(9);
  snailServo.attach(6);

//The position motor's pins need to be defined as output
  pinMode(positionMotorPin1, OUTPUT);
  pinMode(positionMotorPin2, OUTPUT);

//To check whether the contacts are closed the pins of the color wheel and the pin that shows the motor's position are defined as input and set high
  pinMode(wheelPin0, INPUT);
  digitalWrite(wheelPin0, HIGH);
  pinMode(wheelPin1, INPUT);
  digitalWrite(wheelPin1, HIGH);
  pinMode(wheelPin2, INPUT);
  digitalWrite(wheelPin2, HIGH);

  pinMode(positionPin, INPUT);
  digitalWrite(positionPin, HIGH);

//Both servos are setup to a basic angle in case their angle has been moved
  angleServo.write(155);
  snailServo.write(155);
}

void loop() {
//The current state of the wheel pins is saved in variables
  int currentStateWheel0 = digitalRead(wheelPin0);
  int currentStateWheel1 = digitalRead(wheelPin1);
  int currentStateWheel2 = digitalRead(wheelPin2);

//In case this current stat is different to the old state (this means the wheel must have been moved) ..
  if(currentStateWheel0 != oldStateWheel0) {
//  .. the system waits for 1500ms to make sure the wheel has stopped turning
    delay(1500);
//  The wheel stopped turning if the current state is equal to the state written in the variable earlier. This has to be checked for every wheel pin, so there are three if clauses.
    if(digitalRead(wheelPin0) == currentStateWheel0) {
//    If the wheel stopped, the snail can move
      moveSnail(getWheelState());
    } 
  } else if(currentStateWheel1 != oldStateWheel1) {
    delay(1500);
    if(digitalRead(wheelPin1) == currentStateWheel1) {
      moveSnail(getWheelState());
    } 
  } else if(currentStateWheel2 != oldStateWheel2) {
    delay(1500);
    if(digitalRead(wheelPin2) == currentStateWheel2) {
      moveSnail(getWheelState());
    } 
  }

//The current state is now the new old state
  oldStateWheel0 = digitalRead(wheelPin0);
  oldStateWheel1 = digitalRead(wheelPin1);
  oldStateWheel2 = digitalRead(wheelPin2);

//It is checked if one of the snails has reached the goal -position 8. If so, the game is resetted.
  for (int i = 0; i < 6; i++) {
    if (snails[i] == 8) {
          reset();
    }
  }
}

//This method returns the state of the color wheel. Depending on the state of the wheel's pins the state of the wheel (a color or return) is generated.
int getWheelState() {
  if(digitalRead(wheelPin0) == LOW && digitalRead(wheelPin1) == LOW && digitalRead(wheelPin2) == LOW) {
    return 2;
  } else if (digitalRead(wheelPin0) == LOW && digitalRead(wheelPin1) == LOW && digitalRead(wheelPin2) == HIGH) {
    return 1;
  } else if (digitalRead(wheelPin0) == HIGH && digitalRead(wheelPin1) == LOW && digitalRead(wheelPin2) == HIGH) {
    return 7;
  } else if (digitalRead(wheelPin0) == HIGH && digitalRead(wheelPin1) == LOW && digitalRead(wheelPin2) == LOW) {
    return 4;
  } else if (digitalRead(wheelPin0) == LOW && digitalRead(wheelPin1) == HIGH && digitalRead(wheelPin2) == HIGH) {
    return 0;
  } else if (digitalRead(wheelPin0) == HIGH && digitalRead(wheelPin1) == HIGH && digitalRead(wheelPin2) == HIGH) {
    return 3;
  } else if (digitalRead(wheelPin0) == HIGH && digitalRead(wheelPin1) == HIGH && digitalRead(wheelPin2) == LOW) {
    return 6;
  } else if (digitalRead(wheelPin0) == LOW && digitalRead(wheelPin1) == HIGH && digitalRead(wheelPin2) == LOW) {
    return 5;
  }
}

//This methods makes the motor go in a certain direction (forward, backward or stop) with a certain speed
void motorGo(int dir, int pin1, int pin2) {
  switch(dir) {
    case 1:
      digitalWrite(pin1, LOW);
      analogWrite(pin2, positionSpeed);
      break;
    case 2:
      digitalWrite(pin2, LOW);
      analogWrite(pin1, positionSpeed);
      break;
    default:
      digitalWrite(pin1, LOW);
      digitalWrite(pin2, LOW);
  }
}

//This method makes the motor drive to its basic position by leting it drive backward for a second (it usually takes much less so the motor will be at its basic position when it stopps) with maximum speed
void motorGoHome() {
  positionSpeed = 255;
  motorGo(2, positionMotorPin1, positionMotorPin2);
  delay(1000);
  motorGo(0, positionMotorPin1, positionMotorPin2);
}

void moveSnail(int dieNumber) {
  positionSpeed = 255;

//In case neither 6 nor 7 were rolled (those are the return fields)..
  if(dieNumber != 6 && dieNumber != 7) {
//  .. the motor goes forward as many steps as it hase been rolled
    for(int i = 0; i < dieNumber; i++) {
      oneStep(1);
    }

//  Arrived at position, the angle of the angle servo is changed, so that the turning wheel is in contact with the snail's track.
    angleServo.write(70);
//  The next three delays are in case one servo is held up or just takes some time so that all servos have enough time to finish their turns before the next one starts
    delay(1000);
//  The servo that moves the track moves to a specific angle (this movement is exactly one step wide)
    snailServo.write(55);
    delay(1000);
//  The angle servo moves back to its origin position
    angleServo.write(155);
    delay(1000);

//  The moved snail's position is updated in the array.
    snails[dieNumber]++;
    
//  The position motor goes to its basic position for the next round.
    motorGoHome();

//  The servo that moves the snails moves back to its original position. This is the last move as it is save at the basic position of the motor not to accidentily move any snail's track as it is far away from any
    snailServo.write(155);
    delay(1000);

//If a return has been rolled the function that lets snails move backward is called
  } else {
    moveSnailBackward();
  }
}

//This function moves a random snail backward
void moveSnailBackward() {
//  A rando number between 0 and 5 (for 6 snails) is picked
  long goesBack = random(0,5);

//The snail is only moved back, if it is not at its start position
  if(snails[goesBack] > 0) {
//  The motor goes forward as many steps as the random number
    for(int i = 0; i <= goesBack; i++) {
      oneStep(1);
    }

//  Arrived at position, the servo that moves the snail tracks is turned so it can go the other way around.
    snailServo.write(55);
//  The next three delays are in case one servo is held up or just takes some time so that all servos have enough time to finish their turns before the next one starts
    delay(1000);
//  The angle of the angle servo is changed, so that the turning wheel is in contact with the snail's track.
    angleServo.write(150);
    delay(1000);
//  The servo that moves the track moves to a specific angle (this movement is exactly one step wide)
    snailServo.write(155);
    delay(1000);
//  The angle servo moves back to its origin position
    angleServo.write(155);
    delay(1000);

//  The moved snail's position is updated in the array.
    snails[goesBack]--;
    
//  The position motor goes to its basic position for the next round.
    motorGoHome();

//  Since the snail servo has already its basic angle from the move backward it does not need to be moved here again
  }
}

//This method makes the motor move one step at a time
void oneStep(int motorDirection) {
//At first the position pin has contact with the ground so it first needs to move forward as long as its status is low
  while (digitalRead(positionPin) == LOW) {
    motorGo(1, positionMotorPin1, positionMotorPin2);
  }
//When its status is high it still needs to move forward until it finds the nect contact and its status becomes low again
  while (digitalRead(positionPin) == HIGH) {
    motorGo(1, positionMotorPin1, positionMotorPin2);
  }
//As soon as the next contact is found the motor is stopped and has to wait in case any hardware caused delays have occured
  motorGo(0, positionMotorPin1, positionMotorPin2);
  delay(1000);
}

//This method resets the whole game at the end
void reset() {
//The following steps are done with each snail
  for(int i = 0; i < 6; i++) { 
//  Move snail as many steps back at is has moved forward
    for(int j = -1; j < snails[i]; j++) {
      snailServo.write(55);
//    The next three delays are in case one servo is held up or just takes some time so that all servos have enough time to finish their turns before the next one starts
      delay(1000);
//    The angle of the angle servo is changed, so that the turning wheel is in contact with the snail's track.
      angleServo.write(150);
      delay(1000);
//    The servo that moves the track moves to a specific angle (this movement is exactly one step wide)
      snailServo.write(155);
      delay(1000);
//    The angle servo moves back to its origin position
      angleServo.write(155);
      delay(1000);
//    Since the snail servo has already its basic angle from the move backward it does not need to be moved here again
    }

//  The motor only takes a step forward, if it is not already at the last position
    if(i != 5) {
      oneStep(1); 
    }
  }

//The position of all snails is set back to 0
  for (int i = 0; i < 6; i++) {
    snails[i] = 0;
  }
  
//The position motor goes to its basic position for the next round.
  motorGoHome();
}

