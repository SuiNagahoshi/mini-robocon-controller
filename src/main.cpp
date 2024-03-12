//10:b5:88:56:31:08

#include <Arduino.h>
#include <PS4Controller.h>

#include <vector>

// put function declarations here:

//GPIO 0,1,3,14,15,34,35,36,39 は使用しない
/*
motor: 5
move: 2(left, right)
high: 1
tools: 2(low, high)
*/

/*
pins[(15:A13),(2:A12),(0:A11),(4,A10)]
*/

#define RMOTOR A4
#define RDIR 35
#define LMOTOR A19
#define LDIR 33

#define LOWMOTOR A17
#define LOWMTDIR 25
#define LOWOPEN A15
#define LOWOPDIR 14

#define SHOOTMOTOR A13
#define SHOOTDIR 13

#define UPDWNMOTOR A12
#define UPDWNDIR 4

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  //PS4.attach(event);
  PS4.begin("10:b5:88:56:31:08");
  Serial.println("connected");

  pinMode(RMOTOR, OUTPUT);
  pinMode(RDIR, OUTPUT);
  pinMode(LMOTOR, OUTPUT);
  pinMode(LDIR, OUTPUT);

  pinMode(LOWMOTOR, OUTPUT);
  pinMode(LOWMTDIR, OUTPUT);
  pinMode(LOWOPEN, OUTPUT);
  pinMode(LOWOPDIR, OUTPUT);

  pinMode(SHOOTMOTOR, OUTPUT);
  pinMode(SHOOTDIR, OUTPUT);

  pinMode(UPDWNMOTOR, OUTPUT);
  pinMode(UPDWNDIR, OUTPUT);
  
  ledcSetup(0, 490, 8);
  ledcSetup(1, 490, 8);
  ledcSetup(2, 490, 8);
  ledcSetup(3, 490, 8);
  ledcSetup(4, 490, 8);
  ledcSetup(5, 490, 8);

  ledcAttachPin(RMOTOR,0);
  ledcAttachPin(LMOTOR,1);

  ledcAttachPin(LOWMOTOR,2);
  ledcAttachPin(LOWOPEN,3);

  ledcAttachPin(SHOOTMOTOR,4);
  ledcAttachPin(UPDWNMOTOR,5);
}

bool isDIR(int value) {
  if (value < 0) {
    return true;
  } else {
    return false;
  }
}

void stopMotor(std::vector<int> motor) {
  for (auto& m:motor) {
    ledcWrite(m, 0);
  }
  for (auto& m:motor) {
    motor.pop_back();
  }
}

void forward(int stick) {
  int speed = map(abs(stick), 0, 127, 0, 255);
  if (isDIR(stick)) {
    digitalWrite(LDIR, HIGH);
    digitalWrite(RDIR, HIGH);
    ledcWrite(0, speed);
    ledcWrite(1, speed);
  } else {
    digitalWrite(LDIR, LOW);
    digitalWrite(RDIR, LOW);
    ledcWrite(0, speed);
    ledcWrite(1, speed);
  }
}

void rotation(int stick) {
  int speed = map(abs(stick), 0, 127, 0, 255);
  if (isDIR(stick)) {
    digitalWrite(LDIR, HIGH);
    digitalWrite(RDIR, LOW);
    ledcWrite(0, speed);
    ledcWrite(1, speed);
  } else {
    digitalWrite(LDIR, LOW);
    digitalWrite(RDIR, HIGH);
    ledcWrite(0, speed);
    ledcWrite(1, speed);
  }
}

void upDown(bool dir) {
  if (dir) {
    digitalWrite(UPDWNDIR, HIGH);
    ledcWrite(5, 127);
  } else {
    digitalWrite(UPDWNDIR, LOW);
    ledcWrite(5, 127);
  }
}

void lowToolDeploy(bool dir) {
  if (dir) {
    digitalWrite(LOWMTDIR, HIGH);
    ledcWrite(3, 127);
  } else {
    digitalWrite(LOWMTDIR, LOW);
    ledcWrite(3, 127);
  }
}

void lowToolOpen(bool dir) {
  if (dir) {
    digitalWrite(LOWOPDIR, HIGH);
    ledcWrite(2, 127);
  } else {
    digitalWrite(LOWOPDIR, LOW);
    ledcWrite(2, 127);
  }
}

std::vector<int> motor;

void loop() {
  if (PS4.isConnected()) {
    if (PS4.RStickY() >= 10 || PS4.RStickY() <= -10) { 
      forward(PS4.RStickY());
      //Serial.printf("Left Stick y at  %d\n", PS4.LStickY());
    } else {
      motor = {RMOTOR, LMOTOR};
      stopMotor(motor);
    };
    if (PS4.Right()) { 
      forward(PS4.RStickY());
      //Serial.printf("Left Stick y at  %d\n", PS4.LStickY());
    } else {
      motor = {RMOTOR, LMOTOR};
      stopMotor(motor);
    };
    if (PS4.Up()) { 
      forward(PS4.RStickY());
      //Serial.printf("Left Stick y at  %d\n", PS4.LStickY());
    } else {
      motor = {RMOTOR, LMOTOR};
      stopMotor(motor);
    };
    if (PS4.Down()) { 
      forward(PS4.RStickY());
      //Serial.printf("Left Stick y at  %d\n", PS4.LStickY());
    } else {
      motor = {RMOTOR, LMOTOR};
      stopMotor(motor);
    };
    if (PS4.Left()) { 
      forward(PS4.RStickY());
      //Serial.printf("Left Stick y at  %d\n", PS4.LStickY());
    } else {
      motor = {RMOTOR, LMOTOR};
      stopMotor(motor);
    };
    
  }

  // put your main code here, to run repeatedly:
  if (PS4.isConnected()) {
    if (PS4.Right()) Serial.println("Right Button");
    if (PS4.Down()) Serial.println("Down Button");
    if (PS4.Left()) Serial.println("Left Button");
    if (PS4.Up()) Serial.println("Up Button");

    if (PS4.Circle()) Serial.println("Circle Button");
    if (PS4.Cross()) Serial.println("Cross Button");
    if (PS4.Square()) Serial.println("Square Button");
    if (PS4.Triangle()) Serial.println("Triangle Button");

    if (PS4.UpRight()) Serial.println("Up Right");
    if (PS4.DownRight()) Serial.println("Down Right");
    if (PS4.DownLeft()) Serial.println("Down Left");
    if (PS4.UpLeft()) Serial.println("Up Left");

    if (PS4.L1()) Serial.println("L1 Button");
    if (PS4.R1()) Serial.println("R1 Button");

    if (PS4.L2()) { 
      Serial.printf("L2 Button at %d\n", PS4.L2Value());
    };
    if (PS4.R2()) { 
      Serial.printf("R2 Button at %d\n", PS4.R2Value());
    };

    if (PS4.L3()) Serial.println("L3 Button");
    if (PS4.R3()) Serial.println("R3 Button");

    if (PS4.Share()) Serial.println("Share Button");
    if (PS4.Options()) Serial.println("Options Button");

    if (PS4.PSButton()) Serial.println("PS Button");
    if (PS4.Touchpad()) Serial.println("Touch Pad Button");

    if (PS4.LStickX() >= 10 || PS4.LStickX() <= -10) { 
      Serial.printf("Left Stick x at  %d\n", PS4.LStickX());
    };

    if (PS4.LStickY() >= 10 || PS4.LStickY() <= -10) { 
      //forward(PS4.LStickY());
      Serial.printf("Left Stick y at  %d\n", PS4.LStickY());
    };

    if (PS4.RStickX() >= 10 || PS4.RStickX() <= -10) { 
      Serial.printf("Right Stick x at  %d\n", PS4.RStickX());
    };

    if (PS4.RStickY() >= 10 || PS4.RStickY() <= -10) { 
      Serial.printf("Right Stick y at  %d\n", PS4.RStickY());
      forward(PS4.RStickY());
    };

    if (PS4.Charging()) Serial.println("The controller is charging.");
    if (PS4.Audio()) Serial.println("The controller has headphones attached.");
    if (PS4.Mic()) Serial.println("The controller has mic attached.");

    Serial.printf("Battery Level : %d\n", PS4.Battery());

    Serial.println();

    delay(1000);
  }
}