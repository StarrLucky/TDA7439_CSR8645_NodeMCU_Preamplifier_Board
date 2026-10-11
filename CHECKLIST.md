# Code and Schematic Review Checklist

Review of the firmware against `Schematic_TDA7439-CSR8645-NodeMCU-Preamplifier-Board.pdf`
(EasyEDA, rev 4.0, 2019-02-10) and `platformio.ini`.

**Scope and limitations.** The schematic PDF could only be read as its text layer —
component designators, values and net names. **Actual connectivity was not traced**,
and no board was powered up. Everything in the Critical / I2C / Analog sections below
is therefore a *thing to verify against the schematic and the board*, not a confirmed
defect. The Code defects section is different: those were read directly from the
sources and are confirmed.

Parts the schematic text layer revealed: `U2 TDA7439`, `U3 NODEMCU_V1_ESP12E`,
`U4 CSR8645`, `U8 ADUM1250ARZ` (isolated I2C), `U16 B0505S-2WR2` (isolated DC-DC),
`U13 PS2561L` (optocoupler), `U15 MC7805`, `U6 AMS1117-3.3`, `U1.1 AS324` (current
sense op-amp), `SW1 EVQ22705R` (button). Nets: `BUTTON`, `LED_PIN`, `STANDBY` /
`STANDBY_TO_AMP`, `BT_EN`, `AMP_POWER_Relay`, `CURR_SENSOR`, `TEMP_SENSOR`,
`AGND` / `DGND` / `GNDBT`.

---

## Critical — boot failures and resets

- [ ] **GPIO9 (SD3) drives the relay — prime suspect for issue #1 ("H/W Watchdog timer triggered").**
  `AMPPOWER_PIN 9` in `include/defines.h:11`, and `setup()` writes it at `src/main.cpp:59-60`,
  before WiFi comes up. GPIO9/GPIO10 belong to the flash chip and are only free in DIO/DOUT
  mode; touching them under QIO faults the chip, which is reported exactly as a hardware
  watchdog reset. `board_build.flash_mode` is **not set at all** in `platformio.ini`.
  → Confirm the actual flash mode of the build and pin it explicitly: `board_build.flash_mode = dio`.

- [ ] **Relay glitch at power-on.** GPIO9 is driven by the flash controller during boot, so the
  relay may chatter before `setup()` runs. → Check that the input of optocoupler U13 has a
  pull to the inactive state.

- [ ] **GPIO0 (button SW1) is a boot strap pin.** Must be HIGH at reset; if the button is held
  during power-up the module enters flash mode instead of running.
  → Check the pull-up value (R30/R31 near SW1) and what C36 (100n) does. If C36 sits across
  the switch it is a hardware debounce, which coexists fine with the 30 ms software debounce.

- [ ] **GPIO2 (`BT_EN` → CSR8645 `POWER_EN`) is a boot strap pin.** Must be HIGH at reset. If the
  `BT_EN` net is pulled low by the CSR input or a resistor, the ESP will not boot at all.
  → Check the pull-up, and confirm `POWER_EN` tolerates the 3.3 V the ESP drives (the module
  runs from `+3.3VBT`; verify it is not a 1.8 V input).

- [ ] **GPIO15 (`LED_PIN`, LED1 + R11 220R) is a boot strap pin.** Must be LOW at reset. If the
  LED is wired to +3.3 V through the 220R the pin is pulled up and the module will not start.
  → Confirm R15 (10k) acts as a pull-down on GPIO15.

---

## I2C chain

- [ ] **Which pins carry I2C.** `src/TDA7439.cpp:7` calls `Wire.begin(SDA, SCL)`, i.e. the core
  defaults — GPIO4 (D2) and GPIO5 (D1) for `nodemcuv2`. No I2C pins are declared in
  `include/defines.h` at all. → Verify against the schematic where `SDA1` / `SCL1` of U8
  actually go, then declare them explicitly instead of relying on the default.

- [ ] **Pull-ups on both sides of the ADuM1250.** The isolator does not provide them; separate
  resistors are needed on the NodeMCU side and on the TDA7439 side. → Check presence and values.

- [ ] **Bus speed.** The ADuM1250 is a 100 kHz part. `Wire.setClock()` is never called, so the
  bus runs at the 100 kHz default, which is correct — worth setting explicitly so nobody
  raises it to 400 kHz later.

- [ ] **Isolated-side supply VDD2.** `B0505S-2WR2` is an *unregulated* converter: its output
  rises well above 5 V at light load, while the ADuM1250 recommended maximum is 5.5 V.
  → Measure VDD2 under the real load and decide whether a minimum load or a regulator is needed.

- [ ] **Ground tie.** The schematic carries the note "Connection between AGND and DGND here or
  in the PSU". → Confirm the tie exists at exactly one point, not two.

---

## Analog and signal levels

- [ ] **Two sensors share one ADC.** `CURSENS_PIN` and `TEMPSENS_PIN` are both `17` (A0) in
  `include/defines.h:8-9`, while the schematic has two distinct nets, `CURR_SENSOR` and
  `TEMP_SENSOR`. The ESP8266 has a single ADC. → Check whether the board has a multiplexer;
  if not, both cannot be read at once and the (still unwritten) sensor code must account for it.

- [ ] **ADC input range.** Confirm the AS324 output stays inside 0-3.3 V (the NodeMCU module
  carries a divider on A0; a bare ESP-12E would be 0-1 V).

- [ ] **STANDBY level and polarity.** The schematic notes "2V In Standby Mode" while the ESP
  drives 3.3 V. → Check the divider (R12/R13/R14). The firmware assumes HIGH = standby:
  `setup()` writes LOW with the comment "Not in standby mode" (`src/main.cpp:77`) and
  `/ampstandby_on` writes HIGH (`src/espserv.cpp:25`). Confirm that matches the amplifier.

---

## Code defects (confirmed by reading the sources)

- [ ] **`src/perif.cpp` — no `break` in any `switch`.** In `EnableLed`, `BlinkLed` and
  `ChangeState`, `case 1:` falls through into `case 2:`. `EnableLed(1)` writes HIGH and
  immediately LOW, so the pin always ends up LOW and colour selection never works. Given
  `defines.h:7` ("Pin with 2 Light diodes in") and the way the code toggles the pin between
  OUTPUT and INPUT, this is a two-colour LED on a single pin: HIGH = one colour, LOW = the
  other, Hi-Z = off. The missing `break` kills the whole scheme.

- [ ] **`src/perif.cpp:19,21,61,63` — `digitalRead(LED_PIN == HIGH)`.** Misplaced parenthesis:
  `15 == 1` evaluates to `0`, so the code reads GPIO0 — the button pin — and decides what to
  do with the LED based on it.

- [ ] **`PERIF::ChangeState` is never defined.** `include/perif.h:14` declares a member function;
  `src/perif.cpp:56` defines a free function `ChangeState` without the `PERIF::` qualifier.
  Nothing calls it today, but the first `LED.ChangeState(...)` call will fail to link.

- [ ] **`include/wlan.h` is a dead file reusing the `WLAN_H` include guard** — the same guard as
  `include/wrlss.h`. It holds an old, incompatible `W_LAN` / `wlanstruct`. Nothing includes it
  now, but if it is ever included before `wrlss.h` the guard will swallow the real header.
  → Delete it.

- [ ] **`src/perif.cpp:15` — `for (int i = 1; i<times; i++)`** blinks one time fewer than asked.

- [ ] **`include/espserv.h` disagrees with the implementation.** The header documents
  `/standbyon` and `/standbyoff`, while `src/espserv.cpp` registers `/ampstandby_on` and
  `/ampstandby_off`; gain is documented as 0..48 against a 4-bit register.

- [ ] **README claims "Interrupts doesn't work with the newest version of the SDK".** That was
  almost certainly the missing `ICACHE_RAM_ATTR`, now added. → Retest on a current core; the
  pin to `espressif8266@1.8.0` may no longer be needed.

---

## Open findings from the earlier review

Not yet fixed — full detail in the review output.

- [ ] `src/espserv.cpp:48` — `BT_PIN` never gets `pinMode(..., OUTPUT)`, so `/bton` and `/btoff`
  only toggle the internal pull-up while reporting success.
- [ ] `src/espserv.cpp:12` — `preampcontrol` is a second `TDA7439` instance, independent of
  `preamp` in `src/main.cpp:36`; HTTP input changes are invisible to the button path.
- [ ] `src/espserv.cpp:95` — gain accepted as 1..48 and written raw to a 4-bit register; the
  valid 0 dB setting is rejected.
- [ ] `src/espserv.cpp:81` — `handleTdaArgs` can return without sending any response.
- [ ] `src/espserv.cpp:18` — `MDNS.update()` is never called from `loop()`, so `esp8266.local`
  stops resolving shortly after boot.

---

## Security

- [ ] **Live WiFi credentials are committed in `src/main.cpp:28-29`** and the repository is public.
  → Rotate the password and move the credentials out of the sources.

---

## Already fixed in this session

- `src/wrlss.cpp` — `||` replaced with `&&` in both retry loops (the hangs).
- `include/wrlss.h` — SSID storage changed from `char[5]` to `String[WLAN_MAX_APS]`.
- `src/wrlss.cpp` — `SetSSIDs` implemented; `Connect()` passes real credentials and
  bounds-checks the index before use.
- `src/TDA7439.cpp` — `lastVolume` now stores the user-level value, so `/unmute` no longer
  applies the `48 - x` inversion twice; initialized in the constructor.
- `WLanControl` no longer takes `W_LAN` by value, so reconnect state persists.
- `src/main.cpp` — button ISR reduced to edge timestamping, marked `ICACHE_RAM_ATTR`;
  all I2C / Serial / relay work moved into `loop()`. Interrupt changed from `RISING` to
  `CHANGE` so press duration is measured properly.

**None of this has been compiled** — neither `pio` nor `arduino-cli` is installed, and the
ESP8266 core is not present locally. A `pio run` is the first thing to do.
