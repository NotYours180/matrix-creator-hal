/*
 * Copyright 2016 <Admobilize>
 * All rights reserved.
 */

#include <wiringPi.h>

#include <cmath>
#include <iostream>
#include <string>
#include <valarray>

#include "../cpp/driver/everloop.h"
#include "../cpp/driver/everloop_image.h"
#include "../cpp/driver/microphone_array.h"
#include "../cpp/driver/wishbone_bus.h"
#include "./fir.h"

namespace hal = matrix_hal;

int main() {
  hal::WishboneBus bus;
  bus.SpiInit();

  hal::Everloop everloop;
  everloop.Setup(&bus);

  hal::MicrophoneArray mics;
  mics.Setup(&bus);

  hal::EverloopImage image1d;

  std::valarray<float> magnitude(mics.Channels());

  for(int i=0; i<4; i++){
  mics.Read();
  magnitude = 0.0;
  for (unsigned int s = 0; s < mics.NumberOfSamples(); s++) {
    for (unsigned int c = 0; c < mics.Channels(); c++) {
      float x = mics.At(s, c);
      std::cout << x << "\t";
    }
    std::cout << std::endl;
  }
  everloop.Write(&image1d);
  
  }
}
