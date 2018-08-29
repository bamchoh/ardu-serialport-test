#include <pt.h>

static struct pt pt1, pt2;

static char buf[2048];
static int tail;
static char buf1[2048];
static int tail1;

static int protothread1(struct pt *pt) {
  static unsigned long timestamp = 0;
  PT_BEGIN(pt);
  while(1) {
    PT_WAIT_UNTIL(pt, millis() - timestamp > 1000);
    timestamp = millis();
    flushLED(22);
  }
  PT_END(pt);
}

static int protothread2(struct pt *pt) {
  static unsigned long timestamp = 0;
  unsigned char ch;
  PT_BEGIN(pt);
  while(1) {
    PT_WAIT_UNTIL(pt, millis() - timestamp > 10);
    timestamp = millis();
    if(Serial.available() > 0) {
      ch = Serial.read();
      buf[tail++] = ch;
    }
  }
  PT_END(pt);
}

void setup() {
  pinMode(22, OUTPUT);
  Serial.begin(9600);
  Serial1.begin(9600, SERIAL_8E1);
  tail = 0;
  tail1 = 0;
  PT_INIT(&pt1);
  PT_INIT(&pt2);
}

void flushLED(int led) {
  digitalWrite(led, HIGH);
  delay(1);
  digitalWrite(led, LOW);
}

void loop() {
  unsigned char ch;
  protothread1(&pt1);
  protothread2(&pt2);
  if(Serial1.available() > 0) {
    ch = Serial1.read();
    buf1[tail1++] = ch;
    if(strcmp(buf1, "read") == 0) {
      memset(buf1, 0, tail1);
      tail1 = 0;

      if(tail > 0) {
        for(int i = 0;i < tail;i++) {
          Serial1.write(buf[i]);
        }
        Serial1.flush();
        tail = 0;
      } else {
        Serial1.write("nothing");
        Serial1.flush();
      }
    }
  }
}
