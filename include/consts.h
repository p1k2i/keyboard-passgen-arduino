#ifndef CONSTS_H
#define CONSTS_H

#include <Arduino.h>

//#define DEBUG

const unsigned int PASSHASH_LEN = 32;
const unsigned int PASSGEN_MAX_LEN = 512;
const unsigned int PASSGEN_DEFAULT_LEN = 12;

const unsigned long AUTOLOCK_DELAY_SEC = 35;
const unsigned long LOOP_DELAY_MS = 45;

// ---- eeprom ----

const unsigned int E_PRESET_MAXLEN = 100;
// 1000 - 100*4 = 600 free

const unsigned int E_PRESET1 = 0;
const unsigned int E_PRESET2 = E_PRESET1 + E_PRESET_MAXLEN;
const unsigned int E_PRESET3 = E_PRESET2 + E_PRESET_MAXLEN;
const unsigned int E_PRESET4 = E_PRESET3 + E_PRESET_MAXLEN;

const unsigned int E_ID = 0;                   // 1 byte
const unsigned int E_SEED = E_ID + 1;          // 2 bytes
const unsigned int E_PASSENABLED = E_SEED + 4; // 1 byte
const unsigned int E_PASSHASH = E_PASSENABLED + 1; // 32 bytes
const unsigned int E_AUTOLOCK = E_PASSHASH + PASSHASH_LEN; // 1 byte
const unsigned int E_PASSGENLEN = E_AUTOLOCK + 1; // 2 bytes
const unsigned int E_PASSGENTABLE = E_PASSGENLEN + 2; // 1 byte
// total 40 bytes

// ---- pin ----

const int PIN_BTN_RESET = 14;
const int PIN_KEYBOARD = A3;

enum Menu
{
    MENU_MAIN,
    MENU_SETTINGS,
    MENU_PASSGEN,
    MENU_SETTINGCHANGE,
    MENU_PRESETAUTH,
    MENU_PRESETWELLCOME
};

const int BTN_CODES[] =
{
  0, // random
  132, // settings
  235, // 0
  316, // 9
  382, // 8
  437, // 7
  484, // 4
  524, // 5
  558, // 6
  588, // cancel
  613, // preset 3
  636, // preset 4
  658, // preset 2
  676, // preset 1
  693, // ok
  708, // 3
  722, // 2
  735, // 1
};

enum Btn
{
    BTN_UNKNOWN = -1,
    BTN_RANDOM = 0,
    BTN_SETTINGS,
    BTN_0,
    BTN_9,
    BTN_8,
    BTN_7,
    BTN_4,
    BTN_5,
    BTN_6,
    BTN_CANCEL,
    BTN_PRESET3,
    BTN_PRESET4,
    BTN_PRESET2,
    BTN_PRESET1,
    BTN_OK,
    BTN_3,
    BTN_2,
    BTN_1
};

enum Setting
{
    SETTING_SEED,
    SETTING_PASSGENLEN,
    SETTING_PASSGENTABLE,
    SETTING_PASS,
    SETTING_AUTOLOCK,
    SETTING_SAVE,
    SETTING_RESET,
    SETTING_ENDENUM
};

const unsigned int INPUT_BUFFER_LEN = 128;

const unsigned int SYMBOL_TABLE_NUMBERS_LEN = 10;
const char SYMBOL_TABLE_NUMBERS[] =
{
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0'
};

const unsigned int SYMBOL_TABLE_BASIC_LEN = 62;
const char SYMBOL_TABLE_BASIC[] =
{
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
    'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
    'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'
};

const unsigned int SYMBOL_TABLE_ADVANCED_LEN = 93;
const char SYMBOL_TABLE_ADVANCED[] =
{
    '!', '"', '#', '$', '%', '&', '\'', '(', ')', '*', '+', ',', '-', '.', '/',
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', ':', ';', '<', '=', '>', '?',
    '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
    'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '[', '\\', ']', '^', '_',
    '`', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
    'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '|', '}'
};

enum SymbolTableName
{
    TABLENAME_BASIC,
    TABLENAME_ADVANCED,
    TABLENAME_NUMBERS,
    TABLENAME_ENDENUM
};

#endif