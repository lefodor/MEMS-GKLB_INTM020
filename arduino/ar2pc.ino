// combine servo and digital input
// https://forum.arduino.cc/index.php?topic=396450

#include <Servo.h>
Servo myservo1;
Servo myservo2;

int servo1Pin = 3;
int servo2Pin = 6;
int vx=90;
int vy=90;
String receivedString ;
const byte numChars = 32;
char receivedChars[numChars];   // an array to store the received data

boolean askForData = true;
boolean newData = false;

void setup() 
{
  Serial.begin(9600);
  pinMode(servo1Pin, OUTPUT);
  pinMode(servo2Pin, OUTPUT);
  myservo1.attach(servo1Pin);
  myservo2.attach(servo2Pin);
  
  vx=90;
  vy=90;
  myservo1.write(vy);// no move
  myservo2.write(vx);// no move
}

void loop() 
{       
    // get angle from COM3    
    recvWithStartEndMarkers();
    //recvWithEndMarker2() ;
    
    // move servo
    myservo1.write(vy);
    myservo2.write(vx);

    // clean buffer
    while(Serial.available()){Serial.read();}
    
    delay(50) ;
}

void recvWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;
    unsigned int len = 0;
    char sep='.' ;
    String xcoord ;
    String ycoord ;
 
    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                len++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }

    if(len>0)
    {
      bool bsep = false ;
      for(int i=0;i<=len;i++)
      {
        if ( receivedChars[i] == sep ) bsep = true ;
        if( !bsep )  xcoord=xcoord + receivedChars[i] ;
        if( bsep && receivedChars[i] != sep )  ycoord=ycoord + receivedChars[i] ;
      }
    }

    // load only reasonlable data
    if ( xcoord.toInt() > 0 && xcoord.toInt() < 181 ){
      vx=xcoord.toInt() ;
    }
    if ( xcoord.toInt() > 0 && xcoord.toInt() < 181 ){
      vy=ycoord.toInt() ;
    }
    askForData = true;
    newData = false;   
}

void recvWithEndMarker2() {
  while (Serial.available() > 0)
  {
    receivedString = Serial.readString();
    int sep1 = receivedString.indexOf('.');
    String xcoord = receivedString.substring(0,sep1);
    String ycoord = receivedString.substring(sep1+1);
    
    vx=xcoord.toInt() ;
    vy=ycoord.toInt() ;
    askForData = true;
    newData = false;   
  }
}
