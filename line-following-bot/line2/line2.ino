/*  LP Robot
 *  Lasted update 2018-10-10
 */

#include <popx2.h>  

// For  X2
int L_REF=636;
int R_REF=402;
int C_REF=620;
int L2_REF=620;
int R2_REF=134;
int KEEP_DISTANCE=5;
int SPEED=50;

// For XT
//int L_REF=695;
//int R_REF=672;
//int C_REF=642;
//int L2_REF=642;
//int R2_REF=642;

int GRIPPER_UD_NORMAL=90;
int GRIPPER_UD_UP=60;
int GRIPPER_KEEP=110;
int GRIPPER_RELEASE=50;

const int trigPin = 2;
const int echoPin = 3;
long duration_t;
int L, R, C, L2, R2;
int error;

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

  //show_sensor_values();
}

void follow_line(){
  read_sensor();
  if(error > 0){
    sl(50);
  }
  else if(error < 0){
    sr(50);
  }
  else{
    fd(50);
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
  sr(30); delay(200);
  read_sensor();
  while(C > C_REF){
    read_sensor();
    sr(30);
  }
  ao();
}

void FL(){
  ao(); delay(50);
  sl(30); delay(200);
  read_sensor();
  while(C > C_REF){
    read_sensor();
    sl(30);
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
  while(read_distance() > KEEP_DISTANCE){
    read_sensor();
    follow_line();
  }
  ao();
  sound(1500, 100);
  delay(1000);
  servo(2, GRIPPER_KEEP);
}

void LEREASE(){
  servo(2, GRIPPER_RELEASE);
//  bk(SPEED);
}

void UP(){
  int pos = GRIPPER_UD_NORMAL;
  delay(1000);
  for(int i=0; i<300; i++){
    if(pos > GRIPPER_UD_UP){
      pos--;
      servo(1, pos);
    }
    else{
      break;
    }
    delay(30);
  }
}

void DOWN(){
  servo(1, GRIPPER_UD_NORMAL);
}

void setup() {
  glcdMode(3); // For XT
  glcdMode(1); // For XT
  setTextSize(2);
  glcd(0,0,"press OK...");
  
  glcd(0,0,"started!...");

  // Set gripper to normal position
  servo(1, GRIPPER_UD_NORMAL);
  servo(2, GRIPPER_RELEASE);

  // Ultrasocnic
  pinMode (trigPin, OUTPUT);  // Sets the trigPin as an Output
  pinMode (echoPin, INPUT); // Sets the echoPin as an Input

  Serial.begin (9600);
  sw_ok_press();
}

void loop() {

  FF(); FL(); while(1);
  
  
  
  
  //read_distance();
  
  //RELEASE(); delay(1000); while(1);
  //RE(); delay(1000);

  ///////////////////////////////////
  //glcd(0,0,"Stopped......");
  //while(1);
}
