#include <Adafruit_PWMServoDriver.h>

/// PWM
/// 20ms (50Hz) dead band width 5us[1.024]
/// 0deg    90deg   180deg
/// -90deg  0deg    90deg
/// 1ms     1.5ms   2ms
/// 1/20    1.5/20  2/20
/// 204.8   307.2   409.6

byte LED = 10;
char buf[16] = { 0, };
int pos = 0;
int scale = 0;
int g_select = 0;
int g_angle = 0;
bool tourInIdle = true;
Adafruit_PWMServoDriver driver = Adafruit_PWMServoDriver();
float angle = 0.0f;
float angle_delta = 0.25f;
enum class State {
  Idle,
  Aim,
  Lock,
  Receive,
} state = State::Idle;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ;
  }
  driver.begin();
  driver.setPWMFreq(50.0f);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
  Serial.println("Start");
}

void loop() {
  // Serial input
  if (Serial.available()) {
    state = State::Receive;
    Serial.readBytesUntil('\n', buf, 15);
    buf[15] = '\0';

    while (Serial.available()) {
      Serial.read();
    }
  }

  // input interprete
  if (state == State::Receive) {
    if (buf[0] == 'D') {
      // 012345678
      // D NNN NNN\n
      pos = (buf[2] - '0') * 100
            + (buf[3] - '0') * 10
            + (buf[4] - '0');
      scale = (buf[6] - '0') * 10
              + (buf[7] - '0');
      state = State::Aim;
      Serial.print("Detect at ");
      Serial.print(pos);
      Serial.print(" with size ");
      Serial.print(scale);
      Serial.print('\n');
    } else if (buf[0] == 'T') {
      // 0
      // T\n
      tourInIdle = !tourInIdle;
      Serial.print("Touring set ");
      Serial.print(tourInIdle);
      Serial.print('\n');
    } else if (buf[0] == 'G') {
      // 0123456
      // G N SNN\n
      g_select = (buf[2] - '0');
      g_angle = (buf[5] - '0') * 10
              + (buf[6] - '0');
      if (buf[4] == '-') {
        g_angle = -g_angle;
      }
      driver.setPWM(g_select,
                    1.024,
                    map(g_angle, -90.0, 90.0, 204.8, 409.6));
      Serial.print("Servo ");
      Serial.print(g_select);
      Serial.print(" to ");
      Serial.print(g_angle);
      Serial.print("\n");
    } else if (buf[0] == 'S') {
      // 0
      // S\n
      Serial.print("Status\n");
      Serial.print("tourInIdle:");
      Serial.println(tourInIdle);
      Serial.print("buf:");
      Serial.println(buf);
      Serial.print("state:");
      Serial.println(byte(state));
      Serial.print("angle:");
      Serial.println(angle);
    }

    if (state == State::Receive) {
      state = State::Idle;
    }
  }

  // tour if idle and set tour in idle
  if (state == State::Idle && tourInIdle) {
    angle += angle_delta;
    if (angle > 90) {
      angle = 90;
      angle_delta = -angle_delta;
    } else if (angle < -90) {
      angle = - 90;
      angle_delta = -angle_delta;
    }
    driver.setPWM(0,
                  1.024,
                  map(angle, -90.0, 90.0, 204.8, 409.6));
    delay(5); // tour delay
  }

  // aim taget if target detected
  if (state == State::Aim) {
    int delta = map(pos, 0, 640, -32, 32);
    angle += delta;
    if (angle > 90) {
      angle = 90;
    } else if (angle < -90) {
      angle = -90;
    }
    driver.setPWM(0,
                  1.024,
                  map(angle, -90.0, 90.0, 204.8, 409.6));
    delay(200); // Turn delay
    state = State::Lock;
  }

  // long distance reaction to target if taget locked and far
  if (state == State::Lock && scale >= 40) {
    driver.setPWM(4,
                  1.024,
                  map(45, -90.0, 90.0, 204.8, 409.6));
    delay(500); // Action delay
    driver.setPWM(4,
                  1.024,
                  map(0, -90.0, 90.0, 204.8, 409.6));
    state = State::Idle;
  }

  // close distance reaction to target if target locked and close
  if (state == State::Lock && scale < 40) {
    driver.setPWM(4,
                  1.024,
                  map(45, -90.0, 90.0, 204.8, 409.6));
    int interval = 500 / 10;
    for (int i = 0; i < 5; ++i) {
      digitalWrite(LED, LOW);
      delay(interval);
      digitalWrite(LED, HIGH);
      delay(interval);
    }
    driver.setPWM(4,
                  1.024,
                  map(0, -90.0, 90.0, 204.8, 409.6));
    state = State::Idle;
  }
}
