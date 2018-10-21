#include <Servo.h>
#include <ArduinoJson.h>
#define READ_SENSOR
int     SPEED_t=60;

/* REFERENCES */
int     L_REF=597;
int     R_REF=547;
int     L2_REF= 546;
int     R2_REF=640;

Servo             udServo, gripperServo; 
int       ADIR = 7;
int       PWMA = 6;
int       BDIR = 3;
int       PWMB = 5;
const int trigPin = 11;
const int echoPin = 12;
int       buzzer = 8;
long      duration_t;
int       L, R, C, L2, R2;
int       L_min, R_min, C_min, L2_min, R2_min;
int       L_max, R_max, C_max, L2_max, R2_max;
int       L_avg, R_avg, C_avg, L2_avg, R2_avg;
int       error;
int       SW=4;

void clearAndHome()
{
  Serial.write(27);
  Serial.print("[2J"); // clear screen
  Serial.write(27);
  Serial.print("[H"); // cursor to home
}

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

double read_sensor(){
  //clearAndHome();
  
  L   = analogRead(0);
  R   = analogRead(2);
  L2  = analogRead(4);
  R2  = analogRead(3);
  
  if(L > L_REF && R < R_REF ){
    error = -(R_REF - R);
  }  
  else if(L < L_REF && R > R_REF ){
    error = (L_REF - L);
  }
  else{
    error = 0;
  }
  /*
  Serial.print("\tL2\tL\tR\tR2\r\n");
  Serial.print("=====================================\r\n");
  Serial.print("Actual:\t");
  Serial.print(L2); Serial.print("\t");
  Serial.print(L); Serial.print("\t");
  Serial.print(R); Serial.print("\t");
  Serial.print(R2); Serial.print("\r\n");


  if(L2 < L2_min)   L2_min = L2;
  if(L < L_min)     L_min = L;
  if(R < R_min)     R_min = R;
  if(R2 < R2_min)   R2_min = R2;
  Serial.print("MIN:\t");
  Serial.print(L2_min); Serial.print("\t");
  Serial.print(L_min); Serial.print("\t");
  Serial.print(R_min); Serial.print("\t");
  Serial.print(R2_min); Serial.print("\r\n");

  if(L2 > L2_max)   L2_max = L2;
  if(L > L_max)     L_max = L;
  if(R > R_max)     R_max = R;
  if(R2 > R2_max)   R2_max = R2;
  Serial.print("MAX:\t");
  Serial.print(L2_max); Serial.print("\t");
  Serial.print(L_max); Serial.print("\t");
  Serial.print(R_max); Serial.print("\t");
  Serial.print(R2_max); Serial.print("\r\n");

  L2_avg  = (L2_min + L2_max) / 2;
  L_avg   = (L_min + L_max) / 2;
  R_avg   = (R_min + R_max) / 2;
  R2_avg  = (R2_min + R2_max) / 2;
  Serial.print("AVG:\t");
  Serial.print(L2_avg); Serial.print("\t");
  Serial.print(L_avg); Serial.print("\t");
  Serial.print(R_avg); Serial.print("\t");
  Serial.print(R2_avg); Serial.print("\r\n");
  */
  return error;
}

void follow_line(){
  int error_t = read_sensor();
  if(error_t > 0){
    sl(SPEED_t-30);
  }
  else if(error_t < 0){
    sr(SPEED_t-30);
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
  fd(SPEED_t);
  delay(100);
  ao();
  beep();
}

void FR(){
  read_sensor();
  while(L2 > L2_REF && R2 > R2_REF){
    read_sensor();
    follow_line();
  }
  fd(SPEED_t);  delay(120);
  ao(); beep(); delay(1000); 
  sr(SPEED_t);  delay(300);
  ao(); beep();
}

void FL(){
  read_sensor();
  while(L2 > L2_REF && R2 > R2_REF){
    read_sensor();
    follow_line();
  }
  fd(SPEED_t);  delay(120);
  ao(); beep(); delay(1000); 
  sl(SPEED_t);  delay(300);
  ao(); beep();
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
  udServo.write(0);
  gripperServo.write(0);

  /* Initialize ref */
  L2_min  = analogRead(4);
  L2_max  = L2_min;
  L_min   = analogRead(0);
  L_max   = L_min;
  R_min   = analogRead(2);
  R_max   = R_min;
  R2_min  = analogRead(3);
  R2_max  = R_min;

  Serial.println("Press key to start...");
  sw_ok_press();
  
  Serial.println("Started");
  udServo.write(80);
  gripperServo.write(30); 
}

void loop() {
  //read_sensor(); delay(500);
  FR(); FF(); FL();
  while(1);
}
