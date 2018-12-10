#include "F:\project\lp_robot\robot_code_20181130\LP_robot_LP.h"

void setup(){
  lp_init();
   
  KEEP_DISTANCE   =   4 ;     /*"ระยะเจอกระป๋อง" */
  FOUND_DISTANCE  =   8 ;     /*"ระยะเจอสิ่งกีดขวาง" */
  UP              =   90 ;     /*"ยกแขน ทำมุม...องศากับพื้น" */
  DOWN            =    0 ;     /*"วางแขน ทำมุม...องศากับพื้น" */                                                       
  KEEP_t          =   84 ;     /*"จับกระป๋อง (ค่ามากหนีบแน่นมาก)" */
  RELEASE_t       =   45 ;     /*"ปล่อยกระป๋อง (ค่าน้อยกางมือมาก)" */

  /*"เขียนคำสั่งใต้บรรทัดนี้" */ 

  FFS();FF();FFS();FL();FFS();FL();FR();FL();FFS();KEEP();BR();
  FL();FR();FFS();FF();FFS();FL();FFS();RLB();

  BB();FFS();FF();FF();KEEP();BB();FFS();FF();FFS();RLB(); 

  BB();FR();FFS();FF();FFS();FR();KEEP();BB();FFS();FR();FFS();
  FL();FR();FL();FR();FFS();FF();FR();FFS();RLB();

  BB();FL();FFS();FF();FL();FR();FL();FR();FFS();FL();FR();FFS();FR();FL();FFS();FF();ao();         
} 


void loop(){ 
  
}
