#include <Nextion.h>

#define len(arr) sizeof(arr) / sizeof(arr[0])

NexButton powerBtn = NexButton(0, 5, "powerBtn"); // Power Button
NexButton b4 = NexButton(4, 6, "b4"); // Pair Button
NexText t1 = NexText(4, 7, "t1"); // SSID Text
NexText t2 = NexText(4, 8, "t2"); // PWD Text
NexText fESP = NexText(7, 7, "fESP");
NexText bESP = NexText()


char buffer[60] = {0};

NexTouch *nex_listen_list[] = {
  &powerBtn,
  &b4,
  NULL // String terminated
};

int leftMotorPin1 = 2;
int leftMotorPin2 = 3;
int leftMotorPinA = 6;
int rightMotorPin1 = 4;
int rightMotorPin2 = 5;
int rightMotorPinA = 7;

struct RADShoppingCart {
  int power;
  String ssid;
  String pwd;
}; RADShoppingCart cart;

// SETUP //
void setup() {
  Serial.begin(9600); // DEBUG SERIAL
  Serial1.begin(9600); // F_ESP SERIAL
  Serial2.begin(9600); // LCD SERIAL
  Serial3.begin(9600); // B_ESP SERIAL

  // LCD SETUP //
  powerBtn.attachPush(powerBtnPushCallback);
  b4.attachPush(b4PushCallback);
  
  // MOTOR SETUP //
  pinMode(rightMotorPin1, OUTPUT);
  pinMode(leftMotorPin2, OUTPUT);
  pinMode(rightMotorPin1, OUTPUT);
  pinMode(leftMotorPin2, OUTPUT);
  pinMode(leftMotorPinA, OUTPUT);
  pinMode(rightMotorPinA, OUTPUT);

  cart = {0, 0, 0, "", ""}; // Initialize RAD Shopping Cart
}

// TOUCH EVENT FUNCTIONS //
void powerBtnPushCallback(void *ptr) {
  if (cart.power) {
    cart.power = 0;
    cart.ssid = "";
    cart.pwd = "";
    sendResetSignal();
  } else {
    cart.power = 1;
  }
  Serial.print("POWER ");
  Serial.println(cart.power);
}

void sendResetSignal() {
  String message = "RESET\nRESET";
  Serial1.println(message);
  Serial3.println(message);
}

void b4PushCallback(void *ptr) {
  // Clear and buffer and hold value of SSID
  memset(buffer, 0, sizeof(buffer));
  t1.getText(buffer, sizeof(buffer));
  cart.ssid = String(buffer);

  // Clear and buffer and hold value of PWD
  memset(buffer, 0, sizeof(buffer));
  t2.getText(buffer, sizeof(buffer));
  cart.pwd = String(buffer);

  sendWiFiLogin();
}

void sendWiFiLogin() {
  String message = cart.ssid + "\n" + cart.pwd;
  Serial1.println(message);
  Serial3.println(message);
}

void readFrontESP() {
  if (Serial1.available()) {
    String data = Serial1.readStringUntil('\n');
    Serial.print("F_ESP: ");
    Serial.println(data);
  }
}

void readBackESP() {
  if (Serial3.available()) {
    String data = Serial3.readStringUntil('\n');
    Serial.print("B_ESP: ");
    Serial.println(data);
  }
}

// MOVEMENT FUNCTIONS //
void moveForward(int leftSpeed, int rightSpeed) {
  analogWrite(leftMotorPinA, leftSpeed);
  analogWrite(rightMotorPinA, rightSpeed);

  digitalWrite(rightMotorPin1, HIGH);
  digitalWrite(leftMotorPin2, LOW);

  digitalWrite(rightMotorPin1, HIGH);
  digitalWrite(leftMotorPin2, LOW);
}

// LOOP //
void loop() {

  nexLoop(nex_listen_list); // Check for touch events on LCD

  // Receive RSSI value from FRONT_ESP
  readFrontESP();
  readBackESP();
  
}
