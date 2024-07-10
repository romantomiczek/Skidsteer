#include <Arduino.h>
#include "Config.h"

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
uint8_t RemoteXY_CONF[] = // 47 bytes
    {255, 4, 0, 0, 0, 40, 0, 17, 0, 0, 0, 28, 2, 106, 200, 200, 84, 1, 1, 2,
     0, 5, 230, 29, 143, 143, 23, 14, 60, 60, 32, 24, 26, 31, 5, 244, 17, 143, 143, 114,
     14, 60, 60, 32, 24, 26, 31};

// this structure defines all the variables and events of your control interface
struct
{

  // input variables
  int8_t motor_x; // from -100 to 100
  int8_t motor_y; // from -100 to 100
  int8_t arm;     // from -100 to 100
  int8_t spoon;   // from -100 to 100

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

int PWM_motor1 = 0;
int PWM_motor2 = 0;
unsigned long previousMillis = 0;

int roundTo20(int n)
{
  int a = (n / 20) * 20;
  int b = a + 20;
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
  analogWrite(MOTOR1_PIN1, LOW);
  digitalWrite(MOTOR1_PIN2, abs(PWM_motor1));

  analogWrite(MOTOR2_PIN1, LOW);
  digitalWrite(MOTOR2_PIN2, abs(PWM_motor2));
}

void motorLeft()
{
  analogWrite(MOTOR1_PIN1, LOW);
  digitalWrite(MOTOR1_PIN2, abs(PWM_motor1));

  analogWrite(MOTOR2_PIN1, abs(PWM_motor2));
  digitalWrite(MOTOR2_PIN2, LOW);
}

void motorRight()
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
void calcPWM(int x_axis, int y_axis, int arm, int spoon)
{
}

void setup()
{
  Serial.begin(115200);

  RemoteXY_Init();
  motorStop();
}

void loop()
{
  RemoteXY_Handler();

  if (millis() - previousMillis >= 100)
  {
    calcPWM(roundTo20(RemoteXY.motor_x), roundTo20(RemoteXY.motor_y), roundTo20(RemoteXY.arm), roundTo20(RemoteXY.spoon));
    previousMillis = millis();
  }
}
