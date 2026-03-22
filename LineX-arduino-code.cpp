#include <SoftwareSerial.h>

SoftwareSerial BT(2, 4); // RX, TX

// -------- SENSOR PINS --------
#define leftSensorPin A0
#define centerSensorPin A2
#define rightSensorPin A1

// -------- MOTOR DRIVER PINS --------
#define ENA 11
#define IN1 10
#define IN2 9

#define ENB 3
#define IN3 6
#define IN4 5

// -------- REFINED TUNING PARAMETERS --------
int manualSpeed = 180;
int autoBaseSpeed = 115; // Slow enough to react
int autoTurnSpeed = 100; // Controlled pivot speed
int Kp = 60;             // Fast snap-back

int lastError = 0;
int lostCounter = 0;
bool autoMode = true;
bool hasStartedNavigating = false; // Prevents "phantom" starts

void setup() {
  Serial.begin(9600);
  BT.begin(9600);

  pinMode(ENA, OUTPUT); pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT); pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);

  pinMode(leftSensorPin, INPUT);
  pinMode(centerSensorPin, INPUT);
  pinMode(rightSensorPin, INPUT);
}

void loop() {
  if (BT.available()) {
    char cmd = BT.read();
    if (cmd == 'A') { autoMode = true; hasStartedNavigating = false; }
    if (cmd == 'M') { autoMode = false; hasStartedNavigating = false; stopRobot(); }

    if (!autoMode) {
      if (cmd == '1') manualSpeed = 120;
      if (cmd == '2') manualSpeed = 180;
      if (cmd == '3') manualSpeed = 230;
      if (cmd == 'F') moveForward();
      if (cmd == 'B') moveBackward();
      if (cmd == 'L') turnLeft();
      if (cmd == 'R') turnRight();
      if (cmd == 'S') stopRobot();
      if (cmd == 'G') spin360();
    }
  }

  if (autoMode) {
    lineFollower();
  }
}

void lineFollower() {
  int L = digitalRead(leftSensorPin);
  int C = digitalRead(centerSensorPin);
  int R = digitalRead(rightSensorPin);

  // --- LOGIC GATE 1: CHECK FOR ANY LINE SIGHTING ---
  if (L == 1 || C == 1 || R == 1) {
    hasStartedNavigating = true; 
    lostCounter = 0; // Reset timer whenever line is seen
  }

  // --- LOGIC GATE 2: IF WE ARE ON PURE WHITE ---
  if (L == 0 && C == 0 && R == 0) {
    if (!hasStartedNavigating) {
      stopRobot();
      return;
    }
    
    lostCounter++;
    if (lostCounter < 1500) { 
      recoverLine();
    } else {
      stopRobot(); 
      hasStartedNavigating = false; 
    }
    return;
  }

  // --- LOGIC GATE 3: NORMAL NAVIGATION ---
  int error = 0;
  bool hardTurn = false;

  if (L == 1 && C == 1 && R == 0) error = -1; 
  else if (R == 1 && C == 1 && L == 0) error = 1;  
  else if (L == 1 && C == 0 && R == 0) { error = -2; hardTurn = true; } 
  else if (R == 1 && C == 0 && L == 0) { error = 2; hardTurn = true; }  
  else if (C == 1) error = 0; 

  lastError = error; 

  int correction = Kp * error;
  int leftMotorSpeed = autoBaseSpeed + correction;
  int rightMotorSpeed = autoBaseSpeed - correction;

  if (hardTurn) {
    // Aggressive Pivot: One motor reversed, one forward
    if (error < 0) executePivot(-autoTurnSpeed, autoTurnSpeed); 
    else executePivot(autoTurnSpeed, -autoTurnSpeed); 
  } else {
    // Smooth Proportional Drive
    leftMotorSpeed = constrain(leftMotorSpeed, 0, 255);
    rightMotorSpeed = constrain(rightMotorSpeed, 0, 255);
    applyAutoDrive(leftMotorSpeed, rightMotorSpeed);
  }
}

// -------- HARDWARE PIVOT ENGINE --------

void applyAutoDrive(int lVal, int rVal) {
  analogWrite(ENA, lVal);
  analogWrite(ENB, rVal);
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}

void executePivot(int lVal, int rVal) {
  analogWrite(ENA, abs(lVal));
  analogWrite(ENB, abs(rVal));

  if (lVal >= 0) { digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH); }
  else { digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW); }

  if (rVal >= 0) { digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW); }
  else { digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH); }
}

void recoverLine() {
  // Pivot based on last seen error to find line again
  if (lastError <= 0) executePivot(-autoTurnSpeed, autoTurnSpeed);
  else executePivot(autoTurnSpeed, -autoTurnSpeed);
}

void stopRobot() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
}

// --- MANUAL MODE REUSE ---
void moveForward() {
  analogWrite(ENA, manualSpeed); analogWrite(ENB, manualSpeed);
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}
void moveBackward() {
  analogWrite(ENA, manualSpeed); analogWrite(ENB, manualSpeed);
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
}
void turnLeft() {
  analogWrite(ENA, manualSpeed); analogWrite(ENB, manualSpeed);
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}
void turnRight() {
  analogWrite(ENA, manualSpeed); analogWrite(ENB, manualSpeed);
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
}
void spin360() { turnRight(); delay(650); stopRobot(); }