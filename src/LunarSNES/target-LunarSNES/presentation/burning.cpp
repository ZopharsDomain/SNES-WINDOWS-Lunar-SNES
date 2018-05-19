//ZSNES fire effects implementation by Frank Jan Sorensen, Joachim Fenkes,
//Stefan Goehler, Jonas Quinn, et al.

#define screenWidth 288
#define screenHeight 240

auto Presentation::updateBurning(uint8_t* indexedOutput) -> void {
  const uint rootrand = 20;  //Max/Min decrease of the root of the flames
  const uint decay    = 5;  //How far should the flames go up on the screen? This MUST be positive - JF
  const uint topY     = 1;  //Starting line of the flame routine.
  const int smooth    = 1;  //How discrete can the flames be?
  const int minfire   = 50;  //limit between the "starting to burn" and the "is burning" routines
  const int leftX     = 0;  //Starting position on each scanline. Must be a multiple of 4.
  const int rightX    = screenWidth - 1;  //Ending position on the scanline.
  const int width     = screenWidth;
  const int increase  = 3;  //3 = Wood, 90 = Gasoline
  const bool morefire = 1;

  assert((leftX & 3) == 0);

  static uint8_t pt[screenWidth * screenHeight];

  static uint8_t flamearray[screenWidth];
  static bool initialized = false;
  if(!initialized) {
    for(int i = leftX; i <= rightX; i++) {
      flamearray[i] = 0;
    }

    memory::fill(pt, screenHeight * screenWidth, 0x00);

    initialized = 1;
  }

  //Put the values from FlameArray on the bottom line of the screen
  memory::copy(pt + ((screenHeight - 1) * screenWidth) + leftX, flamearray, width);

  //This loop makes the actual flames
  for(int i = leftX; i <= rightX; i++) {
    for(uint j = topY; j <= screenHeight - 1; j++) {
      int v = pt[j * screenWidth + i];
      if(v == 0 || v < decay || i <= leftX || i >= rightX) {
        pt[(j - 1) * screenWidth + i] = 0;
      } else {
        pt[(j - 1) * screenWidth + (i - (rand() % 3 - 1))] = v - rand() % decay;
      }
    }
  }

  //Match?
  if(rand() % 150 == 0) {
    memory::fill(flamearray + leftX + rand() % (rightX - leftX - 5), 5, 0xff);
  }

  //This loop controls the "root" of the flames, i.e. the values in FlameArray.
  for(int i = leftX; i <= rightX; i++) {
    int x = flamearray[i];

    if(x < minfire) {  //Increase by the "burnability"
      //Starting to burn:
      if(x > 10) x += rand() % increase;
    } else {
      //Otherwise randomize and increase by intensity (is burning)
      x += rand() % (rootrand * 2 + 1) - rootrand + morefire;
    }
    if(x > 255) x = 255;  //X Too large?
    flamearray[i] = x;
  }

  //Smoothen the values of FrameArray to avoid "discrete" flames
  int p = 0;
  for(int i = leftX + smooth; i <= rightX - smooth; i++) {
    int x = 0;
    for(int j = -smooth; j <= smooth; j++) x += flamearray[i + j];
    flamearray[i] = x / ((smooth << 1) + 1);
  }

  for(int x = 0; x < screenWidth * screenHeight; x++) {
    int i = indexedOutput[x];
    int j = pt[x] >> 3;

    indexedOutput[x] = j > i ? j : ((i + j) >> 1) + 1;
  }
}

#undef screenWidth
#undef screenHeight
