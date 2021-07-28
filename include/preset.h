#ifndef PRESET_H
#define PRESET_H

#include <Arduino.h>
#include "consts.h"
#include "utils.h"


class Preset
{
public:
    Preset();

    void Load(const unsigned int presetAddr);
    void LoadDefault();
    void Save(const unsigned int presetAddr);
    void CopyFrom(const Preset *preset);

    byte GetId();
    void SetSeed(const unsigned long newSeed, const byte *pass, const unsigned int passLen);
    unsigned long GetSeed(const byte *pass, const unsigned int passLen);
    bool ExistsSeed(const byte *pass, const unsigned int passLen);
    void DisablePass(const byte *pass, const unsigned int passLen);
    bool IsPassEnabled();
    void SetPass(const byte *pass, const unsigned int passLen);
    bool VerifyPass(const byte *pass, const unsigned int passLen);
    void SetAutoLockEnabled(const bool enabled);
    bool IsAutoLockEnabled();
    unsigned int GetPassGenLen(const byte *pass, const unsigned int passLen);
    void SetPassGenLen(const unsigned int len, const byte *pass, const unsigned int passLen);
    SymbolTableName GetSymbolTableName();
    void SetSymbolTableName(const SymbolTableName symbolsTableName);
    void GenPass(char *buffOut,
                 const byte *pass, const unsigned int passLen,
                 const byte *salt, const unsigned int saltLen);

private:
    byte id;
    unsigned long seed;
    bool isPassEnabled;
    byte *passHash;
    bool isAutoLockEnabled;
    unsigned int passGenLen;
    SymbolTableName symbolTableName;

#ifdef DEBUG
    void SerialPrintPresetInfo();
#endif

    byte ReadPresetByte(const unsigned int presetAddr, const unsigned int addrOffset);
    void WritePresetByte(const unsigned int presetAddr, const unsigned int addrOffset, const byte value);

    int ReadPresetInt(const unsigned int presetAddr, const unsigned int addrOffset);
    void WritePresetInt(const unsigned int presetAddr, const unsigned int addrOffset, const int value);

    unsigned int ReadPresetUInt(const unsigned int presetAddr, const unsigned int addrOffset);
    void WritePresetUInt(const unsigned int presetAddr, const unsigned int addrOffset, const unsigned int value);

    long ReadPresetLong(const unsigned int presetAddr, const unsigned int addrOffset);
    void WritePresetLong(const unsigned int presetAddr, const unsigned int addrOffset, const long value);

    unsigned long ReadPresetULong(const unsigned int presetAddr, const unsigned int addrOffset);
    void WritePresetULong(const unsigned int presetAddr, const unsigned int addrOffset, const unsigned long value);
};

#endif