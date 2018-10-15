#include <Servo.h>
//#define READ_SENSOR
int     SPEED_t=50;

/* REFERENCES */
int L_REF=425;
int C_REF=573;
int R_REF=454;
int L2_REF=557;
int R2_REF=536;

Servo udServo, gripperServo; 
int       ADIR = 7;
int       PWMA = 6;
int       BDIR = 3;
int       PWMB = 5;
const int trigPin = 11;
const int echoPin = 12;
int       buzzer = 8;
long      duration_t;
int       L, R, C, L2, R2;
int       error;
int       SW=4;

/* Motor Function */
void ao(){
  analogWrite(PWMA, 0);
  analogWrite(PWMB, 0);
}

void fd(int speed){
  //direction
  digitalWrite(ADIR, LOW);
  digitalWrite(BDIR, LOW);
  // speed
  analogWrite(PWMA, map(speed, 0, 100, 0, 255));
  analogWrite(PWMB, map(speed, 0, 100, 0, 255));
}

void sl(int speed){
  //direction
  digitalWrite(ADIR, HIGH);
  digitalWrite(BDIR, LOW);
  // speed
  analogWrite(PWMA, map(speed, 0, 100, 0, 255));
  analogWrite(PWMB, map(speed, 0, 100, 0, 255));
}

void sr(int speed){
  //direction
  digitalWrite(ADIR, LOW);
  digitalWrite(BDIR, HIGH);
  // speed
  analogWrite(PWMA, map(speed, 0, 100, 0, 255));
  analogWrite(PWMB, map(speed, 0, 100, 0, 255));
}

void bk(int speed){
  //direction
  digitalWrite(ADIR, HIGH);
  digitalWrite(BDIR, HIGH);
  // speed
  analogWrite(PWMA, map(speed, 0, 100, 0, 255));
  analogWrite(PWMB, map(speed, 0, 100, 0, 255));
}

void show_sensor_values(){
  Serial.print("L=\t");Serial.println(analogRead(0));
  Serial.print("C=\t");Serial.println(analogRead(1));
  Serial.print("R=\t");Serial.println(analogRead(2));
  Serial.print("L2=\t");Serial.println(analogRead(4));
  Serial.print("R2=\t");Serial.println(analogRead(3));
//  Serial.print("AN5=");Serial.println(analogRead(5));
//  Serial.print("AN6=");Serial.println(analogRead(6));
//  Serial.print("AN7=");Serial.println(analogRead(7));
  Serial.println("===================================\r\n");
}

int read_sensor(){
  L   = analogRead(0);
  C   = analogRead(1);
  R   = analogRead(2);
  L2  = analogRead(4);
  R2  = analogRead(3);

  if(L > L_REF && C > C_REF && R < R_REF ){
    error = -2;
  }  
  else if(L > L_REF && C < C_REF && R < R_REF ){
    error = -1;
  }
  else if(L < L_REF && C < C_REF && R > R_REF ){
    error = 1;
  }
  else if(L < L_REF && C > C_REF && R > R_REF ){
    error = 2;
  }
  else{
    error = 0;
  }
  Serial.print("Error="); Serial.println(error);
  #ifdef READ_SENSOR
  show_sensor_values();
  #endif
}

void follow_line(){
  read_sensor();
  if(error > 0){
    sl(SPEED_t-20);
  }
  else if(error < 0){
    sr(SPEED_t-20);
  }
  else{
    fd(SPEED_t);
  }  
}

void FF(){
  read_sensor();
  while(L2 > L2_REF && R2 > R2_REF){
    read_sensor();
    follow_line();
  }
  //delay(50);
  ao();
}

void FR(){
  ao(); delay(50);
  sr(SPEED_t); delay(300);
  read_sensor();
  while(C > C_REF){
    read_sensor();
    sr(SPEED_t);
  }
  ao();
}

void FL(){
  ao(); delay(50);
  sl(SPEED_t); delay(300);
  read_sensor();
  while(C > C_REF){
    read_sensor();
    sl(SPEED_t);
  }
  ao();
}

int read_distance(){
  int distance;
  
  // Clears the trigPin
  digitalWrite (trigPin, LOW);
  delayMicroseconds (2);
  
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite (trigPin, HIGH);
  delayMicroseconds (10);
  digitalWrite (trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration_t = pulseIn (echoPin, HIGH);
  
  // Calculating the distance
  distance = duration_t * 0.034 / 2;

  return distance;
}

void beep(){
  tone(buzzer, 1500);
  delay(150);
  noTone(buzzer);
}

void sw_ok_press(){
  while(digitalRead(SW)){
    delay(100);
  }
}
void setup() {
  /* drive motor */
  pinMode(ADIR, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(BDIR, OUTPUT);
  pinMode(PWMB, OUTPUT);
  analogWrite(PWMA, 128);
  digitalWrite(ADIR, LOW);
  analogWrite(PWMB, 128);
  digitalWrite(BDIR, LOW);

  /* servo */
  udServo.attach(9);
  gripperServo.attach(10);

  /* ultrasonic */
  pinMode (trigPin, OUTPUT);  // Sets the trigPin as an Output
  pinMode (echoPin, INPUT); // Sets the echoPin as an Input

  /* SW */
  pinMode(SW, INPUT);
  
  /* Communication */
  Serial.begin(9600);

  /* set to initial val */
  ao(); // all motors off
  sw_ok_press(); 
}

void loop() {
  FF();
  while(1);


  
//  analogWrite(PWMA, 0);
//  analogWrite(PWMB, 0);
//  udServo.write(0); 
//  gripperServo.write(0); 
//  printAnalogRead();
//  Serial.print("Distance=");Serial.println(read_distance());
//  beep();
//  delay(1000);
//
//  analogWrite(PWMA, 128);
//  analogWrite(PWMB, 128);
//  udServo.write(90); 
//  gripperServo.write(90); 
//  printAnalogRead();
//  Serial.print("Distance=");Serial.println(read_distance());
//  beep();
//  delay(1000);
//
//  analogWrite(PWMA, 255);
//  analogWrite(PWMB, 255);
//  udServo.write(180); 
//  gripperServo.write(180); 
//  printAnalogRead();
//  Serial.print("Distance=");Serial.println(read_distance());
//  beep();
//  delay(1000);
//  
}
