/*  LP Robot
 *  Lasted update 2018-10-10
 */

#define XT
//#define READ_SENSOR

#ifdef XT
#include <popxt.h> 
#include <SoftwareServo.h> 
SoftwareServo udServo, gripperServo;
int L_REF=285;
int C_REF=398;
int R_REF=426;
int L2_REF=297;
int R2_REF=375;
const int trigPin = 4;
const int echoPin = 6;
const int udServoPin = 30;
const int gripperServoPin = 12;
int GRIPPER_UD_UP=120;
int GRIPPER_KEEP=60;
int GRIPPER_RELEASE=90;

#else
#include <popx2.h>
int L_REF=636;
int R_REF=402;
int C_REF=620;
int L2_REF=620;
int R2_REF=134;
const int trigPin = 2;
const int echoPin = 3;
int GRIPPER_UD_UP=60;
int GRIPPER_KEEP=110;
int GRIPPER_RELEASE=50;
#endif



int     SPEED_t=50;
int     KEEP_DISTANCE=5;
int     GRIPPER_UD_NORMAL=90;

long    duration_t;
int     L, R, C, L2, R2;
int     error;

void show_sensor_values(){
  glcd(0,0,"L=%d   ", L);
  glcd(1,0,"C=%d   ", C);
  glcd(2,0,"R=%d   ", R);
  glcd(3,0,"L2=%d   ", L2);
  glcd(4,0,"R2=%d   ", R2);
  glcd(5,0,"Error=%d   ", error);
}

int read_sensor(){
  L   = analog(0);
  C   = analog(1);
  R   = analog(2);
  L2  = analog(4);
  R2  = analog(3);

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
  #ifdef READ_SENSOR
  show_sensor_values();
  #endif
}

void follow_line(){
  read_sensor();
  if(error > 0){
    sl(SPEED_t);
  }
  else if(error < 0){
    sr(SPEED_t);
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

void KEEP(){
  int distance = read_distance();
  while(distance > KEEP_DISTANCE || distance == 0){
    distance = read_distance();
    read_sensor();
    glcd(1,0,"D=%d   ", distance);
    follow_line();
  }
  ao();
  sound(1500, 100);
  delay(1000);
  servo_tt(2, GRIPPER_KEEP);
}

void LEREASE(){
  servo_tt(1, GRIPPER_UD_NORMAL);
  delay(1000);
  servo(2, GRIPPER_RELEASE);
  delay(1000);
}

void UP(){
  int pos = GRIPPER_UD_NORMAL;
  delay(1000);
  #ifdef XT
  for(int i=0; i<300; i++){
    if(pos < GRIPPER_UD_UP){
      pos++;
      servo(1, pos);
    }
    else{
      break;
    }
    delay(30);
  }
  #else
  for(int i=0; i<300; i++){
    if(pos > GRIPPER_UD_UP){
      pos--;
      servo_tt(1, pos);
    }
    else{
      break;
    }
    delay(30);
  }
  #endif
}

void DOWN(){
  servo_tt(1, GRIPPER_UD_NORMAL);
}

void servo_tt(int servo_port, int val){
  #ifdef XT
  if(servo_port==1){
    udServo.write(val);
  }
  else if(servo_port==2){
    gripperServo.write(val);
  }
  SoftwareServo::refresh();
  #else
  if(servo_port==1){
    servo(1, val);
  }
  else if(servo_port==2){
    servo(2, val);
  }
  #endif
}

void setup() {
  #ifdef XT
  glcdMode(3); // For XT
  udServo.attach(udServoPin);
  gripperServo.attach(gripperServoPin);
  
  // Set gripper to normal position
  servo_tt(1, GRIPPER_UD_UP);
  servo_tt(2, GRIPPER_RELEASE);
  
  #else
  glcdMode(1); // For X2
  // Set gripper to normal position
  servo_tt(1, GRIPPER_UD_NORMAL);
  servo_tt(2, GRIPPER_RELEASE);
  #endif
  
  setTextSize(2);
  glcd(0,0,"press OK...");
 
  // Ultrasocnic
  pinMode (trigPin, OUTPUT);  // Sets the trigPin as an Output
  pinMode (echoPin, INPUT); // Sets the echoPin as an Input

  //Serial.begin (9600);
  sw_ok_press();
  glcd(0,0,"started!...");
  #ifdef READ_SENSOR
  while(1){
    read_sensor();
    glcd(0,8,"D=%d         ", read_distance());
  }
  #endif
}

void loop() {
  sl(80);
  delay(1000);
  servo_tt(1, 30);
  while(1);
}
