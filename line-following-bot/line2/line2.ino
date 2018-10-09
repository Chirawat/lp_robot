#include <popxt.h>	

int L_REF=695;
int R_REF=672;
int C_REF=642;
int L2_REF=642;
int R2_REF=642;

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

int read_sensor()
{
  L =analog(0);
  C =analog(1);
  R =analog(2);

  L2 = analog(4);
  R2 = analog(3);

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

void setup() {
  glcdMode(3); // For XT
  setTextSize(2);
  glcd(0,0,"press OK...");
  sw_ok_press();
  glcd(0,0,"started!...");
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
void loop() {
  FF(); FL();FF();
  glcd(0,0,"Stopped......");
  while(1);
}
