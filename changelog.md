# Changelog

## Unreleased

### Firmware and menu

- Coordinate disc image publication, sector limits, subcode and GFS across the two Pico cores, and protect sector transfers during seeks and resets.
- Keep a directory listing available for repeated sector reads and correct the listing terminator size.
- Fix menu startup with the expanded font by transferring the complete texture to GPU VRAM. Display the first menu frame before reading configuration.
- Support accented names in the menu and UTF-8 CUE track references when representable in CP850.
- Improve completion handling for CD reads and support ISO directories spanning multiple sectors.
- Start a fresh `erro.log` for each menu session and record firmware and menu errors. Release builds include SD boot diagnostics and disable USB logging.

### Build and validation

- Build the current menu once per release and reuse its image across the four ODE profiles and its executable for Pico Booty. Build steps run in parallel.
- Archive the firmware hardening OpenSpec change and document the menu boot investigation.
- The user confirmed menu startup and game loading on PU-18 after the font DMA fix. Validation of the latest logging changes, remaining board profiles, disc swaps, subcode captures and timing measurements remains pending.

## 1.0.0

PicoStation EX is an optical drive emulator (ODE) for the original PlayStation that loads CUE/BIN images from an SD card. Alex Ishida developed this version to modernize megavolt85's original PicoStation and fix problems in the firmware and the menu.

### CD drive

- Console reset requests an image swap from the core that reads the SD card. That core stops the transfer before closing the BIN when returning to the menu.
- The sector cache protects the buffer that DMA is still transmitting.
- A transient read failure allows the BIN to be reopened and the read retried. Persistent failures have a minimum 250 ms interval between reopenings.
- The end-of-disc calculation counts the last track's pause only once.
- The synthetic `PREGAP` of an audio track returns silence. An `INDEX 00` written in the BIN is still read from the file.
- The reverse seek after reaching the end of the disc uses the zone that matches the emulated laser position.
- Mode 2 Form 1 sector regeneration recomputes the ECC even when the EDC already checks out.

### Menu and SD card

- The menu shows version 1.0.0, the logo with EX in yellow, and credits to Alex Ishida and megavolt85's original project.
- Failures to open or read a folder show an error message in place of the previous or partial listing.
- The menu shows `No SD Card` as a state. Pressing X on `Retry` requests another listing and allows the card to be mounted after insertion.
- The menu keeps distinct CUE entries, even when the displayed names end in `.bin` and `.cue`.
- The firmware checks the CUE and the BINs before swapping the active image. Preparation requires 2352-byte raw sectors and checks the ISO9660 descriptor when the first track is data. If preparation fails, the menu keeps the selection for another attempt.
- Fast Boot and Regular Boot are available for recognized games. Audio-only discs and discs without a PlayStation signature follow the normal boot. The boot window lets you toggle EDC correction and Bootsector Fix.
- Game ID reading recognizes the entire `BOOT` key in `SYSTEM.CNF` and extracts the executable name from paths with subfolders.
- The menu accepts 128 × 128 `.cov` covers in RGB555 or with an indexed palette for games and folders. Colors can be customized through `config.ini` at the SD card root.
- Cover loading fetches all 16 reserved sectors in a single read with a shorter 12-frame debounce, and the CD DMA and pause completion are checked before the read is consumed.
- Accented file names display correctly: FatFs converts long names to CP850 and the menu font covers the accented Latin letters of that table, falling back to the invalid-character box for codes without a glyph.

### Build and validation

- The `cmake -P scripts/build-release.cmake` command generates the local package in `release/` with five UF2 files. Intermediate files stay in `build/`.
- The package contains `picostationex_PU8.uf2`, `picostationex_PU8_OC.uf2`, `picostationex_PU18.uf2`, `picostationex_PU18_OC.uf2`, and `pico_booty.uf2`. The four ODE profiles use the RP2040 and embed the menu built from source. The `_OC` profiles use a higher clock for the Mechacon. Pico Booty packages the menu for a second Pico on the expansion port. [Profiles and clocks](docs/build.md)
- The README, the guides in `docs/`, and the OpenSpec specifications document the build, the architecture, and the menu behavior.
- CMake accepts a Pico SDK installed outside the project through `PICO_SDK_PATH` and uses version 2.1.1 by default when the CMake download is requested.
- The five UF2 files in the local package were verified. The version, the logo, and the credits were confirmed in the menu embedded in the four ODE firmwares.
- Local tests covered CUE pauses, end of disc, reverse seek, SD read recovery, ECC regeneration, paginated listing, and Game ID, including the 100 cases from the legacy table.
- Validation on physical PlayStation hardware is pending for reset during reads, DMA transfer, recovery after an SD failure, and playback of multi-track discs.
