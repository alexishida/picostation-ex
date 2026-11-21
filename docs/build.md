# Build and releases

## Prepared local environment

In this Linux checkout, the tools are installed in `env/` and `build/toolchain/`, both of which are Git-ignored directories. From the project root, activate the environment before building:

```sh
source env/picostation-build.sh
cmake -S . -B build/local-picostation_pico2 -DPICOSTATION_VARIANT=picostation_pico2
cmake --build build/local-picostation_pico2 --parallel 4
```

The activation script is local to this checkout and contains its absolute path. The local `picotool` was built without USB support: it produces UF2 files but does not detect or flash devices. On another checkout or machine, set up the tools as described below.

The standard release build uses CMake 3.25 or newer, Pico SDK 2.1.1, the GNU Arm `arm-none-eabi` toolchain, the GNU MIPS toolchain (for example, `mipsel-linux-gnu`), Python 3.10 or newer, and `mkpsxiso`. CMake builds the PlayStation menu and embeds the resulting image in the firmware.

With the tools on `PATH`, run this from the project root:

```sh
git clone --branch 2.1.1 --recurse-submodules https://github.com/raspberrypi/pico-sdk.git third_party/pico-sdk
python3 -m pip install -r third_party/ps1-bare-metal/tools/requirements.txt
cmake -S . -B build/picostation_pico2 -DPICOSTATION_VARIANT=picostation_pico2
cmake --build build/picostation_pico2
```

The example produces `build/picostation_pico2/picostation_pico2.uf2`. For another build, change the variant and use its own build directory. If you already have the SDK, skip the clone and pass `-DPICO_SDK_PATH=/path/to/pico-sdk` to CMake or set the `PICO_SDK_PATH` environment variable. The `-DPICO_SDK_FETCH_FROM_GIT=ON` option lets the SDK be downloaded during configuration. If the MIPS toolchain is not on `PATH`, pass `-DPICOSTATION_MIPS_TOOLCHAIN_PATH=/path/to/toolchain`.

### PU-8 and PU-18 package

With [`ps1-packer`](https://github.com/pcsx-redux/pcsx-redux/tree/main/tools/ps1-packer) from PCSX-Redux on `PATH`, run:

```sh
cmake -P scripts/build-release.cmake
```

The command gathers only the five final files into `release/`, at the project root. Intermediate files stay in `build/`. If `release/` contains other files, move them before running the command; the script does not delete them. The four ODE profiles use **the RP2040 and the standard pinout**. The `_OC` suffix selects a higher fixed clock for the Mechacon. In the profiles below, GP3 supplies that clock, and installation must follow the schematic for the corresponding PU-8 or PU-18 board, with the original clock isolated when GP3 is wired to the Mechacon.

| File | Build | Clock on GP3 |
| --- | --- | --- |
| `picostationex_PU8.uf2` | PU-8 | 3.988 MHz |
| `picostationex_PU8_OC.uf2` | PU-8 with overclock | 6.457 MHz |
| `picostationex_PU18.uf2` | PU-18 | 4.238 MHz |
| `picostationex_PU18_OC.uf2` | PU-18 with overclock | 7.533 MHz |
| `pico_booty.uf2` | Second RP2040 connected to the PlayStation expansion port | — |

The release script builds the menu from the current checkout in `build/picostationex_PU8/menu` and embeds the resulting disc image in all four ODE firmwares. Pico Booty packages the executable from that same menu build for the expansion port. Pico Booty's code comes from [Nicolas Noble's fork](https://github.com/nicolasnoble/pico_booty/tree/ea96b7fcd4196d666bc90340799bbacba50d3b31) of John Baumann's project.

To build a single ODE firmware, use the matching name from the table, without `.uf2`, in `-DPICOSTATION_VARIANT`.

The menu is built from source by default. To reuse an already-built menu disc image, configure with `-DPICOSTATION_BUILD_MENU=OFF -DPICOSTATION_MENU_IMAGE=/path/to/picostation-menu.bin`. This mode requires CMake 3.24.1 or newer and does not need the menu build tools. The file must exist, and CMake checks its path during configuration. Relative paths are resolved from the project root.


See also [architecture](architecture.md) and [menu usage](menu.md).
