#ifndef NRF52840_DHKE_
#define NRF52840_DHKE_
#include <Adafruit_nRFCrypto.h>
#include "Adafruit_nRFCrypto.h"

uint64_t P = 0xffffffffffffffc5;
uint64_t G = 5;
uint64_t randomint64();
uint64_t power(uint64_t, uint64_t);

uint64_t randomint64() {
  uint64_t a;
  uint8_t j, x, bf[8];
  nRFCrypto.Random.generate(bf, 8);
  for (j = 0; j < 8; j++) {
    a = (a << 8) | bf[j];
  }
  return a;
}

// Power function to return value of a ^ b mod P
uint64_t power(uint64_t a, uint64_t b) {
  if (b == 1) return a;
  uint64_t t = power(a, b / 2);
  if (b % 2 == 0) return (t * t) % P;
  else return (((t * t) % P) * a) % P;
}

union BigKey {
  uint8_t oneChunk[16]; // occupies 16 bytes
  uint64_t twoWords[2]; // occupies 16 bytes
}; // the whole union occupies 16 bytes

class nrf52DHKEKey {
  public:
    nrf52DHKEKey(void);
    BigKey getKey(void);
  private:
    BigKey Key;
};

nrf52DHKEKey::nrf52DHKEKey() {
  Key.twoWords[0] = randomint64();
  Key.twoWords[1] = randomint64();
}

BigKey nrf52DHKEKey::getKey() {
  return Key;
}

class nrf52_DHKE_Set {
  public:
    nrf52_DHKE_Set(void);
    bool begin();
    int encrypt(char *, uint16_t, char *, BigKey);
    int decrypt(char *, uint16_t, char *, BigKey);
    BigKey getPub();
    BigKey getPvt();
  private:
    nrf52DHKEKey pubKey;
    nrf52DHKEKey pvtKey;
    BigKey SecretKey;
    nRFCrypto_AES aes;
};

nrf52_DHKE_Set::nrf52_DHKE_Set() {
}

bool nrf52_DHKE_Set::begin() {
  nRFCrypto.begin();
  if (!aes.begin()) {
    return false;
  }
  return true;
}

int nrf52_DHKE_Set::encrypt(char *pDataIn, uint16_t msgLen, char *pDataOut, BigKey BobPublic) {
  uint8_t i;
  uint8_t myIV[16];
  BigKey PV = pvtKey.getKey();
  SecretKey.twoWords[0] = power(BobPublic.twoWords[0], PV.twoWords[0]);
  SecretKey.twoWords[1] = power(BobPublic.twoWords[1], PV.twoWords[1]);
  int err = aes.Process(pDataIn, msgLen, myIV, SecretKey.oneChunk, 16, pDataOut, aes.encryptFlag, aes.ecbMode);
  return err;
}

int nrf52_DHKE_Set::decrypt(char *pDataIn, uint16_t msgLen, char *pDataOut, BigKey BobPublic) {
  uint8_t i;
  uint8_t myIV[16];
  BigKey PV = pvtKey.getKey();
  SecretKey.twoWords[0] = power(BobPublic.twoWords[0], PV.twoWords[0]);
  SecretKey.twoWords[1] = power(BobPublic.twoWords[1], PV.twoWords[1]);
  int err = aes.Process(pDataIn, msgLen, myIV, SecretKey.oneChunk, 16, pDataOut, aes.decryptFlag, aes.ecbMode);
  return err;
}

BigKey nrf52_DHKE_Set::getPub() {
  return pubKey.getKey();
}

BigKey nrf52_DHKE_Set::getPvt() {
  return pvtKey.getKey();
}

#endif