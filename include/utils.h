#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
#include "consts.h"

void computeHash(const byte *data, const byte dataLen, byte *hashOut, const unsigned int hashLen);

#ifdef DEBUG
void SerialPrintHexBytes(const byte *bytes, const int length);
void SerialPrintArrBytes(const byte *arr, const int length);
#endif

int bytesToInt(const byte byte0, const byte byte1);
void intToBytes(const int value, byte bytesOut[2]);

unsigned int bytesToUInt(const byte byte0, const byte byte1);
void uIntToBytes(const unsigned int value, byte bytesOut[2]);

long bytesToLong(const byte byte0, const byte byte1, const byte byte2, const byte byte3);
void longToBytes(const long value, byte bytesOut[4]);

unsigned long bytesToULong(const byte byte0, const byte byte1, const byte byte2, const byte byte3);
void uLongToBytes(const unsigned long value, byte bytesOut[4]);

int encryptInt(const int val, const byte *key, const unsigned int keyLength);
int decryptInt(const int val, const byte *key, const unsigned int keyLength);

unsigned int encryptUInt(const unsigned int val, const byte *key, const unsigned int keyLength);
unsigned int decryptUInt(const unsigned int val, const byte *key, const unsigned int keyLength);

long encryptLong(const long val, const byte *key, const unsigned int keyLength);
long decryptLong(const long val, const byte *key, const unsigned int keyLength);

unsigned long encryptULong(const unsigned long val, const byte *key, const unsigned int keyLength);
unsigned long decryptULong(const unsigned long val, const byte *key, const unsigned int keyLength);

void encryptBytes(const byte *inputBytes, const unsigned int bytesLen,
                const byte *key, const unsigned int keyLength,
                byte *outputBytes);

void decryptBytes(const byte *inputBytes, const unsigned int bytesLen,
                const byte *key, const unsigned int keyLength,
                byte *outputBytes);

void genPass(char *buffOut, const unsigned int passLength,
             const SymbolTableName symbolTableName,
             const unsigned long seed);

#endif