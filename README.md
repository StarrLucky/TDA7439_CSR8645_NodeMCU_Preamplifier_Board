TDA7439 + NodeMCU + CSR8645 DIY Amplifier project (ATOM IDE + PlatformIO)

Schematics (TDA7439 and logic) available here: [easyEDA](https://easyeda.com/fxndstrs/tda7439)

  **TO DO:**
- [x] Access points connection and mdnsserver
- [x] Server handlers for controlling preamp (gain, volume, input settings)
- [x] Power Button handlers
- [x] Amplifier Power On Relay control
- [ ] Sensor Readings
- [ ] Saving settings to EEPROM
- [ ] CSR8645 Control


**Project works with PlatformIO sdk espressif8266@1.8.0**
only. Interrupts doesn't work with the newest version of the SDK.
