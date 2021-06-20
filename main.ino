//Declare pins
//Left side motor variabe declaration
int LSpeed = 5;
int LeftFront = 6;
int LeftBack = 7;
//Right side motor variable declaration
int RSpeed = 3;
int RightForward = 2;
int RightBack = 4;
//Sensor declarations
//Ultrasonic
int echoPin = 12;
int trigPin = 11;
//IR sensors
int BR = A3;
int FR = A2;
int FL = A1;
int BL = A0;
//IR VLAUES
int BackRightIR = analogRead(BR);
int FrontRightIR = analogRead(FR);
int FrontLeftIR = analogRead(FL);
int BackLefttIR = analogRead(BL);

const int offset = -2;

void StopMotors() { //Turn all of the motors off (uses H-bridge)
    Serial.println("Stopping...");
    digitalWrite(LeftFront, LOW);
    digitalWrite(LeftBack, LOW);
    digitalWrite(RightBack, LOW);
    digitalWrite(RightForward, LOW);
    delay(200);
}

void ChangeSpeed(int speed) { //Change the speed of the motors
    Serial.println("Changing speed...");
    //int OffSett = (int(round(speed / 100) * offset));
    analogWrite(LSpeed, (speed - offset));
    analogWrite(RSpeed, speed);
}

void Forward() {
    Serial.println("Forward...");
    StopMotors();
    digitalWrite(RightForward, HIGH);
    digitalWrite(LeftFront, HIGH);
}

void Backward() {
    Serial.println("Backward");
    StopMotors();
    digitalWrite(RightBack, HIGH);
    digitalWrite(LeftBack, HIGH);
}

void Left() {
    Serial.println("Left...");
    StopMotors();
    digitalWrite(RightForward, HIGH);
    digitalWrite(LeftBack, HIGH);
}

void Right() {
    Serial.println("Right...");
    StopMotors();
    digitalWrite(RightBack, HIGH);
    digitalWrite(LeftFront, HIGH);
}

float SonicSense() { //Get a ultrasonic reading and return it in mm
    Serial.println("Ultrasonicing...");
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    int mm = pulseIn(echoPin, HIGH, 1000000) * 0.01715; //I calculated this by dividing the speed of sound (343M/s) by 10,000, then by 2
    return mm;
}

boolean LineSense(boolean checkRes) { //Check all sensors
    Serial.println("Linesensors...");
    BackRightIR = analogRead(BR);
    FrontRightIR = analogRead(FR);
    FrontLeftIR = analogRead(FL);
    BackLefttIR = analogRead(BL);
    Serial.println(BackRightIR);
    Serial.println(BackLefttIR);
    Serial.println(FrontLeftIR);
    Serial.println(FrontRightIR);
    if (checkRes) {
        if (BackLefttIR < 700 || FrontLeftIR < 200 || FrontLeftIR < 200) {
            return true;
        }
        else {
            return false;
        }
    }
}

void setup() {
    pinMode(LSpeed, OUTPUT);
    pinMode(RSpeed, OUTPUT);
    pinMode(LeftFront, OUTPUT);
    pinMode(LeftBack, OUTPUT);
    pinMode(RightForward, OUTPUT);
    pinMode(RightBack, OUTPUT);
    pinMode(trigPin, OUTPUT);
    Serial.begin(9600);
    ChangeSpeed(80);
    Forward();
}

void loop() {
    LineSense(false);
    delay(3000);
    
   if (SonicSense() < 100 && !LineSense(true)) {
       ChangeSpeed(100);
       Forward();
       if(SonicSense() < 50) {
           ChangeSpeed(120);
       }
   }
   else if (LineSense(true)) {
       if (analogRead(FL) < 200) {
           //On line, move off
           Backward();
           analogWrite(RSpeed, 50);
           analogWrite(RSpeed, 100); //Veer to the right
           delay(200);
           ChangeSpeed(100); //Reset speed to normal
       }
       else if (analogRead(FR) < 200) { //My tests showed values less than 800 were typically on the line
           //On line, move off
           Backward();
           analogWrite(RSpeed, 50);
           analogWrite(LSpeed, 100); //Veer to the right
           delay(200);
           ChangeSpeed(100); //Reset speed to normal
       }
   }
   else { //Start spinning to find people
       ChangeSpeed(10);
       Left();
       delay(100);
   }
}