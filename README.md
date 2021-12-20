# nRF52840_DHKE

This is a library providing basic Diffie–Hellman key exchange for [my extension](https://github.com/Kongduino/Adafruit_nRFCrypto) to the [Adafruit_nRFCrypto library](https://github.com/adafruit/Adafruit_nRFCrypto).

It requires the following pieces:

* `nRFCrypto_Random`, to produce the public and private keys
* `nRFCrypto_AES`, for the encryption (AES 128)

Given a `nrf52_DHKE_Set` Alice, which has received another `nrf52_DHKE_Set` Bob's public key (with `Bob.getPub()`) as `BobPub`:

```c
  nrf52_DHKE_Set Alice;
  if (!Alice.begin()) {
    Serial.println("Error initing Alice!");
    return;
  }
  int err = Alice.encrypt(pDataIn, msgLen, pDataOut, BobPub);
  if (err == 0) {
    hexDump(pDataOut, msgLen);
  } else {
    Serial.println("Encryption error: " + String(err));
    return;
  }
```

Bob can decrypt the message, with `AlicePub`:

```c
  int err = Bob.decrypt(msg, msgLen, plainText, AlicePub);
  if (err == 0) {
    hexDump(plainText, msgLen);
  } else {
    Serial.println("Encryption error: " + String(err));
    return;
  }
```

This uses ECB. If you want the more secure CBC, add a 16-byte IV to the function call:

```c
  uint8_t myIV[16];
  nRFCrypto.Random.generate(myIV, 16); // Keep in mind Bob should have a copy of that
  // You need to transmit IV and cipher.
  err = Alice.encrypt(myIV, pDataIn, msgLen, pDataOut, Bob.getPub());
  if (err == 0) {
    hexDump(pDataOut, msgLen);
  } else {
    Serial.println("Encryption error: " + String(err));
    return;
  }
```

Decryption:

```c
  int err = Bob.decrypt(myIV, msg, msgLen, plainText, AlicePub);
  if (err == 0) {
    hexDump(plainText, msgLen);
  } else {
    Serial.println("Encryption error: " + String(err));
    return;
  }
```
