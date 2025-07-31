#include <IRremote.h>
#include <Servo.h>

const int trigPin = 11, echoPin = 3; // Ultra-Sonic Distance Sensor Pins
const int recvPin = 2; // IR Remote Receiver Pin
IRrecv irrecv(recvPin);
decode_results results;
Servo srv; // Servo Motor

class Motor
{
  public:
    int m_IN1, m_IN2; // Input Pins
    int m_EN; // Enable Pin

    Motor(int IN1, int IN2, int EN) : m_IN1(IN1), m_IN2(IN2), m_EN(EN) {} // Initializer

    void begin() // Initialize motor
    {
      pinMode(m_IN1, OUTPUT);
      pinMode(m_IN2, OUTPUT);
      pinMode(m_EN , OUTPUT);
      stop();
    }

    void moveForward(int speed)
    {
      digitalWrite(m_IN1, HIGH);
      digitalWrite(m_IN2, LOW);
      analogWrite(m_EN, speed);
    }

    void moveBackward(int speed)
    {
      digitalWrite(m_IN1, LOW);
      digitalWrite(m_IN2, HIGH);
      analogWrite(m_EN, speed);
    }

    void stop()
    {
      digitalWrite(m_IN1, LOW);
      digitalWrite(m_IN2, LOW);
      analogWrite(m_EN, 0);
    }
};

Motor MR(4, 5, 9), ML(6, 7, 10); // Right Motor, Left Motor
bool isOn = false; // Check Power

void ultraSonic() // Ultra-Sonic Sensor distance check
{
  // Send signal
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Calculate distance with time duration
  long duration = pulseIn(echoPin, HIGH);
  int distance = duration * 0.034 / 2;
  Serial.println(distance);
  if (distance <= 15)
    srv.write(130); // open DRS using Servo Motors
  else
    srv.write(0); // close DRS
}

void setup()
{
  Serial.begin(9600);
  IrReceiver.begin(recvPin, ENABLE_LED_FEEDBACK);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  MR.begin();
  ML.begin();
  srv.attach(8);
  srv.write(0);
}

void loop()
{
  //ultraSonic();
  if (IrReceiver.decode())
  {
    // Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX); // remote HEX code
    if (IrReceiver.decodedIRData.decodedRawData == 0xBA45FF00) // Power Button pressed
    {
      isOn = !isOn;
    }
    if (isOn)
    {
      digitalWrite(12, HIGH);
      switch (IrReceiver.decodedIRData.decodedRawData)
      {
      case 0xBC43FF00: // ▶▶ Button pressed
        MR.moveForward(255);
        ML.moveForward(255);
        break;

      case 0xBF40FF00: // ▶|| Button pressed
        MR.stop();
        ML.stop();
        break;

      case 0xBB44FF00: // ◀◀ Button pressed
        MR.moveBackward(255);
        ML.moveBackward(255);
        break;

      case 0xF609FF00: // ▲ Button pressed
        MR.moveForward(255);
        ML.stop();
        break;

      case 0xF807FF00: // ▼ Button pressed
        MR.stop();
        ML.moveForward(255);
        break;

      case 0xE619FF00: // EQ, for debug
        srv.write(130);
        digitalWrite(13, HIGH);
        break;
      
      case 0xF20DFF00: // ST/REPT, for debug
        srv.write(0);
        digitalWrite(13, LOW);
        break;

      default:
        break;
      }
    }
    else digitalWrite(12, LOW);
    IrReceiver.resume();
  }
}
