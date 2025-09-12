/**********************************************************************
  Filename    : 02.4.1_Automatic_Stay_Close_To_Obstacle.ino
  Product     : Freenove 4WD Car for UNO
  Description : Automatic Stay Close To Obstacle mode. 
  Auther      : www.freenove.com
  Modification: 2019/08/15
**********************************************************************/

#define OBSTACLE_DISTANCE_MAX   150
#define OBSTACLE_DISTANCE_MID   35
#define OBSTACLE_DISTANCE_MIN 15


void initAutomatic_Stay_Close_To_Obstacle() {    
  
  //Serial.println("Started");

  servo.setPosition(90);
      
//---blink & pip
  driver.setAllLedsColor(LedColor::White); //Set all LED color to white  
  buzzer.toneOn(5000, 500);
  driver.setAllLedsColor(LedColor::Dark);    //set all LED off .
}

void loopAutomatic_Stay_Close_To_Obstacle() {

  updateAutomaticStayCloseToAbstacle();  
}

void updateAutomaticStayCloseToAbstacle()
{
    int sumDistance = 0;
    int distances[5];
    for (int j = 0; j < 5; j++) {
      int distance = sonar.getDistance();
      distances[j] = distance;
      sumDistance += distance;
      //delayMicroseconds(2 * SONIC_TIMEOUT);      
    }      

    int distances_length = sizeof(distances) / sizeof(distances[0]);
  // qsort - last parameter is a function pointer to the sort function
    qsort(distances, distances_length, sizeof(distances[0]), sort_desc);
      
      int midDistance = distances[2];// sumDistance / 5;
      if(midDistance <= OBSTACLE_DISTANCE_MIN)
      {
        driver.setAllLedsColor(LedColor::Red); //Set all LED color to red  
        //buzzer.toneOn(12000,500);
        driver.backward(150); //Move back                 
      } 
      else if( midDistance > OBSTACLE_DISTANCE_MIN && midDistance <= OBSTACLE_DISTANCE_MID)
      {
        driver.setAllLedsColor(LedColor::Blue); //Set all LED color to blue  
        //buzzer.toneOn(10000, 500);
        driver.stop();
      }
      else if( midDistance > OBSTACLE_DISTANCE_MID && midDistance < OBSTACLE_DISTANCE_MAX)
      {
        driver.setAllLedsColor(LedColor::Green); //Set all LED color to green  
        //buzzer.toneOn(7000, 500);
        driver.forward(120);
      }
      else if( midDistance >= OBSTACLE_DISTANCE_MAX)
      {
        driver.setAllLedsColor(LedColor::White); //Set all LED color to white
        //buzzer.toneOn(5000, 500);
        driver.stop();
      }
}

// qsort requires you to create a sort function
int sort_desc(const void *cmp1, const void *cmp2)
{
  // Need to cast the void * to int *
  int a = *((int *)cmp1);
  int b = *((int *)cmp2);
  // The comparison
  return a > b ? -1 : (a < b ? 1 : 0);
  // A simpler, probably faster way:
  //return b - a;
}
