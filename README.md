# keyboard-passgen-arduino
(Password generator p1k)

Keyboard password generator with promicro16 (or phisical password manager).

Is connecting like a keyboard via USB, no special drives needed. On the top panel is located 10 numeric buttons, 4 for switching presets, 1 for open and iterate settings, 1 for generate random password and 2 buttons for confirm and cancel actions. One more button is located under the bottom which inmediatly reset the device (erase all data and restore default settings).

Before using a preset, you need to specify your unique numeric seed. That seed will be used for generate passwords. And then, writing the same numeric code (in the main page of the preset) you will receive the same password. And yes, you can specify a table of symbols (Numeric, Basic, Advanced) and password length (up to 512).

Are available 4 presets for save them in EEPROM with encryption and 1 preset (preset #0) stored in the RAM memory. Every preset can be secured with a numeric master-password. Every data will be encrypted with that master-password. A hash of that master-password is present in the EEPROM and will be used for identify user and log in.

Your important data (seed, length of generated password, name of the table of symbols) are always encrypted if master-password is enabled. So the only way to extract information (for example your seed) from the device is to find correct master-password via brute force using the hash. So do not set short and obvious passwords!
However, your final passwords are not generated with the seed only, cracker needs to know codes for combine them with that seed. So, my recomendations for setup a good secured preset are:
1) Use strong master-password (8+, up to 128)
2) Use a large codes (like 4-6 numbers or more, up to 128)
3) Enable "Autolock" (exit from preset after 30sec of inactivity and after avery password generation)

![preview](preview.jpg)


Examples:

seed (base 10) | code | password numeric | password basic | password advanced
--- | --- | --- | --- | ---
111 | 0 | 049549805917 | 4bwiBkdjgmic | bF<ljng/jQl(
123 | 0 | 662921249621 | vz8WVg6xU2L1 | ;?f`@,E=^"U_
123 | 00 | 662921249621 | vz8WVg6xU2L1 | ;?f`@,E=^"U_
123 | 1 | 217609576606 | VMyRT7w9xRRv | _V][]'zH=z[Z
123 | 10 | 972759792100 | qe85yo1ELGdt | UhGc>r@NUogw
123 | 01 | 398377023851 |  Ygzs112r7F71 | b,}v_!"8Fne_