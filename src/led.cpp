#include "led.h"

uint8_t render_buffer[ROWS * COLS];
int currentRotation;

uint8_t positions[ROWS * COLS] = {
    0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11, 0x10, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00,
    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x27, 0x26, 0x25, 0x24, 0x23, 0x22, 0x21, 0x20, 0x37, 0x36, 0x35, 0x34, 0x33, 0x32, 0x31, 0x30,
    0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
    0x57, 0x56, 0x55, 0x54, 0x53, 0x52, 0x51, 0x50, 0x47, 0x46, 0x45, 0x44, 0x43, 0x42, 0x41, 0x40,
    0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
    0x67, 0x66, 0x65, 0x64, 0x63, 0x62, 0x61, 0x60, 0x77, 0x76, 0x75, 0x74, 0x73, 0x72, 0x71, 0x70,
    0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
    0x97, 0x96, 0x95, 0x94, 0x93, 0x92, 0x91, 0x90, 0x87, 0x86, 0x85, 0x84, 0x83, 0x82, 0x81, 0x80,
    0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
    0xa7, 0xa6, 0xa5, 0xa4, 0xa3, 0xa2, 0xa1, 0xa0, 0xb7, 0xb6, 0xb5, 0xb4, 0xb3, 0xb2, 0xb1, 0xb0,
    0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
    0xd7, 0xd6, 0xd5, 0xd4, 0xd3, 0xd2, 0xd1, 0xd0, 0xc7, 0xc6, 0xc5, 0xc4, 0xc3, 0xc2, 0xc1, 0xc0,
    0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
    0xe7, 0xe6, 0xe5, 0xe4, 0xe3, 0xe2, 0xe1, 0xe0, 0xf7, 0xf6, 0xf5, 0xf4, 0xf3, 0xf2, 0xf1, 0xf0,
    0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff};


void rotate(uint8_t matrix[ROWS * COLS], bool turnRight) {
  uint8_t SIZE = 16;
  if (turnRight) {
    currentRotation += 90;
  } else {
    currentRotation -= 90;
  }

  if (currentRotation <= -360 || currentRotation >= 360) {
    currentRotation = 0;
  }

  for (uint8_t i = 0; i < SIZE; i++) {
    for (uint8_t j = i; j < SIZE; j++) {
      uint8_t temp = matrix[i * SIZE + j];
      matrix[i * SIZE + j] = matrix[j * SIZE + i];
      matrix[j * SIZE + i] = temp;
    }
  }

  for (uint8_t i = 0; i < SIZE; i++) {
    uint8_t col1 = 0;
    uint8_t col2 = SIZE - 1;
    while (col1 < col2) {
      uint8_t index1 = i * SIZE + col1;
      uint8_t index2 = i * SIZE + col2;
      if (turnRight) {
        index1 = col1 * SIZE + i;
        index2 = col2 * SIZE + i;
      }

      uint8_t temp = matrix[index1];
      matrix[index1] = matrix[index2];
      matrix[index2] = temp;
      col1++;
      col2--;
    }
  }
};

// void rotateRight(uint8_t matrix[ROWS * COLS])
// {
//   currentRotation += 90;

//   if (currentRotation >= 360)
//   {
//     currentRotation = 0;
//   }

//   int size = ROWS;
//   for (int i = 0; i < size; i++)
//   {
//     for (int j = i; j < size; j++)
//     {
//       int temp = matrix[i * size + j];
//       matrix[i * size + j] = matrix[j * size + i];
//       matrix[j * size + i] = temp;
//     }
//   }

//   for (int i = 0; i < size; i++)
//   {
//     int col1 = 0;
//     int col2 = size - 1;
//     while (col1 < col2)
//     {
//       int temp = matrix[col1 * size + i];
//       matrix[col1 * size + i] = matrix[col2 * size + i];
//       matrix[col2 * size + i] = temp;
//       col1++;
//       col2--;
//     }
//   }
// }

// void rotateLeft(uint8_t matrix[ROWS * COLS])
// {
//   currentRotation -= 90;

//   if (currentRotation <= -360)
//   {
//     currentRotation = 0;
//   }

//   int size = ROWS;
//   for (int i = 0; i < size; i++)
//   {
//     for (int j = i; j < size; j++)
//     {
//       int temp = matrix[i * size + j];
//       matrix[i * size + j] = matrix[j * size + i];
//       matrix[j * size + i] = temp;
//     }
//   }

//   for (int i = 0; i < size; i++)
//   {
//     int col1 = 0;
//     int col2 = size - 1;
//     while (col1 < col2)
//     {
//       int temp = matrix[i * size + col1];
//       matrix[i * size + col1] = matrix[i * size + col2];
//       matrix[i * size + col2] = temp;
//       col1++;
//       col2--;
//     }
//   }
// }

void clearScreenAndBuffer(uint8_t buffer[ROWS * COLS])
{
  memset(buffer, 0, ROWS * COLS);
  renderScreen(buffer);
}

void setPixelAtIndex(uint8_t buffer[ROWS * COLS], uint8_t index, uint8_t value)
{
  if (index >= 0 && index < COLS * ROWS)
  {
    buffer[index] = value;
  }
}

void setPixel(uint8_t buffer[ROWS * COLS], uint8_t x, uint8_t y, uint8_t value)
{
  if (x >= 0 && y >= 0 && x <= ROWS && y <= COLS)
  {
    buffer[y * 16 + x] = value;
  }
}

int findPosition(uint8_t count)
{
  uint8_t wantedpos = 0;
  for (uint8_t i = 0; i < sizeof(positions); i++)
  {
    if (count == positions[i])
    {
      wantedpos = i;
      break;
    }
  }

  return wantedpos;
}

void drawLine(uint8_t buffer[ROWS * COLS], uint8_t line, bool isHorizontal)
{
  for (uint8_t i = 0; i < 16; i++)
  {
    buffer[!isHorizontal ? line * 16 + i : i * 16 + line] = 1;
  }
};

void renderScreen(uint8_t data[ROWS * COLS])
{
  for (uint8_t row = 0; row < ROWS; row++)
  {
    for (uint8_t col = 0; col < COLS; col++)
    {
      digitalWrite(PIN_DATA, data[findPosition(row * 16 + col)]);
      delayMicroseconds(10);
      digitalWrite(PIN_CLOCK, HIGH);
      delayMicroseconds(10);
      digitalWrite(PIN_CLOCK, LOW);
    }
  }

  digitalWrite(PIN_LATCH, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_LATCH, LOW);
}
