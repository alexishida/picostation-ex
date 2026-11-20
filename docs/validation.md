# Validation and troubleshooting

## Build checks

Use one build directory per variant and check that the corresponding UF2 is produced. For changes to shared code, the project rules require building all four base variants. Changes to pinout, timing, SD reads, or the menu protocol also require validation on a PlayStation with the affected board and variant. Record the console model, board revision, variant, card, image used, and result.

Release qualification may cover the four PU-8 and PU-18 profiles and the Pico Booty package described in the [build guide](build.md). The presence of OpenSpec specifications does not show that the tests were run.

## Scenarios to test on the console

- Browsing with an empty folder, long names, a paginated listing, and returning to the parent directory.
- A missing or removed card, reinsertion, and a new attempt with Retry.
- Valid Mode 1 and Mode 2 CUE/BIN, an audio-only disc, a missing BIN, a truncated file, and a read failure.
- Fast and regular boot of recognized games, including Game ID identification.
- Indexed and RGB555 covers, entries without a cover, and `config.ini` present or absent.
- For each relevant profile, reset, signals, and clock for the corresponding PU-8 or PU-18 build.

## Diagnostics

| Symptom | Initial check |
| --- | --- |
| CMake cannot find the SDK | Set `PICO_SDK_PATH` or install the SDK in `third_party/pico-sdk`; see [build](build.md). |
| The menu does not build | Check CMake 3.25+, the MIPS toolchain, Python, and `mkpsxiso`; for the package, also check `ps1-packer`. |
| Configuration rejects the prebuilt menu | Check that `PICOSTATION_MENU_IMAGE` points to an existing file, not a directory. |
| Card or listing unavailable | Check the card, connection, and format; try Retry after reinserting it. |
| Game does not open | Check paths in the CUE, the BIN files, and 2352-byte raw sectors. Validation does not detect every form of truncation. |
| Cover or colors do not appear | Check the `.cov` name and location, the cover size and format, and `config.ini` at the root. |

No build or physical test results are recorded by this documentation. Specifications can be checked with `openspec validate --all --strict` when the tool is installed.

## Lessons from PU-18 boot

On a PU-18 without overclock, firmware 1.0.0 showed the PlayStation logo and returned to the Memory Card browser. On the same build and with the same card, the original rc5 opened the menu. Reducing the menu executable to the old size did not fix the symptom, and the new firmware with the rc5 menu image also returned to the browser.

A stack overflow was identified on core 1. In the build examined, the entry to `I2S::start` used 5,872 bytes with only 2,048 bytes reserved. Swapping `DiscImage` objects created a large temporary on the stack. Transferring the prepared image now avoids that temporary. When changing large objects, check the function frame in the disassembly and the stack reservation in the linker map, including the calls it makes. The configured stack does not by itself guarantee that the whole call chain fits.

Setting up the DMA address before waiting for the LRCK edge and triggering immediately after the edge were also part of later firmwares. The v2 diagnostic log recorded the transfer of the executable's sectors and a `GET_CFG` request, but wrote the report to the SD card before responding to the command. The menu stayed on `Please Wait Loading...`. In v3, the report was deferred, the EX menu was built with the firmware, and the user confirmed that the menu opened and a game loaded on the PU-18. These tests identify a working combination but do not isolate each change's contribution to boot.

To investigate similar regressions, compare against a known working firmware on the same build. Check the stack, LRCK synchronization, and the read path, and build the menu and firmware from compatible sources. A DMA trigger counter shows what the Pico tried to send. Menu execution and game loading must be confirmed on the console. Avoid extensive SD writes while core 1 needs to respond to the PlayStation.

For covers with wrong bands or colors, test indexed and RGB555 files, including switching between games and between cover and disc images. Check that the CD DMA and pause command complete before the read is consumed, the texture and palette update in VRAM, and the GPU cache. The fixes for these points were built and tested with cover data on the computer, but release display still needs console confirmation.

Opening covers now reads the 16 reserved sectors in a single `ReadN`, instead of one sector per part, and the debounce wait dropped from 40 to 12 frames. That change reduces cover latency, but the batched read still needs console validation: check an RGB555 cover, an indexed cover, an entry without a cover, and fast selection changes during the read.
