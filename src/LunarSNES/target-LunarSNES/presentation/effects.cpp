#include "snow.cpp"
#include "water.cpp"
#include "burning.cpp"
#include "smoke.cpp"

auto Presentation::updateEffect(Effect effect, uint32_t* output, uint8_t* indexedOutput) -> void {
  switch(effect) {
  case Effect::Snow:    updateSnow(output);   break;
  case Effect::WaterA:  updateWater(indexedOutput, 0); break;
  case Effect::WaterB:  updateWater(indexedOutput, 1); break;
  case Effect::Burning: updateBurning(indexedOutput); break;
  case Effect::Smoke:   updateSmoke(indexedOutput); break;
  case Effect::TVNoise: {
    uint32_t noise;
    for(uint y : range(240)) {
      for(uint x : range(256)) {
        noise = 0xff << 24 | (random() & 0xff) * 0x010101;
        *(output + (y >> 8) * 1024 + (x >> 8) * 2 +   0) = noise;
        *(output + (y >> 8) * 1024 + (x >> 8) * 2 +   1) = noise;
        *(output + (y >> 8) * 1024 + (x >> 8) * 2 + 512) = noise;
        *(output + (y >> 8) * 1024 + (x >> 8) * 2 + 513) = noise;
      }
    }
    break;
  }
  }
}

auto Presentation::effect() -> void {
  if(!video) return;

  uint32_t* output;
  uint length;

  string effectName = settings["UI/Effect"].text();
  Effect effect = Effect::None;
  if(effectName == "Snow")    effect = Effect::Snow;
  if(effectName == "WaterA")  effect = Effect::WaterA;
  if(effectName == "WaterB")  effect = Effect::WaterB;
  if(effectName == "Burning") effect = Effect::Burning;
  if(effectName == "Smoke")   effect = Effect::Smoke;
  if(effectName == "TVNoise") effect = Effect::TVNoise;

  bool useIndexedOutput = (
     effect == Effect::WaterA
  || effect == Effect::WaterB
  || effect == Effect::Burning
  || effect == Effect::Smoke
  );

  uint width  = 512;
  uint height = 480;

  if(emulator->information.overscan) {
    uint overscanHorizontal = settings["Video/Overscan/Horizontal"].natural();
    uint overscanVertical = settings["Video/Overscan/Vertical"].natural();
    auto information = emulator->videoInformation();
    overscanHorizontal *= information.internalWidth / information.width;
    overscanVertical *= information.internalHeight / information.height;
    width -= overscanHorizontal * 2;
    height -= overscanVertical * 2;
  }

  if(video->lock(output, length, width, height)) {
    uint8_t indexedOutput[288 * 240];
    if(useIndexedOutput) {
      for(uint y : range(height / 2)) {
        auto dp = indexedOutput + y * 288;
        for(uint _ : range(16)) *dp++ = 0x01;
        for(uint x : range(width / 2)) {
          if(emulator->loaded() && videoBuffer) {
            uint32 pixel = *(videoBuffer + (y * 2) * (length >> 2) + x * 2);
            *dp++ = 1 + (pixel.byte(0) + pixel.byte(1) + pixel.byte(2)) / 26;
          } else {
            *dp++ = 0x01;
          }
        }
        for(uint _ : range(16)) *dp++ = 0x01;
      }
    } else {
      uint32_t back = (0xff << 24
      | ((15 << 2) + 4) << 16
      | ((10 << 2) + 4) <<  8
      | ((31 << 2) + 4) <<  0
      );
      for(uint y : range(height)) {
        auto dp = output + y * (length >> 2);
        for(uint x : range(width)) {
          if(emulator->loaded() && videoBuffer) {
            uint32 pixel = *(videoBuffer + y * (length >> 2) + x);
            uint8_t luma = ((pixel.byte(0) + pixel.byte(1) + pixel.byte(2)) / 3) >> 1;
            *dp++ = back + (luma << 16 | luma << 8 | luma << 0);
          } else {
            *dp++ = back;
          }
        }
      }
    }
    updateEffect(effect, output, indexedOutput);
    if(useIndexedOutput) {
      for(uint y : range(height / 2)) {
        auto dp0 = output + (y * 2) * (length >> 2) + 0;
        auto dp1 = output + (y * 2) * (length >> 2) + width;
        auto di = indexedOutput + y * 288 + 16;
        for(uint x : range(width / 2)) {
          uint8_t luma = *di++;
          uint32_t color = luma == 0 ? 0xff000000 : (0xff << 24
          | ((15 << 2) + luma * 4) << 16
          | ((10 << 2) + luma * 4) <<  8
          | ((31 << 2) + luma * 4) <<  0
          );
          *dp0++ = color;
          *dp0++ = color;
          *dp1++ = color;
          *dp1++ = color;
        }
      }
    }

    video->unlock();
    video->output();
  }
}
