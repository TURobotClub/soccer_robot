#include <ATX2.h>	// ATX2 Board

unsigned long lastRead = 0;

unsigned int ch[7] = { 0, 0, 0, 0, 0, 0, 0 }, chi = 0;

const float wheelVecs[3][2] = { {1, 0}, {-0.5, -0.866}, {-0.5, 0.866} };

void readPPM()
{
  unsigned long delta = micros() - lastRead;
  lastRead = micros();
  
  if (delta > 3000) chi = 0;
  
  ch[chi] = delta - 905;
  chi++;

  if (chi > 7) 
  {
    chi = 0;
  }
}

float input[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

void getInputVec()
{
  // ch[2] = left right +
  // ch[4] = down up    +
  // ch[3] = rotate    left right

  if (ch[0] == 0) // controller not connected
  {
    input[0] = 0.0f;
    input[1] = 0.0f;
    input[2] = 0.0f;
    input[3] = 0.0f;
    return;
  }

  input[0] = (ch[2] - 500.0f) / 512.0f;
  input[1] = (ch[4] - 500.0f) / 512.0f;
  input[2] = (ch[3] - 500.0f) / 512.0f;
  input[3] = (ch[1] - 500.0f) / 512.0f;
}

float dot(float* vec1, float* vec2)
{
  return vec1[0] * vec2[0] + vec1[1] * vec2[1];
}

void printCh()
{
  for (int i = 0; i < 7; i++)
  {
    Serial.print(ch[i]);
    Serial.print('\t');
  }

  Serial.print(input[0]);
  Serial.print('\t');
  Serial.println(input[1]);
}

void setup() {
  XIO();	// ATX2 initialize
  OK();
  glcdClear();
  glcdMode(3); // use 3 if upside down
  if (knob() > 50)
  {
    glcdFillRect(128, 0, 30, 127, colorRGB(31,27,16));
  }
  else
  {
    glcdFillRect(128, 0, 30, 127, GLCD_YELLOW);
  }
  
  Serial.begin(115200);
  lastRead = micros();
  attachInterrupt(INT2, readPPM, FALLING);

  delay(10);
}

int prevVec[2] = { 0, 0 };
void loop() {
  getInputVec();

  printCh();
  float motors[3];

  for (int i = 0; i <3; i++)
  {
    motors[i] = dot(wheelVecs[i], input);
    motors[i] += input[2] * 0.6f;
  }

  float speedscale = 20.0f * (input[3]) + 80.0f;
  for (int i = 0; i < 3; i++)
  {
    motor(i + 1, int(motors[i] * speedscale));
  }

  // input vec
  glcdLine(64, 64, prevVec[0], prevVec[1], GLCD_BLACK);
  glcdLine(64, 64, int(input[0] * 50.0f) + 64, int(input[1] * 50.0f) + 64, GLCD_WHITE);
  prevVec[0] = int(input[0] * 50.0f) + 64;
  prevVec[1] = int(input[1] * 50.0f) + 64;

  // wheel vec
  for (int i = 0; i < 3; i++)
  {
    glcdLine(64, 64, int(wheelVecs[i][0] * 60.0f) + 64, int(wheelVecs[i][1] * 60.0f) + 64, GLCD_GREEN);
    glcdLine(64, 64, int(wheelVecs[i][0] * -60.0f) + 64, int(wheelVecs[i][1] * -60.0f) + 64, GLCD_RED);
  }

  delay(10);
  
  /* output 
  for (int i = 0; i < 3; i++)
  {
    glcdLine(64, 64, int(wheelVecs[i][1] * motors[i] * 50) + 64, int(wheelVecs[i][0] * motors[i] * 50) + 64, GLCD_BLUE);
  }
  */
}
