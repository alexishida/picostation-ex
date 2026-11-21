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
- For disc publication changes, test confirmed and direct mounts, failed mounts, lid-driven swaps between images of different sizes, and return to the menu. Exercise console reset during a swap and verify that playback and subcode resume for the active image.

## Interrupt and pregap review

The SDK initializes IRQ priorities to `0x80`, and the project does not override the priorities of the GPIO and PIO0 handlers. On the core that initializes them, equal-priority handlers do not preempt each other. That protects each `m_latched` update from interruption by the other handler, but it does not guarantee that the PIO handler consumes the final command byte before the XLAT handler runs. Verify the relative signals and pending IRQs on PU-8 and PU-18, including during a disc-publication pause, before treating command ordering as confirmed.

The subcode alarm is scheduled for `elapsed + 3333` microseconds from the scheduling call, where `elapsed` is the time since the reported sector transfer began. Ignoring scheduling overhead, its target is therefore approximately `3333 + 2 * elapsed` microseconds after that transfer began. For example, an elapsed time of 500 microseconds gives a target approximately 4333 microseconds after transfer start. If the intended target is 3333 microseconds after transfer start, the remaining delay would be `max(0, 3333 - elapsed)`. A hardware capture is needed to establish the intended timing and actual latency.

For the first track's two-second pregap, the current relative-sector calculation gives index 0 at `-150` as `00:02:00`, index 0 at `-1` as `00:00:01`, and index 1 at `0` as `00:00:00`. The absolute values in `sectorToMSF` preserve this countdown. The negative-sector branch sets the minute field to zero, so a pregap of one minute or more cannot be represented correctly by that path. Validate unusual multi-track pregaps against a known subchannel capture before changing this behavior.

## Change validation on 2026-11-20

The four PU-8 and PU-18 release profiles, Pico Booty, and the four basic Pico and Pico 2 variants compiled locally. The PU8 profile also compiled with SD boot diagnostics enabled. A clean release build and an incremental menu-asset rebuild produced the expected five UF2 files and incorporated the same menu in all four ODE profiles.

Console validation of these changes is pending for PU-8 and PU-18, including the disc swaps, reset, navigation and latency scenarios above. Subcode signal captures and comparison with reference hardware are also pending.

## Diagnostics

| Symptom | Initial check |
| --- | --- |
| CMake cannot find the SDK | Set `PICO_SDK_PATH` or install the SDK in `third_party/pico-sdk`; see [build](build.md). |
| The menu does not build | Check CMake 3.25+, the MIPS toolchain, Python, and `mkpsxiso`; for the package, also check `ps1-packer`. |
| Configuration rejects the prebuilt menu | Check that `PICOSTATION_MENU_IMAGE` points to an existing file, not a directory. |
| Card or listing unavailable | Check the card, connection, and format; try Retry after reinserting it. |
| Game does not open | Check paths in the CUE, the BIN files, and 2352-byte raw sectors. Validation does not detect every form of truncation. |
| Cover or colors do not appear | Check the `.cov` name and location, the cover size and format, and `config.ini` at the root. |

Recorded build and console results appear in the sections below. Specifications can be checked with `openspec validate --all --strict` when the tool is installed.

## Lessons from PU-18 boot

On a PU-18 without overclock, firmware 1.0.0 showed the PlayStation logo and returned to the Memory Card browser. On the same build and with the same card, the original rc5 opened the menu. Reducing the menu executable to the old size did not fix the symptom, and the new firmware with the rc5 menu image also returned to the browser.

A stack overflow was identified on core 1. In the build examined, the entry to `I2S::start` used 5,872 bytes with only 2,048 bytes reserved. Swapping `DiscImage` objects created a large temporary on the stack. Transferring the prepared image now avoids that temporary. When changing large objects, check the function frame in the disassembly and the stack reservation in the linker map, including the calls it makes. The configured stack does not by itself guarantee that the whole call chain fits.

Setting up the DMA address before waiting for the LRCK edge and triggering immediately after the edge were also part of later firmwares. The v2 diagnostic log recorded the transfer of the executable's sectors and a `GET_CFG` request, but wrote the report to the SD card before responding to the command. The menu stayed on `Please Wait Loading...`. In v3, the report was deferred, the EX menu was built with the firmware, and the user confirmed that the menu opened and a game loaded on the PU-18. These tests identify a working combination but do not isolate each change's contribution to boot.

To investigate similar regressions, compare against a known working firmware on the same build. Check the stack, LRCK synchronization, and the read path, and build the menu and firmware from compatible sources. A DMA trigger counter shows what the Pico tried to send. Menu execution and game loading must be confirmed on the console. Avoid extensive SD writes while core 1 needs to respond to the PlayStation.

### Menu font DMA and boot freeze

A later menu revision froze on the black PlayStation logo screen on PU-18. Its boot trace recorded attempts to send every executable sector, no LRCK timeout and no `GET_CFG` request. A comparison build using the same Pico firmware and the menu sources from tag `v1.0.0` opened successfully. DMA counters record attempts, so the trace alone could not establish whether the PlayStation received or executed the complete menu.

The current menu enlarged the indexed font to 96 × 114 pixels. In VRAM, its 4-bit texture is 24 × 114 16-bit pixels, or 1,368 32-bit DMA words. The GPU upload routine divided the transfer into 16-word blocks and rounded the block count down, sending 1,360 words. The GPU was configured to receive eight more words and could wait indefinitely during menu initialization. The revised routine chooses a block size that divides the full word count, sending 171 blocks of 8 words for this texture.

After this correction, the user confirmed that the current menu opened and a game ran on PU-18. A host check exercised GPU DMA block counts across dimensions from 1 × 1 to 256 × 256. Future texture changes should check that DMA block size multiplied by block count equals `ceil(width × height / 2)` 32-bit words, using the dimensions passed to the VRAM upload. Verify boot on the affected console because host checks do not reproduce GPU timing.

For covers with wrong bands or colors, test indexed and RGB555 files, including switching between games and between cover and disc images. Check that the CD DMA and pause command complete before the read is consumed, the texture and palette update in VRAM, and the GPU cache. The fixes for these points were built and tested with cover data on the computer, but release display still needs console confirmation.

Opening covers now reads the 16 reserved sectors in a single `ReadN`, instead of one sector per part, and the debounce wait dropped from 40 to 12 frames. That change reduces cover latency, but the batched read still needs console validation: check an RGB555 cover, an indexed cover, an entry without a cover, and fast selection changes during the read.

A host check with simulated file I/O exercised the CUE parser and image publication with AddressSanitizer and UndefinedBehaviorSanitizer. It covered CP850 and UTF-8 track names, a UTF-8 BOM, repeated lid swaps, preservation of boot options, return to the menu and a subsequent mount. All opened files were closed once in this sequence.

On PU-8 and PU-18, check a long reset during a software return from a subdirectory, repeat the disc swap with EDC and Bootsector Fix changed, and verify the file and folder icons beside names containing Å, Æ and Portuguese accents. Check that a matching cover loads and an accented BIN opens from its CUE.

### Sector transfer and ISO directory checks

Host tests with simulated CD-ROM interrupts and DMA registers covered completion of a multi-sector read, delayed DMA completion, timeout and controller error. A two-sector ISO root fixture put `SYSTEM.CNF;1` after the first sector’s padding and exercised repeat lookups, a shorter root extent and a failed sector read. A separate host check verified matching XA subheaders across menu, pregap and menu sectors.

Firmware state checks exercised a seek and a stop/restart between LRCK preparation and DMA commit, a pending reset, an option change with a populated cache, and lid events before and after mounting. On PU-8 and PU-18, measure the interval from LRCK to the DMA trigger, exercise seek and reset during playback, and verify that a lid event in the menu leaves the first selected disc mounted. Check boot-sector rereads after the boot pattern changes and Game ID lookup on a disc with a multi-sector root.
