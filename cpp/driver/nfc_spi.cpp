/*
 * Copyright 2016 <Admobilize>
 * MATRIX Labs  [http://creator.matrix.one]
 * This file is part of MATRIX Creator HAL
 *
 * MATRIX Creator HAL is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <unistd.h>
#include <string>
#include <iostream>
#include "cpp/driver/nfc_spi.h"
#include "cpp/driver/creator_memory_map.h"

namespace matrix_hal {

NFCSpi::NFCSpi() : divisor_(0x0), busy_(0x0) {}

/* Init Function for NFC SPI
   SCK = 200MHZ/(2*divisor_)
*/
bool NFCSpi::Init() {
  if (!wishbone_) return false;
  divisor_ = 0xFFFF;
  wishbone_->SpiWrite16(kNFCSpi + DIVISOR_ADDR, divisor_);
  ClearCS();
  return true;
}

bool NFCSpi::SetCS() {
  if (!wishbone_) return false;
  uint16_t cs = 0;
  wishbone_->SpiWrite16(kNFCSpi + CS_ADDR, cs);
  return true;
}

bool NFCSpi::ClearCS() {
  if (!wishbone_) return false;
  uint16_t cs = 1;
  wishbone_->SpiWrite16(kNFCSpi + CS_ADDR, cs);
  return true;
}

bool NFCSpi::IsBusy() {
  if (!wishbone_) return false;
  wishbone_->SpiRead16(kNFCSpi + BUSY_ADDR, (unsigned char *)&busy_);
  return true;
}

bool NFCSpi::Reset() {
  if (!wishbone_) return false;
  uint16_t nrst = 0;
  wishbone_->SpiWrite16(kNFCSpi + NRST_ADDR, nrst);
  usleep(10000);
  nrst = 1;
  wishbone_->SpiWrite16(kNFCSpi + NRST_ADDR, nrst);
  usleep(10000);
  return true;
}

bool NFCSpi::Transfer(uint16_t *txData, uint16_t *rxData, uint16_t length) {
  if (!wishbone_) return false;

  SetCS();

  for (int i = 0; i < length; i++) {
    wishbone_->SpiWrite16(kNFCSpi + DATA_ADDR, txData[i]);
    IsBusy();
    while (busy_) {
      IsBusy();
    }
    wishbone_->SpiRead16(kNFCSpi + DATA_ADDR, (unsigned char *)&rxData[i]);
  }

  ClearCS();
  return true;
}

bool NFCSpi::BurstTransfer(unsigned char *txData, unsigned char *rxData,
                           uint16_t length) {
  unsigned char data_tx[2];
  unsigned char data_rx[2];

  for (int i = 1; i < length; i++) {
    data_tx[0] = txData[0];
    data_tx[1] = txData[i];
    Transfer((uint16_t *)data_tx, (uint16_t *)data_rx,
                 sizeof(data_tx) / sizeof(uint16_t));
    rxData[0] = data_rx[0];
    rxData[i] = data_rx[1];
  }
  
  return true;
}

void NFCSpi::Setup(WishboneBus *wishbone) {
  MatrixDriver::Setup(wishbone);
  Reset();
  Init();
}

};  // namespace matrix_hal
