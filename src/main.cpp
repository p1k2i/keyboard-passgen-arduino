#include <Arduino.h>
#include <Keyboard.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include "consts.h"
#include "utils.h"
#include "preset.h"

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

Preset *currentPreset;
Preset *zeroPreset;
Preset *savedPreset;

byte *inputBuffer;
unsigned int inputBufferCursor = 0;

char *passgenBuffer;

Menu menu = MENU_PRESETWELLCOME;
Menu nextMenu = menu;
int btnPressedCode = 1023;
Btn btnPressed = BTN_UNKNOWN;
Btn btnPressed2 = BTN_UNKNOWN;

Setting settingSelected = SETTING_SEED;

bool needRefreshLcd = true;

byte *pass;
unsigned int passLen = 0;

unsigned long sleepTimer = 0;

// -------------------------------------

void clearPass()
{
	for( unsigned int i = 0; i < INPUT_BUFFER_LEN; i++)
	{
		pass[i] = 0;
	}
	passLen = 0;

#ifdef DEBUG
	Serial.println("pass cleared");
#endif
}

void resetInputBuffer()
{
	inputBufferCursor = 0;
	for (unsigned int i = 0; i < INPUT_BUFFER_LEN; i++)
	{
		inputBuffer[i] = 0;
	}
}

void appendInputBuffer(const byte val, const unsigned int maxLen = INPUT_BUFFER_LEN)
{
	if (inputBufferCursor == maxLen)
		//inputBufferCursor %= maxLen;
		return;
	inputBuffer[inputBufferCursor] = val;
	++inputBufferCursor;
}

unsigned long getULongFromInputBuffer()
{
	unsigned long res = 0;
	for (unsigned int i = 0; i < inputBufferCursor; i++)
	{
		unsigned long val = inputBuffer[i];
		for (unsigned int j = 0; j < inputBufferCursor - i - 1; j++)
		{
			val *= (unsigned long)10;
		}
		res += val;
#ifdef DEBUG
		Serial.print("inputBuffer[");
		Serial.print(i, 10);
		Serial.print("] = ");
		Serial.print(inputBuffer[i], 10);
		Serial.print(", res = (unsigned long) ");
		Serial.println(res, 10);
#endif
	}
	return res;
}

unsigned int getUIntFromInputBuffer()
{
	unsigned int res = 0;
	for (unsigned int i = 0; i < inputBufferCursor; i++)
	{
		unsigned int val = inputBuffer[i];
		for (unsigned int j = 0; j < inputBufferCursor - i - 1; j++)
		{
			val *= (unsigned int)10;
		}
		res += val;
#ifdef DEBUG
		Serial.print("inputBuffer[");
		Serial.print(i, 10);
		Serial.print("] = ");
		Serial.print(inputBuffer[i], 10);
		Serial.print(", res = (unsigned int) ");
		Serial.println(res, 10);
#endif
	}
	return res;
}

void setNextMenu(Menu nextMenu_)
{
	nextMenu = nextMenu_;
	needRefreshLcd = true;

	if (
		nextMenu == MENU_PRESETWELLCOME // clear insecured data after exit from preset
		// clear before input
		|| nextMenu == MENU_PRESETAUTH
		|| nextMenu == MENU_SETTINGCHANGE
		|| nextMenu == MENU_PASSGEN
		)
	{
		resetInputBuffer();
	}

	if (nextMenu == MENU_PRESETWELLCOME || nextMenu == MENU_PRESETAUTH) // clear insecured data after exit from preset
	{
		clearPass();
	}
}

void reset()
{
	lcd.clear();
	lcd.print("reset...");
	delay(150);

	resetInputBuffer();
	clearPass();

	savedPreset->LoadDefault();
	savedPreset->Save(E_PRESET1);
	savedPreset->Save(E_PRESET2);
	savedPreset->Save(E_PRESET3);
	savedPreset->Save(E_PRESET4);
	zeroPreset->LoadDefault();
	currentPreset = zeroPreset;

	setNextMenu(MENU_PRESETWELLCOME);
	needRefreshLcd = true;

	lcd.clear();
	lcd.print("reset done");
	delay(1000);
}

bool isBtnPressed(Btn btnId)
{
	return btnPressed == btnId;
}

Btn getBtnIdByCode(int btnCode)
{
	for (int i = 0; i < 18; i++)
	{
		if ((btnPressedCode + 6 > BTN_CODES[i]) && (btnPressedCode - 6 < BTN_CODES[i]))
		{
			return (Btn)i;
		}
	}
	return (Btn)-1;
}

int readKeypad()
{
	if (isBtnPressed(BTN_1))
	{
		return 1;
	}
	if (isBtnPressed(BTN_2))
	{
		return 2;
	}
	if (isBtnPressed(BTN_3))
	{
		return 3;
	}
	if (isBtnPressed(BTN_4))
	{
		return 4;
	}
	if (isBtnPressed(BTN_5))
	{
		return 5;
	}
	if (isBtnPressed(BTN_6))
	{
		return 6;
	}
	if (isBtnPressed(BTN_7))
	{
		return 7;
	}
	if (isBtnPressed(BTN_8))
	{
		return 8;
	}
	if (isBtnPressed(BTN_9))
	{
		return 9;
	}
	if (isBtnPressed(BTN_0))
	{
		return 0;
	}

	return -1;
}

void loadPreset(unsigned int presetAddr)
{
	savedPreset->Load(presetAddr);
	currentPreset = savedPreset;
	setNextMenu(MENU_PRESETWELLCOME);
}

void lcdPrintPass(unsigned int len, unsigned int screenSize = 16)
{
	if (len <= screenSize)
	{
		for (unsigned int i = 0; i < len; i++)
		{
			lcd.print("*");
		}
	}
	else

		if ((len/screenSize)%2 == 0)
	{
		for (unsigned int i = 0; i < len%screenSize; i++)
		{
			lcd.print("*");
		}
		for (unsigned int i = len%screenSize; i < screenSize; i++)
		{
			lcd.print("/");
		}
	}
	else

	{
		for (unsigned int i = 0; i < len%screenSize; i++)
		{
			lcd.print("/");
		}
		for (unsigned int i = len%16; i < screenSize; i++)
		{
			lcd.print("*");
		}
	}
}

void lcdPrintArray(byte *array, unsigned int len, unsigned int screenSize = 16)
{
	const unsigned int begin = len - (len % screenSize);

	if (len % screenSize == 0)
	{
		for (unsigned int i = len-screenSize; i < len; i++)
		{
			lcd.print(array[i], 10);
		}
		return;
	}
	else
	{
		for (unsigned int i = begin; i < len; i++)
		{
			lcd.print(array[i], 10);
		}
	}

	if (len < screenSize) return;

	for (unsigned int i = len-begin; i < screenSize; i++)
	{
		lcd.print("/");
	}
}

void lcdPrintInputBuffer()
{
	for (unsigned int i = max(((int)inputBufferCursor)-16, 0); i < inputBufferCursor; i++)
	{
		lcd.print(inputBuffer[i]);
	}
}

void updateLcd();
void sleepModeOn()
{
#ifdef DEBUG
	Serial.println("Sleep mode on");
#endif

	sleepTimer = __LONG_MAX__;

	if (menu == MENU_PRESETWELLCOME || menu == MENU_PRESETAUTH)
	{
		setNextMenu(MENU_PRESETWELLCOME);
	}
	else
	{
		if (currentPreset->IsAutoLockEnabled())
		{
			setNextMenu(MENU_PRESETWELLCOME);
		}
		else
		{
			setNextMenu(MENU_MAIN);
		}
	}

	needRefreshLcd = true;
}

bool isSleepMode()
{
	return sleepTimer == __LONG_MAX__;
}

void sleepModeOff()
{
#ifdef DEBUG
	Serial.println("Sleep mode off");
#endif
	sleepTimer = 0;
	lcd.backlight();
	needRefreshLcd = true;
}

// --------------------------------

void updateBtns()
{
	btnPressedCode = analogRead(PIN_KEYBOARD);
	btnPressed = getBtnIdByCode(btnPressedCode);
	if (btnPressed == BTN_UNKNOWN)
		btnPressed2 = BTN_UNKNOWN;
	if (btnPressed == btnPressed2)
		return;
	btnPressed2 = btnPressed;
	needRefreshLcd = true;

	if (isSleepMode() && isBtnPressed(BTN_OK))
	{
		sleepModeOff();
		return;
	}

	if (!isSleepMode()) sleepTimer = 0;

	if (menu == MENU_PRESETWELLCOME)
	{
		if (isBtnPressed(BTN_CANCEL))
		{
			if (isSleepMode())
			{
				sleepModeOff();
			}
			else
			{
				if (currentPreset->GetId() != 0)
				{
					clearPass();
					currentPreset = zeroPreset;
				}
				else

				{
					sleepModeOn();
				}
			}
			
			return;
		}

		sleepModeOff();

		if (currentPreset->IsPassEnabled())
		{
			if (isBtnPressed(BTN_OK))
			{
				setNextMenu(MENU_PRESETAUTH);
			}
		}
		else
		{
			if (isBtnPressed(BTN_OK))
			{
				setNextMenu(MENU_MAIN);
			}
		}
	}

	if (menu == MENU_SETTINGS)
	{
		// start editing setting
		if (isBtnPressed(BTN_OK))
		{
			setNextMenu(MENU_SETTINGCHANGE);
			return;
		}

		// back to main
		if (isBtnPressed(BTN_CANCEL))
		{
			setNextMenu(MENU_MAIN);
			return;
		}

		// next setting
		if (isBtnPressed(BTN_SETTINGS))
		{
			settingSelected = (Setting)((settingSelected + 1) % SETTING_ENDENUM);
			return;
		}
	}

	if (menu == MENU_MAIN)
	{
		if (isBtnPressed(BTN_CANCEL))
		{
			setNextMenu(MENU_PRESETWELLCOME);
		}
		else

			if (isBtnPressed(BTN_SETTINGS))
		{
			setNextMenu(MENU_SETTINGS);
		}
		else

			if (isBtnPressed(BTN_OK))
		{
			if (currentPreset->ExistsSeed(pass, passLen))
			{
				setNextMenu(MENU_PASSGEN);
			}
			else
			{
				setNextMenu(MENU_SETTINGCHANGE);
				settingSelected = SETTING_SEED;
			}
		}
		else

		{
			int btnNumber = readKeypad();
			if (btnNumber != -1)
			{
				if (currentPreset->ExistsSeed(pass, passLen))
				{
					setNextMenu(MENU_PASSGEN);
				}
				else
				{
					setNextMenu(MENU_SETTINGCHANGE);
					settingSelected = SETTING_SEED;
				}
				appendInputBuffer(btnNumber);
				return;
			}
		}
	}
	
	if (menu == MENU_PASSGEN)
	{
		if (isBtnPressed(BTN_CANCEL))
		{
			setNextMenu(MENU_MAIN);
			return;
		}

		if (isBtnPressed(BTN_OK))
		{
			currentPreset->GenPass(passgenBuffer, pass, passLen, inputBuffer, inputBufferCursor);
			Keyboard.print(passgenBuffer);
			setNextMenu(MENU_MAIN);
			if (currentPreset->IsAutoLockEnabled())
			{
				setNextMenu(MENU_PRESETWELLCOME);
			}
			return;
		}
		else

		{
			int btnNumber = readKeypad();
			if (btnNumber != -1)
			{
				appendInputBuffer(btnNumber);
				return;
			}
		}
	}

	if (menu == MENU_SETTINGCHANGE)
	{

		if (settingSelected == SETTING_SEED)
		{
			// save changes
			if (isBtnPressed(BTN_OK))
			{
				if (inputBufferCursor != 0)
				{
					currentPreset->SetSeed(getULongFromInputBuffer(), pass, passLen);
					setNextMenu(MENU_MAIN);
				}
			}
			else
			// enter seed
			{
				int btnNumber = readKeypad();
				if (btnNumber != -1)
				{
					appendInputBuffer(btnNumber, 9);
				}
			}
		}
		else

			if (settingSelected == SETTING_PASSGENLEN)
		{
			if (isBtnPressed(BTN_OK))
			{
				const unsigned int inputValue = getUIntFromInputBuffer();
				currentPreset->SetPassGenLen(inputValue==0 ? PASSGEN_DEFAULT_LEN : min(inputValue, PASSGEN_MAX_LEN), pass, passLen);
			}
			else
			{
				int btnNumber = readKeypad();
				if (btnNumber != -1)
				{
					appendInputBuffer(btnNumber);
				}
			}
		}
		else

			if (settingSelected == SETTING_PASSGENTABLE)
		{
			if (isBtnPressed(BTN_OK))
			{
				currentPreset->SetSymbolTableName((SymbolTableName)((currentPreset->GetSymbolTableName()+1) % TABLENAME_ENDENUM));
			}
		}
		else

			if (settingSelected == SETTING_PASS)
		{
			if (currentPreset->IsPassEnabled()){
				if (isBtnPressed(BTN_OK))
				{
					currentPreset->DisablePass(pass, passLen);
					clearPass();
				}
			}
			else
			{
				if (isBtnPressed(BTN_OK))
				{
					if (inputBufferCursor < 4) return; // too short pass
					passLen = inputBufferCursor;
					for (unsigned int i = 0; i < inputBufferCursor; i++){
						pass[i] = inputBuffer[i];
					}
				#ifdef DEBUG
					Serial.print("pass: ");
					SerialPrintArrBytes(pass, passLen);
					Serial.println();
				#endif
					currentPreset->SetPass(pass, passLen);
				}
				else
				{
					int btnNumber = readKeypad();
					if (btnNumber != -1)
					{
						appendInputBuffer(btnNumber);
					}
				}
			}
		}
		else

			if (settingSelected == SETTING_AUTOLOCK)
		{
			if (isBtnPressed(BTN_OK))
			{
				currentPreset->SetAutoLockEnabled(!currentPreset->IsAutoLockEnabled());
			}
		}
		else

			if (settingSelected == SETTING_SAVE)
		{
			if (isBtnPressed(BTN_OK))
			{
				if (inputBuffer[0] != 0)
				{
					int addr = E_PRESET1;
					if (inputBuffer[0] == 1)
						addr = E_PRESET1;
					else if (inputBuffer[0] == 2)
						addr = E_PRESET2;
					else if (inputBuffer[0] == 3)
						addr = E_PRESET3;
					else if (inputBuffer[0] == 4)
						addr = E_PRESET4;
					
					currentPreset->Save(addr);
				}
				else
				{
					zeroPreset->CopyFrom(currentPreset);
				}
				setNextMenu(MENU_SETTINGS);
			}
			else

				if (isBtnPressed(BTN_PRESET1) || isBtnPressed(BTN_1))
			{
				inputBuffer[0] = 1;
			}
			else

				if (isBtnPressed(BTN_PRESET2) || isBtnPressed(BTN_2))
			{
				inputBuffer[0] = 2;
			}
			else

				if (isBtnPressed(BTN_PRESET3) || isBtnPressed(BTN_3))
			{
				inputBuffer[0] = 3;
			}
			else

				if (isBtnPressed(BTN_PRESET4) || isBtnPressed(BTN_4))
			{
				inputBuffer[0] = 4;
			}
			else

				if (isBtnPressed(BTN_0))
			{
				inputBuffer[0] = 0;
			}
		}
		else

			if (settingSelected == SETTING_RESET)
		{
			if (isBtnPressed(BTN_OK))
			{
				currentPreset->LoadDefault();
				setNextMenu(MENU_PRESETWELLCOME);
			}
		}

		if (isBtnPressed(BTN_CANCEL) || isBtnPressed(BTN_OK))
		{
			setNextMenu(MENU_SETTINGS);
		}

		return; // dont check default btns here
	}

	if (menu == MENU_PRESETAUTH)
	{
		if (isBtnPressed(BTN_OK))
		{
			if (currentPreset->VerifyPass(inputBuffer, inputBufferCursor))
			{
				passLen = inputBufferCursor;
				for (unsigned int i = 0; i < inputBufferCursor; i++){
					pass[i] = inputBuffer[i];
				}
			#ifdef DEBUG
				Serial.print("pass: ");
				SerialPrintArrBytes(pass, passLen);
				Serial.println();
			#endif
				setNextMenu(MENU_MAIN);
			}
			else
			{
				resetInputBuffer();
			}
		}
		else

			if (isBtnPressed(BTN_CANCEL))
		{
			setNextMenu(MENU_PRESETWELLCOME);
		}
		else

		{
			int btnNumber = readKeypad();
			if (btnNumber != -1)
			{
				appendInputBuffer(btnNumber);
			}
		}
	}

	// default btns

	if (isBtnPressed(BTN_RANDOM))
	{
		unsigned long randSeed = analogRead(7)*analogRead(8)*analogRead(9);
		randSeed += random();
		randSeed *= millis();
		randomSeed(randSeed);

		genPass(passgenBuffer,
			(currentPreset->VerifyPass(pass, passLen) ? currentPreset->GetPassGenLen(pass, passLen) : 12),
			(currentPreset->VerifyPass(pass, passLen) ? currentPreset->GetSymbolTableName() : SymbolTableName::TABLENAME_BASIC),
			randSeed);
		Keyboard.print(passgenBuffer);
	}

	if (isBtnPressed(BTN_PRESET1))
	{
		loadPreset(E_PRESET1);
	}
	else

		if (isBtnPressed(BTN_PRESET2))
	{
		loadPreset(E_PRESET2);
	}
	else

		if (isBtnPressed(BTN_PRESET3))
	{
		loadPreset(E_PRESET3);
	}
	else

		if (isBtnPressed(BTN_PRESET4))
	{
		loadPreset(E_PRESET4);
	}
}

void updateLcd()
{
	needRefreshLcd = false;
	lcd.clear();

	if (menu == MENU_MAIN)
	{
		lcd.print("-main-");

		lcd.setCursor(14, 0);
		lcd.print("#");
		lcd.print(currentPreset->GetId(), 10);

		lcd.setCursor(0, 1);
		if (currentPreset->ExistsSeed(pass, passLen))
		{
			lcd.print("Enter code:");
		}
		else
		{
			lcd.print("Enter seed:");
		}
	}
	else

		if (menu == MENU_SETTINGS)
	{
		lcd.print("-settings-");

		lcd.setCursor(14, 0);
		lcd.print("#");
		lcd.print(currentPreset->GetId(), 10);

		lcd.setCursor(0, 1);

		if (settingSelected == SETTING_SEED)
		{
			if (currentPreset->ExistsSeed(pass, passLen))
			{
				lcd.print("SEED ");
				lcd.print(currentPreset->GetSeed(pass, passLen), 36);
			}
			else
			{
				lcd.print("SET SEED");
			}
		}
		else

			if (settingSelected == SETTING_PASSGENLEN)
		{
			lcd.print("LEN: ");
			lcd.print(currentPreset->GetPassGenLen(pass, passLen), 10);
		}
		else

			if (settingSelected == SETTING_PASSGENTABLE)
		{
			lcd.print("TABLE: ");
			if (currentPreset->GetSymbolTableName() == TABLENAME_BASIC) lcd.print("Basic");
			if (currentPreset->GetSymbolTableName() == TABLENAME_ADVANCED) lcd.print("Advanced");
			if (currentPreset->GetSymbolTableName() == TABLENAME_NUMBERS) lcd.print("Numeric");
		}
		else

			if (settingSelected == SETTING_PASS)
		{
			lcd.print("PASS: ");
			lcd.print(currentPreset->IsPassEnabled() ? "ON" : "OFF");
		}
		else

			if (settingSelected == SETTING_AUTOLOCK)
		{
			lcd.print("AUTOLOCK: ");
			lcd.print(currentPreset->IsAutoLockEnabled() ? "ON" : "OFF");
		}
		else

			if (settingSelected == SETTING_SAVE)
		{
			lcd.print("SAVE");
		}
		else

			if (settingSelected == SETTING_RESET)
		{
			lcd.print("RESET");
		}
	}
	else

		if (menu == MENU_PASSGEN)
	{
		lcd.print("Enter code:");
		lcd.setCursor(0, 1);
		lcdPrintInputBuffer();
	}
	else

		if (menu == MENU_SETTINGCHANGE)
	{
		if (settingSelected == SETTING_SEED)
		{
			lcd.print("Enter seed:");
			lcd.setCursor(0, 1);
			//lcdPrintPass(inputBufferCursor);
			lcdPrintArray(inputBuffer, inputBufferCursor);
		}
		else

			if (settingSelected == SETTING_PASSGENLEN)
		{
			lcd.print("Enter length:");
			lcd.setCursor(0, 1);
			lcdPrintInputBuffer();
		}
		else

			if (settingSelected == SETTING_PASSGENTABLE)
		{
			if (currentPreset->GetSymbolTableName() == TABLENAME_NUMBERS)
			{
				lcd.print("Set Basic?");
			}
			else
				if (currentPreset->GetSymbolTableName() == TABLENAME_BASIC)
			{
				lcd.print("Set Advanced?");
			}
			else
				if (currentPreset->GetSymbolTableName() == TABLENAME_ADVANCED)
			{
				lcd.print("Set Numeric?");
			}
			lcd.setCursor(0, 1);
			lcd.print(" OK / CANCEL");
		}
		else

			if (settingSelected == SETTING_PASS)
		{
			if (currentPreset->IsPassEnabled()){
				lcd.print("Disable pass?");
				lcd.setCursor(0, 1);
				lcd.print(" OK / CANCEL");
			}
			else
			{
				lcd.print("Enter new pass:");
				lcd.setCursor(0, 1);
				lcdPrintPass(inputBufferCursor);
			}
		}
		else

			if (settingSelected == SETTING_AUTOLOCK)
		{
			if (currentPreset->IsAutoLockEnabled())
			{
				lcd.print("Disable autolock?");
			}
			else
			{
				lcd.print("Enable autolock?");
			}
			lcd.setCursor(0, 1);
			lcd.print(" OK / CANCEL");
		}
		else

			if (settingSelected == SETTING_SAVE)
		{
			lcd.print("Choose preset:");
			lcd.setCursor(0, 1);
			lcd.print("preset #");
			lcd.print(inputBuffer[0], 10);
		}
		else

			if (settingSelected == SETTING_RESET)
		{
			lcd.print("Reset preset #");
			lcd.print(currentPreset->GetId(), 10);
			lcd.print("?");
			lcd.setCursor(0, 1);
			lcd.print(" OK / CANCEL");
		}
	}
	else

		if (menu == MENU_PRESETWELLCOME)
	{
		lcd.print("  -Preset #");
		lcd.print(currentPreset->GetId());
		lcd.print("-");
		lcd.setCursor(0, 1);
		if (currentPreset->IsPassEnabled())
		{
			lcd.print("pass on");
		}
		else
		{
			if (currentPreset->ExistsSeed(nullptr, 0))
			{
				lcd.print("pass off");
			}
			else
			{
				lcd.print("empty");
			}
		}
	}
	else

		if (menu == MENU_PRESETAUTH)
	{
		lcd.print("Enter pass:");
		lcd.setCursor(0, 1);
		lcdPrintPass(inputBufferCursor);
	}

	if (isSleepMode())
	{
		lcd.noBacklight();
	}
}

// --------------------------------

void setup()
{
	// initialize the LCD and wellcome message
	lcd.init();
	lcd.backlight();
	lcd.print("Password");
	lcd.setCursor(3, 1);
	lcd.print("generator p1k");
	delay(500);

	// initialize device
#ifdef DEBUG
	Serial.begin(9600);
#endif
	Keyboard.begin();
	pinMode(PIN_BTN_RESET, INPUT_PULLUP);
	zeroPreset = new Preset();
	savedPreset = new Preset();
	currentPreset = zeroPreset;
	inputBuffer = new byte[INPUT_BUFFER_LEN];
	pass = new byte[INPUT_BUFFER_LEN];
	passLen = 0;
	passgenBuffer = new char[PASSGEN_MAX_LEN];
	delay(100);

	// finish initialization

#ifdef DEBUG
	delay(3000); // wait for serial monitoring
	Serial.println("Testing hash:");
	byte testData1[4] = {'A', 'B', 'C', 'D'};
	byte digest[PASSHASH_LEN]{};
	computeHash(testData1, 4, digest, PASSHASH_LEN);
	//SerialPrintHexBytes(digest, PASSHASH_LEN);
	Serial.println();

	Serial.println("Testing encryption:");
	decryptLong(
		encryptLong(123l, testData1, 4),
		testData1, 4);
	Serial.println();
#endif
}

void loop()
{
	if (digitalRead(PIN_BTN_RESET) == LOW)
	{
		reset();
		return;
	}

	updateBtns();

	if (!isSleepMode() && ((++sleepTimer) == AUTOLOCK_DELAY_SEC*1000 / LOOP_DELAY_MS))
	{
		sleepModeOn();
	}

	if (nextMenu != menu)
	{
		menu = nextMenu;
	}

	if (needRefreshLcd)
	{
		updateLcd();
	}

	delay(LOOP_DELAY_MS);
}
