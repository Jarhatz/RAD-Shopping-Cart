#include <Nextion.h>
#include <SoftwareSerial.h>

SoftwareSerial espSerial(10, 7); // RX, TX

#define len(arr) sizeof(arr) / sizeof(arr[0])

// NexObject Declarations
NexButton powerBtn = NexButton(0, 5, "powerBtn"); // Power Button
NexButton b4 = NexButton(4, 6, "b4"); // Pair Button
NexText t1 = NexText(4, 7, "t1"); // SSID Text
NexText t2 = NexText(4, 8, "t2"); // PWD Text
NexText isFollow = NexText(3, 6, "isFollow"); // isFollow Text
NexButton followBtn = NexButton(3, 5, "followBtn"); // Follow Button
NexText fESP = NexText(7, 7, "fESP"); // Front ESP Text
NexText blESP = NexText(7, 8, "blESP"); // Backleft ESP Text
NexText brESP = NexText(7, 9, "brESP"); // BackRight ESP Text
NexPicture trackPic = NexPicture(7, 13, "trackPic"); // Track picture

int currentPage = 0;

// NexPage Declarations
NexPage powerPage = NexPage(0, 0, "powerPage");
NexPage userPage = NexPage(1, 0, "userPage");
NexPage storePage = NexPage(2, 0, "storePage");
NexPage drivePage = NexPage(3, 0, "drivePage");
NexPage pairPage = NexPage(4, 0, "pairPage");
NexPage typeSSIDPage = NexPage(5, 0, "typeSSIDPage");
NexPage typePWDPage = NexPage(6, 0, "typePWDPage");
NexPage trackPage = NexPage(7, 0, "trackPage");

char buffer[60] = {0};

NexTouch *nex_listen_list[] = {
  &powerBtn,
  &b4,
  &followBtn,
  &powerPage,
  &userPage,
  &storePage,
  &drivePage,
  &pairPage,
  &typeSSIDPage,
  &typePWDPage,
  &trackPage,
  NULL // String terminated
};

// RSSI Queue Attributes
const int QUEUE_SIZE = 10;

int frontQueue[QUEUE_SIZE] = {0};
int frontQueueFront = 0;
int frontQueueBack = -1;
int frontQueueItemCount = 0;

int rightQueue[QUEUE_SIZE] = {0};
int rightQueueFront = 0;
int rightQueueBack = -1;
int rightQueueItemCount = 0;

int leftQueue[QUEUE_SIZE] = {0};
int leftQueueFront = 0;
int leftQueueBack = -1;
int leftQueueItemCount = 0;

int leftMotorPin1 = 52;
int leftMotorPin2 = 51;
int leftMotorPinE = 53;
int rightMotorPin1 = 50;
int rightMotorPin2 = 49;
int rightMotorPinE = 48;

struct RADShoppingCart {
  int power;
  String ssid;
  String pwd;
  String frontESP;
  String leftESP;
  String rightESP;
  int follow;
}; RADShoppingCart cart;

// SETUP //
void setup() {
  Serial.begin(9600); // DEBUG SERIAL
  Serial2.begin(9600); // LCD SERIAL
  Serial1.begin(9600); // Front ESP SERIAL
  Serial3.begin(9600); // Left ESP SERIAL

  pinMode(10, INPUT);
  pinMode(7, OUTPUT);
  espSerial.begin(9600); // Right ESP SERIAL

  // LCD SETUP //
  powerBtn.attachPush(powerBtnPushCallback);
  b4.attachPush(b4PushCallback);
  followBtn.attachPush(followBtnPushCallback);
  powerPage.attachPush(powerPagePushCallback);
  userPage.attachPush(userPagePushCallback);
  storePage.attachPush(storePagePushCallback);
  drivePage.attachPush(drivePagePushCallback);
  pairPage.attachPush(pairPagePushCallback);
  typeSSIDPage.attachPush(typeSSIDPagePushCallback);
  typePWDPage.attachPush(typePWDPagePushCallback);
  trackPage.attachPush(trackPagePushCallback);
  
  // MOTOR SETUP //
  pinMode(rightMotorPin1, OUTPUT);
  pinMode(leftMotorPin2, OUTPUT);
  pinMode(rightMotorPin1, OUTPUT);
  pinMode(leftMotorPin2, OUTPUT);
  pinMode(leftMotorPinE, OUTPUT);
  pinMode(rightMotorPinE, OUTPUT);

  // Initialize RAD Shopping Cart
  cart = {0, "", "", "", "", "", 0};
}

// TOUCH EVENT FUNCTIONS //
void powerPagePushCallback(void *ptr) {
  currentPage = 0;
}

void userPagePushCallback(void *ptr) {
  currentPage = 1;
}

void storePagePushCallback(void *ptr) {
  currentPage = 2;
}

void drivePagePushCallback(void *ptr) {
  currentPage = 3;
}

void pairPagePushCallback(void *ptr) {
  currentPage = 4;
}

void typeSSIDPagePushCallback(void *ptr) {
  currentPage = 5;
}

void typePWDPagePushCallback(void *ptr) {
  currentPage = 6;
}

void trackPagePushCallback(void *ptr) {
  currentPage = 7;
}

void trackPageRefresh() {
  if (currentPage == 7) {
    // Send frontESP value to LCD on trackPage
    Serial2.print("fESP.txt=");
    Serial2.print("\"");
    Serial2.print(cart.frontESP);
    Serial2.print("\"");
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.write(0xff);
    
    // Send backLeftESP value to LCD on trackPage
    Serial2.print("blESP.txt=");
    Serial2.print("\"");
    Serial2.print(cart.leftESP);
    Serial2.print("\"");
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.write(0xff);

    // Send backRightESP value to LCD on trackPage
    Serial2.print("brESP.txt=");
    Serial2.print("\"");
    Serial2.print(cart.rightESP);
    Serial2.print("\"");
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.write(0xff);

    // Send trackPicture value to LCD on trackPage
    Serial2.print("trackPic.pic=");
    Serial2.print(getUserDirection());
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.write(0xff);
  }
}

int getUserDirection() {
  // Delay for 0.2 seconds
  delay(200);
  float frontVal = normalizedFrontESP();
  float leftVal = normalizedLeftESP();
  // float rightVal = normalizedRightESP();
  // float min = min(leftVal, rightVal);

  if (frontVal <= leftVal) {
    return 27;
  } else {
    return 28;
  }
}

void powerBtnPushCallback(void *ptr) {
  if (cart.power) {
    cart.power = 0;
    cart.ssid = "";
    cart.pwd = "";
    sendResetSignal();
  } else {
    cart.power = 1;
  }
}

void sendResetSignal() {
  String message = "RESET\nRESET";
  Serial1.println(message);
  Serial3.println(message);
  espSerial.println(message);
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
  String message = "LOGIN " + cart.ssid + "\n" + cart.pwd;
  Serial1.println(message);
  Serial3.println(message);
  espSerial.println(message);
}

void followBtnPushCallback(void *ptr) {
  // Clear and buffer and hold value of isFollow
  memset(buffer, 0, sizeof(buffer));
  isFollow.getText(buffer, sizeof(buffer));
  cart.follow = atoi(buffer);
}

void readESP() {
  // Read Front ESP
  if (Serial1.available()) {
    String data = Serial1.readStringUntil('\n');
    if (data.startsWith("RSSI ")) {
      cart.frontESP = data.substring(5);
    } else {
      cart.frontESP = "DC";
    }
  }
  // Read BackLeft ESP
  if (Serial3.available()) {
    String data = Serial3.readStringUntil('\n');
    if (data.startsWith("RSSI ")) {
      cart.leftESP = data.substring(5);
    } else {
      cart.leftESP = "DC";
    }
  }
  // Read BackRight ESP
  if (espSerial.available()) {
    String data = espSerial.readStringUntil('\n');
    if (data.startsWith("RSSI ")) {
      cart.rightESP = data.substring(5);
    } else {
      cart.rightESP = "DC";
    }
  }
}

float normalizedFrontESP() {
  if (cart.frontESP != "DC") {
    int intVal = cart.frontESP.toInt();
    if (frontQueueItemCount == QUEUE_SIZE) {
      int popped = pop(frontQueue, frontQueueFront, frontQueueBack, frontQueueItemCount);
      push(frontQueue, frontQueueFront, frontQueueBack, frontQueueItemCount, intVal);
      return getAverage(frontQueue, frontQueueFront, frontQueueBack, frontQueueItemCount);
    } else {
      push(frontQueue, frontQueueFront, frontQueueBack, frontQueueItemCount, intVal);
      return getAverage(frontQueue, frontQueueFront, frontQueueBack, frontQueueItemCount);
    }
  } else {
    return getAverage(frontQueue, frontQueueFront, frontQueueBack, frontQueueItemCount);
  }
}

float normalizedLeftESP() {
  if (cart.leftESP != "DC") {
    int intVal = cart.leftESP.toInt();
    if (leftQueueItemCount == QUEUE_SIZE) {
      int popped = pop(leftQueue, leftQueueFront, leftQueueBack, leftQueueItemCount);
      push(leftQueue, leftQueueFront, leftQueueBack, leftQueueItemCount, intVal);
      return getAverage(leftQueue, leftQueueFront, leftQueueBack, leftQueueItemCount);
    } else {
      push(leftQueue, leftQueueFront, leftQueueBack, leftQueueItemCount, intVal);
      return getAverage(leftQueue, leftQueueFront, leftQueueBack, leftQueueItemCount);
    }
  } else {
    return getAverage(leftQueue, leftQueueFront, leftQueueBack, leftQueueItemCount);
  }
}

float normalizedRightESP() {
  if (cart.rightESP != "DC") {
    int intVal = cart.rightESP.toInt();
    if (rightQueueItemCount == QUEUE_SIZE) {
      int popped = pop(rightQueue, rightQueueFront, rightQueueBack, rightQueueItemCount);
      push(rightQueue, rightQueueFront, rightQueueBack, rightQueueItemCount, intVal);
      return getAverage(rightQueue, rightQueueFront, rightQueueBack, rightQueueItemCount);
    } else {
      push(rightQueue, rightQueueFront, rightQueueBack, rightQueueItemCount, intVal);
      return getAverage(rightQueue, rightQueueFront, rightQueueBack, rightQueueItemCount);
    }
  } else {
    return getAverage(rightQueue, rightQueueFront, rightQueueBack, rightQueueItemCount);
  }
}

// QUEUE FUNCTIONS //
void push(int queue[], int& front, int& back, int& itemCount, int value) {
  if (itemCount == QUEUE_SIZE) {
    // Queue is full, cannot push any more items
    return;
  }
  
  back = (back + 1) % QUEUE_SIZE;
  queue[back] = value;
  itemCount++;
}

int pop(int queue[], int& front, int& back, int& itemCount) {
  if (itemCount == 0) {
    // Queue is empty, cannot pop any more items
    return -1;
  }
  
  int value = queue[front];
  front = (front + 1) % QUEUE_SIZE;
  itemCount--;
  return value;
}

float getAverage(int queue[], int front, int back, int itemCount) {
  if (itemCount == 0) {
    // Queue is empty, return 0
    return 0;
  }
  
  float sum = 0;
  for (int i = front; i <= back; i++) {
    sum += queue[i];
  }
  
  return sum / itemCount;
}

// MOVEMENT FUNCTIONS //
void moveForward(int leftSpeed, int rightSpeed) {
  analogWrite(leftMotorPinE, leftSpeed);
  analogWrite(rightMotorPinE, rightSpeed);

  digitalWrite(rightMotorPin1, HIGH);
  digitalWrite(leftMotorPin2, LOW);

  digitalWrite(rightMotorPin1, HIGH);
  digitalWrite(leftMotorPin2, LOW);
}

// LOOP //
void loop() {

  nexLoop(nex_listen_list); // Check for touch events on LCD

  // Receive RSSI value from ESPs
  readESP();

  // Refresh the trackingPage on LCD
  trackPageRefresh();

  Serial.println(cart.rightESP);
}