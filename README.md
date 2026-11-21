# PicoStation EX

Version: **1.0.1**

PicoStation EX is an optical drive emulator (ODE) for the original PlayStation, based on [megavolt85's PicoStation](https://github.com/megavolt85/PicoStation). The project was created to modernize the original PicoStation and fix bugs. The firmware runs on a Raspberry Pi Pico or Pico 2, reads disc images from an SD card, and shows a console menu for browsing and launching games.

## Compatibility

The project supports the PU-8 (SCPH-100x) and PU-18 (SCPH-55xx) boards. Check the revision printed on the motherboard before choosing a wiring diagram and firmware. Early and late PU-8 boards use different chipsets. [PlayStation chipset reference](https://psx-spx.consoledev.net/pinouts/) The four ODE release profiles use the RP2040 and the standard pinout. The `_OC` profiles use a higher clock for the Mechacon.

| Release file | Build |
| --- | --- |
| `picostationex_PU8.uf2` | PU-8 |
| `picostationex_PU8_OC.uf2` | PU-8 with overclock |
| `picostationex_PU18.uf2` | PU-18 |
| `picostationex_PU18_OC.uf2` | PU-18 with overclock |
| `pico_booty.uf2` | Second RP2040 on the expansion port |

The release profiles supply the Mechacon clock on GP3. Installation must follow the schematic for the corresponding PU-8 or PU-18 board, with the original clock isolated when GP3 is wired to the Mechacon. See [build and releases](docs/build.md) for clock details.

## Building the release package

With the tools described in the [build guide](docs/build.md) installed, run this from the project root:

```sh
cmake -P scripts/build-release.cmake
```

The five final files are written to `release/`; intermediate files stay in `build/`. If `release/` contains any other file, move it before building the package. The script preserves unexpected items and stops.

## Start here

1. Organize the `.cue` and `.bin` files on the SD card, which should be formatted as exFAT; images must use 2352-byte raw sectors.
2. Build the package and choose the firmware in `release/` that matches your board and build.
3. Use the [menu guide](docs/menu.md) to browse, launch discs, and set up covers and colors.

## Documentation

- [Documentation index](docs/README.md)
- [Build, variants, and release package](docs/build.md)
- [SD card and PlayStation menu](docs/menu.md)
- [Architecture and protocol](docs/architecture.md)
- [Validation and troubleshooting](docs/validation.md)
- [OpenSpec specifications](openspec/specs/)
- [Development rules](.ai-framework/RULES.md) and [menu design](.ai-framework/DESIGN.md)

## License and credits

PicoStation EX: Alex Ishida. PicoStation EX builds on megavolt85’s fork of [paulocode’s original PicoStation](https://github.com/paulocode/picostation). This project is distributed under the [Apache 2.0 license](LICENSE). The Pico Booty package uses code from [Nicolas Noble's fork](https://github.com/nicolasnoble/pico_booty/tree/ea96b7fcd4196d666bc90340799bbacba50d3b31) of John Baumann's project; other dependencies keep their own license files in `third_party/`.
