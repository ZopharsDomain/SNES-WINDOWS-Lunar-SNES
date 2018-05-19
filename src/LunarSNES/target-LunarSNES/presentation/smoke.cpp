//ZSNES smoke effects implementation by Stainless et al.

#define screenWidth  288
#define screenHeight 240

auto Presentation::drawSmokeBottom(uint8_t* buffer) -> void {
  const int hotspots = 80;
  static int fire_hotspot[hotspots];
  static bool initialized = false;
  if(!initialized) {
    for(int count : range(hotspots)) {
      fire_hotspot[count] = (rand() % screenWidth);
    }
    initialized = true;
  }

  uint8_t fire_line[screenWidth];
  memory::fill(fire_line, screenWidth, 0x00);

  for(int count : range(hotspots)) {
    for(int count2 = (fire_hotspot[count] - 20); count2 < (fire_hotspot[count] + 20); count2++) {
      if(count2 >= 0 && count2 < screenWidth) {
        fire_line[count2] = min((fire_line[count2] + 20) - abs(fire_hotspot[count] - count2), 255);
      }
    }

    fire_hotspot[count] += (rand() & 7) - 3;

    if(fire_hotspot[count] < 0) {
      fire_hotspot[count] += screenWidth;
    } else if(fire_hotspot[count] >= screenWidth) {
      fire_hotspot[count] -= screenWidth;
    }
  }

  for(int count : range(screenWidth)) {
    buffer[((screenHeight - 1) * (screenWidth)) + count] = fire_line[count];
  }
}

auto Presentation::updateSmoke(uint8_t* indexedOutput) -> void {
  static uint8_t buffer[screenWidth * screenHeight];
  static bool initialized = false;
  if(!initialized) {
    for(int count : range(screenHeight)) {
      drawSmokeBottom(buffer);
      for(int y : range(screenHeight - 1)) {
        for(int x : range(screenWidth)) {
          uint8_t pixel = buffer[((y + 1) * screenWidth) + x];

          if(pixel > 0) pixel--;

          buffer[(y * screenWidth) + x] = pixel;
        }
      }
    }
    initialized = true;
  }

  drawSmokeBottom(buffer);

  for(int y : range(screenHeight - 1)) {
    for(int x : range(screenWidth)) {
      uint8_t pixel = buffer[((y + 1) * screenWidth) + x];

      if(pixel > 0) pixel--;

      buffer[(y * screenWidth) + x] = pixel;
    }
  }

  for(int y : range(screenHeight)) {
    for(int x : range(screenWidth)) {
      uint8_t pixel = indexedOutput[(y * screenWidth) + x];
      uint8_t pixel2 = buffer[(y * screenWidth) + x] >> 3;

      if(pixel2 > pixel) {
        indexedOutput[(y * screenWidth) + x] = pixel2;
      } else {
        indexedOutput[(y * screenWidth) + x] = (((pixel + pixel2) / 2) + 1);
      }
    }
  }
}

#undef screenWidth
#undef screenHeight
