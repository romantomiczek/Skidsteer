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

#define DEBUG 0

#if DEBUG == 1
#define debug(x) Serial.print(x)
#define debugln(x) Serial.println(x)
#else
#define debug(x)
#define debugln(x)
#endif

int PWM_motor1 = PWM_VALUE;
int PWM_motor2 = PWM_VALUE;
unsigned long previousMillis = 0;

int carMove = 0;
int carTurn = 0;
int armMove = 0;
int bucketMove = 0;
int armPosition = 180;
int bucketPosition = 0;

Servo armServo;
Servo bucketServo;
int pos = 0;

int roundTo20(int n)
{
  int a = (n / 20) * 20;
  int b = a + 20;
  return (n - a > b - n) ? b : a;
}

int roundTo100(int n)
{
  int a = (n / 100) * 100;
  int b = a + 100;
  return (n - a > b - n) ? b : a;
}

void motorForward()
{
  analogWrite(MOTOR1_PIN1, abs(PWM_motor1));
  digitalWrite(MOTOR1_PIN2, LOW);

  analogWrite(MOTOR2_PIN1, abs(PWM_motor2));
  digitalWrite(MOTOR2_PIN2, LOW);
}

void motorBackward()
{
  digitalWrite(MOTOR1_PIN1, LOW);
  analogWrite(MOTOR1_PIN2, abs(PWM_motor1));

  digitalWrite(MOTOR2_PIN1, LOW);
  analogWrite(MOTOR2_PIN2, abs(PWM_motor2));
}

void motorRight()
{
  digitalWrite(MOTOR1_PIN1, LOW);
  analogWrite(MOTOR1_PIN2, abs(PWM_motor1));

  analogWrite(MOTOR2_PIN1, abs(PWM_motor2));
  digitalWrite(MOTOR2_PIN2, LOW);
}

void motorLeft()
{
  analogWrite(MOTOR1_PIN1, abs(PWM_motor1));
  digitalWrite(MOTOR1_PIN2, LOW);

  analogWrite(MOTOR2_PIN1, LOW);
  digitalWrite(MOTOR2_PIN2, abs(PWM_motor2));
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
      Serial.println("Forward");
    }
    else
    {
      motorBackward();
      Serial.println("Backward");
    }
  }
  else if (carMove == 0 && carTurn != 0)
  {
    if (carTurn > 0)
    {
      motorLeft();
    }
    else
    {
      motorRight();
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
    armPosition = armPosition + 3;
    armPosition > 180 ? armPosition = 180 : armPosition;
  }
  else if (armMove < -90)
  {
    armPosition = armPosition - 3;
    armPosition < 0 ? armPosition = 0 : armPosition;
  }
  armServo.write(armPosition);
}

void bucketControl()
{
  if (bucketMove > 90)
  {
    bucketPosition = bucketPosition + 3;
    bucketPosition > 180 ? bucketPosition = 180 : bucketPosition;
  }
  else if (bucketMove < -90)
  {
    bucketPosition = bucketPosition - 3;
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

  //  if (millis() - previousMillis >= 50)
  // {
  previousMillis = millis();
  // Serial.println(roundTo100(RemoteXY.motor_x) + "\t" + roundTo100(RemoteXY.motor_y));

  carMove = RemoteXY.motor_y / 100;
  carTurn = RemoteXY.motor_x / 100;
  armMove = RemoteXY.arm;
  bucketMove = RemoteXY.bucket;

  /* Serial.print(carMove);
  Serial.print("\t");
  Serial.println(carTurn); */

  Serial.print(armPosition);
  Serial.print("\t");
  Serial.println(bucketPosition);

  carControl();
  armControl();
  bucketControl();
  // }
}
