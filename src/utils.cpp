#include "utils.h"

#include <SpritzCipher.h>

/*
void computeHash256(const byte *data, const byte dataLen, byte hashOut[32])
{
  spritz_ctx hash_ctx; // the CTX for chunk by chunk API

  // Print input
#ifdef DEBUG
  SerialPrintArrBytes(data, dataLen);
  Serial.println();
#endif

  spritz_hash_setup(&hash_ctx); 
  // For easy test: code add a byte each time
  for (unsigned int i = 0; i < dataLen; i++)
  {
    spritz_hash_update(&hash_ctx, data + i, 1);
  }

  spritz_hash(hashOut, 32, data, dataLen);

#ifdef DEBUG
  SerialPrintHexBytes(hashOut, 32);
#endif
}*/

void computeHash(const byte *data, const byte dataLen, byte *hashOut, const unsigned int hashLen)
{
  spritz_ctx hash_ctx; /* the CTX for chunk by chunk API */

  /* Print input */
#ifdef DEBUG
  SerialPrintArrBytes(data, dataLen);
  Serial.println();
#endif

  spritz_hash_setup(&hash_ctx); 
  /* For easy test: code add a byte each time */
  for (unsigned int i = 0; i < dataLen; i++)
  {
    spritz_hash_update(&hash_ctx, data + i, 1);
  }

  spritz_hash(hashOut, hashLen, data, dataLen);

#ifdef DEBUG
  SerialPrintHexBytes(hashOut, hashLen);
#endif
}

#ifdef DEBUG
void SerialPrintHexBytes(const byte *bytes, const int length)
{
  for (int i = 0; i < length; i++)
  {
    if (bytes[i] < 0x10)
    { /* To print "0F" not "F" */
      Serial.write('0');
    }
    Serial.print(bytes[i], HEX);
  }
  Serial.println();
}

void SerialPrintArrBytes(const byte *bytes, const int length)
{
  Serial.print("[ ");
  for (int i = 0; i < length; i++)
  {
    Serial.print(bytes[i], 10);
    Serial.print(" ");
  }
  Serial.print("]");
}
#endif

int bytesToInt(const byte byte0, const byte byte1)
{
  return int((int)byte0 << 8 |
             (int)byte1);
}

void intToBytes(const int value, byte bytesOut[2])
{
  bytesOut[0] = (byte)(value >> 8);
  bytesOut[1] = (byte)(value);
}

unsigned int bytesToUInt(const byte byte0, const byte byte1)
{
  return (unsigned int)((unsigned int)byte0 << 8 |
             (unsigned int)byte1);
}

void uIntToBytes(const unsigned int value, byte bytesOut[2])
{
  bytesOut[0] = (byte)(value >> 8);
  bytesOut[1] = (byte)(value);
}

long bytesToLong(const byte byte0, const byte byte1, const byte byte2, const byte byte3)
{
  return long((long)byte0 << 24 |
              (long)byte1 << 16 |
              (long)byte2 << 8 |
              (long)byte3);
}

void longToBytes(const long value, byte bytesOut[4])
{
  bytesOut[0] = (byte)(value >> 24);
  bytesOut[1] = (byte)(value >> 16);
  bytesOut[2] = (byte)(value >> 8);
  bytesOut[3] = (byte)(value);
}

unsigned long bytesToULong(const byte byte0, const byte byte1, const byte byte2, const byte byte3)
{
  return (unsigned long)((unsigned long)byte0 << 24 |
              (unsigned long)byte1 << 16 |
              (unsigned long)byte2 << 8 |
              (unsigned long)byte3);
}

void uLongToBytes(const unsigned long value, byte bytesOut[4])
{
  bytesOut[0] = (byte)(value >> 24);
  bytesOut[1] = (byte)(value >> 16);
  bytesOut[2] = (byte)(value >> 8);
  bytesOut[3] = (byte)(value);
}

int encryptInt(const int val, const byte *key, const unsigned int keyLength)
{
  byte byteBuffer[2];
  intToBytes(val, byteBuffer);
  encryptBytes(byteBuffer, 2, key, keyLength, byteBuffer);
  return bytesToInt(byteBuffer[0], byteBuffer[1]);
}

int decryptInt(const int val, const byte *key, const unsigned int keyLength)
{
  byte byteBuffer[2];
  intToBytes(val, byteBuffer);
  decryptBytes(byteBuffer, 2, key, keyLength, byteBuffer);
  return bytesToInt(byteBuffer[0], byteBuffer[1]);
}

unsigned int encryptUInt(const unsigned int val, const byte *key, const unsigned int keyLength)
{
  byte byteBuffer[2];
  uIntToBytes(val, byteBuffer);
  encryptBytes(byteBuffer, 2, key, keyLength, byteBuffer);
  return bytesToUInt(byteBuffer[0], byteBuffer[1]);
}

unsigned int decryptUInt(const unsigned int val, const byte *key, const unsigned int keyLength)
{
  byte byteBuffer[2];
  uIntToBytes(val, byteBuffer);
  decryptBytes(byteBuffer, 2, key, keyLength, byteBuffer);
  return bytesToUInt(byteBuffer[0], byteBuffer[1]);
}

long encryptLong(const long val, const byte *key, const unsigned int keyLength)
{
  byte byteBuffer[4];
  longToBytes(val, byteBuffer);
  encryptBytes(byteBuffer, 4, key, keyLength, byteBuffer);
  return bytesToLong(byteBuffer[0], byteBuffer[1], byteBuffer[2], byteBuffer[3]);
}

long decryptLong(const long val, const byte *key, const unsigned int keyLength)
{
  byte byteBuffer[4];
  longToBytes(val, byteBuffer);
  decryptBytes(byteBuffer, 4, key, keyLength, byteBuffer);
  return bytesToLong(byteBuffer[0], byteBuffer[1], byteBuffer[2], byteBuffer[3]);
}

unsigned long encryptULong(const unsigned long val, const byte *key, const unsigned int keyLength)
{
  byte byteBuffer[4];
  uLongToBytes(val, byteBuffer);
  encryptBytes(byteBuffer, 4, key, keyLength, byteBuffer);
  return bytesToULong(byteBuffer[0], byteBuffer[1], byteBuffer[2], byteBuffer[3]);
}

unsigned long decryptULong(const unsigned long val, const byte *key, const unsigned int keyLength)
{
  byte byteBuffer[4];
  uLongToBytes(val, byteBuffer);
  decryptBytes(byteBuffer, 4, key, keyLength, byteBuffer);
  return bytesToULong(byteBuffer[0], byteBuffer[1], byteBuffer[2], byteBuffer[3]);
}

void encryptBytes(const byte *inputBytes, const unsigned int bytesLen,
                const byte *key, const unsigned int keyLength,
                byte *outputBytes)
{

#ifdef DEBUG
  SerialPrintArrBytes(inputBytes, bytesLen);
  Serial.print(" -encrypt> ");
#endif

  if (outputBytes != inputBytes)
  {
    for (unsigned int i = 0; i < bytesLen; i++)
    {
      outputBytes[i] = inputBytes[i];
    }
  }

  for (unsigned int i = 0; i < max(keyLength, bytesLen); i++)
  {
    outputBytes[i % bytesLen] = outputBytes[i % bytesLen] ^ key[keyLength - (i % keyLength) - 1];
  }

#ifdef DEBUG
  SerialPrintArrBytes(outputBytes, bytesLen);
  Serial.println();
#endif
}

void decryptBytes(const byte *inputBytes, const unsigned int bytesLen,
                const byte *key, const unsigned int keyLength,
                byte *outputBytes)
{

#ifdef DEBUG
  SerialPrintArrBytes(inputBytes, bytesLen);
  Serial.print(" -decrypt> ");
#endif

  if (outputBytes != inputBytes)
  {
    for (unsigned int i = 0; i < bytesLen; i++)
    {
      outputBytes[i] = inputBytes[i];
    }
  }
  
  for (unsigned int i = max(keyLength, bytesLen); i > 0; i--)
  {
    outputBytes[(i-1) % bytesLen] = outputBytes[(i-1) % bytesLen] ^ key[keyLength - ((i-1) % keyLength) - 1];
  }

#ifdef DEBUG
  SerialPrintArrBytes(outputBytes, bytesLen);
  Serial.println();
#endif
}

void genPass(char *buffOut, const unsigned int passLength,
             const SymbolTableName symbolTableName,
             const unsigned long seed)
{
  randomSeed(seed);
#ifdef DEBUG
  const long rand1 = random();
#endif


  if (symbolTableName == SymbolTableName::TABLENAME_BASIC)
  {
    for (unsigned int i = 0; i < passLength; i++)
    {
      buffOut[i] = SYMBOL_TABLE_BASIC[((unsigned int)random()) % SYMBOL_TABLE_BASIC_LEN];
    }
  }
  else

    if (symbolTableName == SymbolTableName::TABLENAME_ADVANCED)
  {
    for (unsigned int i = 0; i < passLength; i++)
    {
      buffOut[i] = SYMBOL_TABLE_ADVANCED[((unsigned int)random()) % SYMBOL_TABLE_ADVANCED_LEN];
    }
  }
  else

    if (symbolTableName == SymbolTableName::TABLENAME_NUMBERS)
  {
    for (unsigned int i = 0; i < passLength; i++)
    {
      buffOut[i] = SYMBOL_TABLE_NUMBERS[((unsigned int)random()) % SYMBOL_TABLE_NUMBERS_LEN];
    }
  }

  buffOut[passLength] = '\0';

  #ifdef DEBUG
    Serial.println();
    Serial.println("Generate pass:");
    Serial.print("len: ");
    Serial.println(passLength);
    Serial.print("SymbolTableName: ");
    Serial.println( (symbolTableName == SymbolTableName::TABLENAME_ADVANCED ? "Advanced" : "Basic") );
		Serial.print("Seed: ");
		Serial.println(seed, 10);
		Serial.print("First random value: ");
		Serial.println(rand1, 10);
		Serial.print("Generated pass: ");
		Serial.println(buffOut);
    Serial.println();
	#endif
}
