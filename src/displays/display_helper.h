#pragma once
#include <Arduino.h>
#include "display_virtual.h"
#include "display_structs.h"
#include <U8g2lib.h>


static int getTextCenterX(int textLength, int elementPositionX, int maxWidth, int8_t maxCharWidth)
{
  return elementPositionX + ((maxWidth * 0.5) - ((maxCharWidth * textLength) * 0.5));
}

static point_t getTextCenter(int textLength, display_area area, int8_t maxCharWidth, int8_t maxCharHeight)
{
  int cX = area.getX() + (area.getWidth() / 2) - ((textLength * maxCharWidth) / 2);
  int cY = area.getY() + maxCharHeight + 2;

  return point_t{
      .x = cX,
      .y = cY,
  };
}
