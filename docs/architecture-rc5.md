# rc5 architecture: available evidence

This document gathers what can be stated about PicoStation rc5 from the local artifacts and records, including a static reconstruction of the PU-18 firmware's main paths from the binary. It is not a complete source reconstruction nor a guaranteed description of every rc5 variant. For the current implementation, see [architecture and protocol](architecture.md).

## Artifacts and limits

- [`referencia/rc5-build/`](../referencia/rc5-build/) contains UF2s identified as PU-8, PU-18, their OC variants, and Pico Booty. The extracted file `picostation_PU18.uf2.bin` has SHA-256 `442e84237f2611e27c28257233dd1fc2c10cfd959ecfdf82ef207d1b0a26d28e`. The recorded firmware analysis refers to the rc5 binary, but the record does not associate every observation with a specific hash.
- [`binary/picostation-menu.bin`](../binary/picostation-menu.bin), SHA-256 `d10c138286ffbe4a78b36b5d9a750f41d9a110e58f6a7815db841df1891645e5`, is the image analyzed in [`referencia/menu-decompilado/README.md`](../referencia/menu-decompilado/README.md). The exact association of this image with each rc5 UF2 was not verified here.
- The menu analysis includes pseudocode and MIPS assembly. Pseudocode names and types are inferred; the presence of code does not by itself prove its execution in every scenario.
- Console results and earlier comparisons are in [validation.md](validation.md). They are historical records, not tests repeated for this document.

## Known components and flow

The system combines the device firmware attached to the console with a menu executed by the PlayStation. The test record reports that the original rc5 firmware opened the menu on a PU-18 without overclock, using the same card on which an early EX combination did not open. There is also a cross test with EX firmware and an rc5 menu image, which did not open the menu: this shows that the menu and the firmware can be treated as distinct artifacts in the experiment, but does not by itself determine the cause of the failure.

The analyzed menu image contains the executable `SCES_313.37`, in PS-X EXE format for little-endian MIPS R3000. Its header indicates load address `0x80010000`, entry point `0x80018D84`, and 151,552 bytes of code, data, and padding loaded. The local analysis identifies the main loop at `0x80011A28`, footer drawing at `0x80011420`, and Game ID lookup by volume date at `0x800149C4`. These addresses are specific to the analyzed executable and can be checked in the assembly and pseudocode linked above.

The visual comparison record reports that the rc5 menu left the message “Please Wait Loading...” on a black background during fast boot. This is an interface observation, not a complete description of the internal boot sequence.

## Sector handling observed in earlier analysis

The investigation documented in [validation.md](validation.md#change-validation-on-2026-11-20) records that disassembly of the rc5 firmware showed `loaderImage` substitution limited to sectors 0–4, conditional regeneration for Mode 2 Form 1, and preservation of zeroed or pattern EDC in the Form 2 case examined. In the reported comparison, the original firmware served sectors equivalent to those of the examined implementation for the investigated cases. This conclusion is limited to the compared paths and images; it does not prove full equivalence of processing or timing.

The same record compares the sector send-timestamp position with a later EX refactor. The hypothesis investigated is that its position affects the Q subchannel scheduling relative to the data. Signal captures and console confirmation remain pending per the record; the hypothesis must not be treated as a conclusive measurement of rc5 timing.

## What still needs reconstruction

The exact division of responsibilities between cores, the internal states, the complete menu-firmware protocol, the GPIO mapping, the card reading path, and the rc5 DMA and PIO details were not established by the materials examined for this document. The current EX code can guide investigation questions, but does not prove the rc5 version had the same architecture. Closing these gaps would require identifying each UF2 and its embedded menu by hash, analyzing the startup and reading paths in the ARM binary, and correlating commands and responses with the MIPS executable; console measurements would be needed to confirm timing.

The REA integration was not connected in the session that produced these notes; its diagnostics pointed to a missing or divergent record. This document therefore uses the existing local analysis and records and does not present a new decompilation or execution observation as its own result.

## Close reading of the menu executable

The observations below come from static inspection of [`menu_decompilado.c`](../referencia/menu-decompilado/menu_decompilado.c) and refer to the image with the hash reported above. The `FUN_...` names are addresses assigned by the tool, not recovered original names. The analysis did not execute the code nor confirm the identity of the image inside each firmware.

### Initialization, frames, and list

The routine `FUN_80011a28` initializes subsystems and enters a loop with no return. It toggles a boolean flag every frame, chooses between two command memory regions, and writes graphics parameters alternately using offsets `0` and `0x140`. This is consistent with two command sets and alternating display areas. The routine draws the list and the selection and, in another state, the waiting message. The assembly is needed to resolve ambiguities of types and overlapping globals in the pseudocode.

The routine `FUN_800143c4` requests blocks of `0x2000` and `0x101000` bytes. `FUN_80014424` writes entries with a stride of `0x101` bytes: a leading byte, followed by the name and a terminator; it also initializes a 16-bit index. `FUN_80014508` accesses an entry indirectly through that index, and `FUN_80014574` returns the underlying index. In the main loop, the first byte changes the icon and the behavior associated with the entry. This organization allows keeping a presentation order separate from the data; it does not by itself demonstrate how the firmware enumerates files.

While navigating, states `1`, `2`, and `3` call `FUN_800116e4` with different operations. In state `3`, the code stores the selected index in a table before going down and resets the selection; in state `2`, it decreases the level and restores the saved selection. Reading this as directory navigation is supported by this flow, although the exact meaning of each transmitted value must also be confirmed in the firmware.

### Disc selection and identification

In states `4` and `5`, the loop gets the internal index of the selection and calls `FUN_80010d64(5, index)`. After waits and CD initialization calls, the flow tries to recognize the disc. Operation `5` is evidence of a specific request between the components; it is not safe to assume it has the same semantics or representation as the current EX protocol.

`FUN_80017044` requests a read of sector `0x10`, compares 11 bytes against `PLAYSTATION`, and on a match extracts an address from the read structure and requests another sector. `FUN_80017114` walks records of the loaded region up to 0x800 bytes, advances by each record's size, and compares names. `FUN_80016b90` uses that search and, when it finds an address, requests a read of `0x800` bytes. On the boot path, the searched name is `SYSTEM.CNF;1`. The search visible in this routine is therefore limited to that 2048-byte region; it must not be extrapolated to larger directories without analyzing other paths.

The code starts with `cdrom:PS.EXE;1` as the fallback path. If the search and extraction in `SYSTEM.CNF` succeed, it uses the extracted value. `FUN_8001487c` looks for a key, skips equals signs and spaces, and copies the following token; the key literal must be checked in the image data before assigning it a precise name.

When the path matches one of the verified spellings of `PSX.EXE`, the code reads sector `0x10` again, copies 16 bytes from an offset of the buffer, and calls `FUN_800149c4`; per the existing analysis, that routine consults the volume date to determine a Game ID. It then builds a string prefixed `SLPS_` or `SCPS_`, depending on a global variable. Other paths follow different formatting rules. Finally it calls `FUN_8001614c` and, depending on the state, `FUN_800178c8` or `FUN_800178a4`. The full effects of these final calls and the meaning of the two states require additional tracing; the appearance of a call does not prove successful execution of the game.

### Interrupts and observation limits

`FUN_8001726c` reads the state of registers mapped in the `0xbf801800` range, collects response bytes, and dispatches to one of five routines according to the low bits of the state. This indicates CD event handling on the PlayStation side; it does not establish the corresponding implementation on the RP2040. The read routine and the other waits must be analyzed together with the interrupts to establish their completion and failure conditions.

In the additional investigation behind this document, the REA CLI was consulted to inspect the ARM binary, but the firmware inspection provider reported that it requires an absolute path to Binwalk, which was not configured. Consequently, the new observations above come from the existing local pseudocode, not from a new REA analysis or hardware execution. An ARM firmware reconstruction remains pending.
