#include <Arduino.h>
#include "Config.h"
#include <Servo.h>

// you can enable debug logging to Serial at 115200
// #define REMOTEXY__DEBUGLOG

// RemoteXY select connection mode and include library
#define REMOTEXY_MODE__WIFI_POINT

#include <ESP8266WiFi.h>

// RemoteXY connection settings
#define REMOTEXY_WIFI_SSID "RemoteXY"
#define REMOTEXY_WIFI_PASSWORD "12345678"
#define REMOTEXY_SERVER_PORT 6377
#define REMOTEXY_ACCESS_PASSWORD "12345678"

#include <RemoteXY.h>

// RemoteXY GUI configuration
#pragma pack(push, 1)
uint8_t RemoteXY_CONF[] = // 37 bytes
    {255, 4, 0, 0, 0, 30, 0, 17, 0, 0, 0, 28, 1, 200, 84, 1, 1, 2, 0, 5,
     23, 14, 60, 60, 32, 24, 26, 31, 5, 114, 14, 60, 60, 32, 24, 26, 31};

// this structure defines all the variables and events of your control interface
struct
{

  // input variables
  int8_t motor_x; // from -100 to 100
  int8_t motor_y; // from -100 to 100
  int8_t bucket;  // from -100 to 100
  int8_t arm;     // from -100 to 100

  // other variable
  uint8_t connect_flag; // =1 if wire connected, else =0

} RemoteXY;
#pragma pack(pop)

#define DEBUG 1

#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif

unsigned long previousMillis = 0;

int carMove = 0;
int carTurn = 0;
int armMove = 0;
int bucketMove = 0;
int armPosition = 150;
int bucketPosition = 180;

Servo armServo;
Servo bucketServo;


void motorForward()
{
  analogWrite(MOTOR1_PIN1, abs(PWM_VALUE_MOVE));
  digitalWrite(MOTOR1_PIN2, LOW);

  analogWrite(MOTOR2_PIN1, abs(PWM_VALUE_MOVE));
  digitalWrite(MOTOR2_PIN2, LOW);
}

void motorBackward()
{
  digitalWrite(MOTOR1_PIN1, LOW);
  analogWrite(MOTOR1_PIN2, abs(PWM_VALUE_MOVE));

  digitalWrite(MOTOR2_PIN1, LOW);
  analogWrite(MOTOR2_PIN2, abs(PWM_VALUE_MOVE));
}

void motorLeft()
{
  digitalWrite(MOTOR1_PIN1, LOW);
  analogWrite(MOTOR1_PIN2, abs(PWM_VALUE_TURN));

  analogWrite(MOTOR2_PIN1, abs(PWM_VALUE_TURN));
  digitalWrite(MOTOR2_PIN2, LOW);
}

void motorRight()
{
  analogWrite(MOTOR1_PIN1, abs(PWM_VALUE_TURN));
  digitalWrite(MOTOR1_PIN2, LOW);

  digitalWrite(MOTOR2_PIN1, LOW);
  analogWrite(MOTOR2_PIN2, abs(PWM_VALUE_TURN));
}

void motorStop()
{
  digitalWrite(MOTOR1_PIN1, LOW);
  digitalWrite(MOTOR1_PIN2, LOW);
  digitalWrite(MOTOR2_PIN1, LOW);
  digitalWrite(MOTOR2_PIN2, LOW);
}

void carControl()
{
  if (carMove != 0 && carTurn == 0)
  {
    if (carMove > 0)
    {
      motorForward();
      debugln("Forward");
    }
    else
    {
      motorBackward();
      debugln("Backward");
    }
  }
  else if (carMove == 0 && carTurn != 0)
  {
    if (carTurn > 0)
    {
      motorRight();
      debugln("Right");
    }
    else
    {
      motorLeft();
      debugln("Left");
    }
  }
  else
  {
    motorStop();
  }
}

void armControl()
{
  if (armMove > 90)
  {
    armPosition = armPosition + SERVO_STEP;
    armPosition > 180 ? armPosition = 180 : armPosition;
  }
  else if (armMove < -90)
  {
    armPosition = armPosition - SERVO_STEP;
    armPosition < 0 ? armPosition = 0 : armPosition;
  }
  armServo.write(armPosition);
}

void bucketControl()
{
  if (bucketMove > 90)
  {
    bucketPosition = bucketPosition + SERVO_STEP;
    bucketPosition > 180 ? bucketPosition = 180 : bucketPosition;
  }
  else if (bucketMove < -90)
  {
    bucketPosition = bucketPosition - SERVO_STEP;
    bucketPosition < 0 ? bucketPosition = 0 : bucketPosition;
  }
  bucketServo.write(bucketPosition);
}

void setup()
{
  Serial.begin(115200);
  armServo.attach(SERVO_PIN_1);
  bucketServo.attach(SERVO_PIN_2);
  RemoteXY_Init();
  // motorStop();
}

void loop()
{
  RemoteXY_Handler();

  if (millis() - previousMillis >= 5)
  {
    previousMillis = millis();

    carMove = RemoteXY.motor_y / 100;
    carTurn = RemoteXY.motor_x / 100;
    armMove = -RemoteXY.arm;
    bucketMove = -RemoteXY.bucket;

    /* debug(carMove);
    debug("\t");
    debugln(carTurn); */

    /* debug(armPosition);
    debug("\t");
    debugln(bucketPosition); */

    carControl();
    armControl();
    bucketControl();
  }
}
