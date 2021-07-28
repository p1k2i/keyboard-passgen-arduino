#include "preset.h"

#include <EEPROM.h>

void Preset::LoadDefault()
{
	id = 0;
	seed = 0;
	isPassEnabled = false;
	for (unsigned int i = 0; i < PASSHASH_LEN; i++)
	{
		passHash[i] = 0;
	}
	isAutoLockEnabled = true;
	passGenLen = 12;
	symbolTableName = TABLENAME_BASIC;
}

Preset::Preset()
{
	passHash = new byte[PASSHASH_LEN];
	LoadDefault();
}

void Preset::Load(const unsigned int presetAddr)
{
	id = ReadPresetByte(presetAddr, 0);
	seed = ReadPresetULong(presetAddr, E_SEED);
	isPassEnabled = (ReadPresetByte(presetAddr, E_PASSENABLED) == 1);
	for (unsigned int i = 0; i < PASSHASH_LEN; i++)
	{
		passHash[i] = ReadPresetByte(presetAddr, E_PASSHASH + i);
	}
	isAutoLockEnabled = (ReadPresetByte(presetAddr, E_AUTOLOCK) == 1);
	passGenLen = ReadPresetUInt(presetAddr, E_PASSGENLEN);
	symbolTableName = (SymbolTableName)ReadPresetByte(presetAddr, E_PASSGENTABLE);

#ifdef DEBUG
	SerialPrintPresetInfo();
#endif
}

void Preset::Save(const unsigned int presetAddr)
{
	WritePresetByte(presetAddr, 0, (presetAddr / E_PRESET_MAXLEN) + 1);
	WritePresetULong(presetAddr, E_SEED, seed);
	WritePresetByte(presetAddr, E_PASSENABLED, isPassEnabled ? 1 : 0);
	for (unsigned int i = 0; i < PASSHASH_LEN; i++)
	{
		WritePresetByte(presetAddr, E_PASSHASH + i, passHash[i]);
	}
	WritePresetByte(presetAddr, E_AUTOLOCK, isAutoLockEnabled ? 1 : 0);
	WritePresetUInt(presetAddr, E_PASSGENLEN, passGenLen);
	WritePresetByte(presetAddr, E_PASSGENTABLE, symbolTableName);

#ifdef DEBUG
	SerialPrintPresetInfo();
#endif
}

void Preset::CopyFrom(const Preset *preset)
{
	seed = preset->seed;
	isPassEnabled = preset->isPassEnabled;
	for (unsigned int i = 0; i < PASSHASH_LEN; i++)
	{
		passHash[i] = preset->passHash[i];
	}
	isAutoLockEnabled = preset->isAutoLockEnabled;
	passGenLen = preset->passGenLen;
	symbolTableName = preset->symbolTableName;
}

byte Preset::GetId()
{
	return id;
}

void Preset::DisablePass(const byte *pass, const unsigned int passLen)
{
	if (!isPassEnabled)
		return;

	isPassEnabled = false;
#ifdef DEBUG
	Serial.println("pass disabled");
#endif
	SetSeed(decryptULong(seed, pass, passLen), nullptr, 0);
	SetPassGenLen(decryptUInt(passGenLen, pass, passLen), nullptr, 0);
}

bool Preset::IsPassEnabled()
{
	return isPassEnabled;
}

void Preset::SetAutoLockEnabled(const bool enabled)
{
#ifdef DEBUG
	Serial.print("AutoLock ");
	Serial.println((isAutoLockEnabled ? "enabled" : "disabled"));
#endif
	isAutoLockEnabled = enabled;
}

bool Preset::IsAutoLockEnabled()
{
	return isAutoLockEnabled;
}

void Preset::SetSeed(const unsigned long newSeed, const byte *pass, const unsigned int passLen)
{
	if (isPassEnabled)
	{
		seed = encryptULong(newSeed, pass, passLen);
	}
	else
	{
		seed = newSeed;
	}
#ifdef DEBUG
	Serial.print("set seed: ");
	Serial.println(seed, 10);
#endif
}

unsigned long Preset::GetSeed(const byte *pass, const unsigned int passLen)
{
	if (isPassEnabled)
	{
		return decryptULong(seed, pass, passLen);
	}
	else
	{
		return seed;
	}
#ifdef DEBUG
	Serial.print("get seed: ");
	Serial.println(seed, 10);
#endif
}

bool Preset::ExistsSeed(const byte *pass, const unsigned int passLen)
{
	if (isPassEnabled)
	{
		return decryptULong(seed, pass, passLen) != 0;
	}
	return seed != 0;
}

void Preset::SetPass(const byte *pass, const unsigned int passLen)
{
	computeHash(pass, passLen, passHash, PASSHASH_LEN);
	isPassEnabled = true;
	SetSeed(seed, pass, passLen);
	SetPassGenLen(passGenLen, pass, passLen);
}

bool Preset::VerifyPass(const byte *pass, const unsigned int passLen)
{
	if (!isPassEnabled) return true;
	
	byte passHashCheck[PASSHASH_LEN];
	computeHash(pass, passLen, passHashCheck, PASSHASH_LEN);
	for (unsigned int i = 0; i < PASSHASH_LEN; i++)
	{
		if (passHashCheck[i] != passHash[i])
			return false;
	}
	return true;
}

unsigned int Preset::GetPassGenLen(const byte *pass, const unsigned int passLen)
{
	if (isPassEnabled)
	{
		return decryptUInt(passGenLen, pass, passLen);
	}
	return passGenLen;
}

void Preset::SetPassGenLen(const unsigned int len, const byte *pass, const unsigned int passLen)
{
	if (isPassEnabled)
	{
		passGenLen = encryptUInt(len, pass, passLen);
	}
	else
	{
		passGenLen = len;
	}
}

void Preset::GenPass(char *buffOut,
					 const byte *pass, const unsigned int passLen,
					 const byte *salt, const unsigned int saltLen)
{
#ifdef DEBUG
	Serial.print("Salt: ");
	SerialPrintArrBytes(salt, saltLen);
	Serial.println();
#endif
	genPass(buffOut,
			(isPassEnabled ? decryptUInt(passGenLen, pass, passLen) : passGenLen),
			symbolTableName,
			encryptULong(
				isPassEnabled ? decryptULong(seed, pass, passLen) : seed,
				salt, saltLen));
}

SymbolTableName Preset::GetSymbolTableName()
{
	return symbolTableName;
}

void Preset::SetSymbolTableName(const SymbolTableName symbolTableName)
{
	this->symbolTableName = symbolTableName;
}

// private

#ifdef DEBUG
void Preset::SerialPrintPresetInfo()
{
	Serial.println();
	Serial.println("-Preset info-");
	Serial.print("id: ");
	Serial.println(id);
	Serial.print("seed: ");
	Serial.println(seed);
	Serial.print("isPassEnabled: ");
	Serial.println(isPassEnabled);
	Serial.print("passHash: ");
	SerialPrintHexBytes(passHash, PASSHASH_LEN);
	Serial.print("isAutoLockEnabled: ");
	Serial.println(isAutoLockEnabled);
	Serial.print("passGenLen: ");
	Serial.println(passGenLen);
	Serial.print("symbolTableName: ");
	if (symbolTableName == TABLENAME_BASIC)
	{
		Serial.println("Basic");
	}
	else if (symbolTableName == TABLENAME_ADVANCED)
	{
		Serial.println("Advanced");
	}
	Serial.println();
	Serial.println();
}
#endif

byte Preset::ReadPresetByte(const unsigned int presetAddr, const unsigned int addrOffset)
{
	return EEPROM.read(presetAddr + addrOffset);
}

void Preset::WritePresetByte(const unsigned int presetAddr, const unsigned int addrOffset, const byte value)
{
	EEPROM.write(presetAddr + addrOffset, value);
}

int Preset::ReadPresetInt(const unsigned int presetAddr, const unsigned int addrOffset)
{
	return bytesToInt(
		EEPROM.read(presetAddr + addrOffset),
		EEPROM.read(presetAddr + addrOffset + 1));
}

void Preset::WritePresetInt(const unsigned int presetAddr, const unsigned int addrOffset, const int value)
{
	byte bytes[2];
	intToBytes(value, bytes);
	EEPROM.write(presetAddr + addrOffset, bytes[0]);
	EEPROM.write(presetAddr + addrOffset + 1, bytes[1]);
}

unsigned int Preset::ReadPresetUInt(const unsigned int presetAddr, const unsigned int addrOffset)
{
	return bytesToUInt(
		EEPROM.read(presetAddr + addrOffset),
		EEPROM.read(presetAddr + addrOffset + 1));
}

void Preset::WritePresetUInt(const unsigned int presetAddr, const unsigned int addrOffset, const unsigned int value)
{
	byte bytes[2];
	uIntToBytes(value, bytes);
	EEPROM.write(presetAddr + addrOffset, bytes[0]);
	EEPROM.write(presetAddr + addrOffset + 1, bytes[1]);
}

long Preset::ReadPresetLong(const unsigned int presetAddr, const unsigned int addrOffset)
{
	return bytesToLong(
		EEPROM.read(presetAddr + addrOffset),
		EEPROM.read(presetAddr + addrOffset + 1),
		EEPROM.read(presetAddr + addrOffset + 2),
		EEPROM.read(presetAddr + addrOffset + 3));
}

void Preset::WritePresetLong(const unsigned int presetAddr, const unsigned int addrOffset, const long value)
{
	byte bytes[4];
	longToBytes(value, bytes);
	EEPROM.write(presetAddr + addrOffset, bytes[0]);
	EEPROM.write(presetAddr + addrOffset + 1, bytes[1]);
	EEPROM.write(presetAddr + addrOffset + 2, bytes[2]);
	EEPROM.write(presetAddr + addrOffset + 3, bytes[3]);
}

unsigned long Preset::ReadPresetULong(const unsigned int presetAddr, const unsigned int addrOffset)
{
	return bytesToULong(
		EEPROM.read(presetAddr + addrOffset),
		EEPROM.read(presetAddr + addrOffset + 1),
		EEPROM.read(presetAddr + addrOffset + 2),
		EEPROM.read(presetAddr + addrOffset + 3));
}

void Preset::WritePresetULong(const unsigned int presetAddr, const unsigned int addrOffset, const unsigned long value)
{
	byte bytes[4];
	uLongToBytes(value, bytes);
	EEPROM.write(presetAddr + addrOffset, bytes[0]);
	EEPROM.write(presetAddr + addrOffset + 1, bytes[1]);
	EEPROM.write(presetAddr + addrOffset + 2, bytes[2]);
	EEPROM.write(presetAddr + addrOffset + 3, bytes[3]);
}
