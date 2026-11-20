# SD card and menu

## Organizing games

The project README recommends an exFAT SD card. Organize discs as `.cue` and `.bin` files; the BIN names and paths must match the references in the CUE. Images must use 2352-byte raw sectors. The menu lists visible directories and `.cue` files without showing the extension; other files and hidden entries do not appear.

Before mounting, the firmware parses the CUE, opens the tracks, and checks that the files are non-empty, uncompressed, and a multiple of 2352 bytes. When the first track is data, it also checks the ISO9660 descriptor at sector 16, in Mode 1 or Mode 2. These checks do not guarantee the full integrity of a disc. PlayStation identification happens in the menu after mounting. Audio-only discs and discs without a PlayStation signature follow the console's normal boot.

## Controls and boot

| Control | Action |
| --- | --- |
| Up / down | Select an entry |
| L1 / R1 | Previous / next page |
| X | Open a folder, game options, or retry after an error |
| Circle | Close options or return to the previous folder |
| Select | Open credits while browsing |

In the game window, Fast Boot and Regular Boot start the disc; you can also toggle EDC correction and Bootsector Fix. Cheats are not available yet, and the clock is set by the firmware. If preparation fails, the menu shows an error and keeps the selection. If the card is unavailable, select **Retry** with X to request another listing and try to mount it again.

For recognized games, the menu looks up the boot executable in `SYSTEM.CNF` and, when it identifies the Game ID, sends it to both memory card ports. For a generic `PSX.EXE`, it uses the volume creation date in the table embedded in the menu.

## Error log on the SD card

In the four ODE release firmwares, the Pico creates or appends lines to `/erro.log` only when it detects a failure. Each line reports the time since boot in milliseconds, the operation, a numeric detail, and the number of grouped occurrences. Repeated errors from the same operation are written at most once every five seconds. An existing file is preserved between sessions.

The log covers SD mount failures, listing, game preparation or mounting, sector reads, cover or configuration access errors, and LRCK timeouts. A missing cover or a missing `config.ini` is normal. A failure while the SD card is unavailable can only be written if the card accepts writes again during the same session.

## Covers

Place a `.cov` file next to the entry using the same base name: `Games/Example.cue` uses `Games/Example.cov`; the `Games/RPG/` folder uses `Games/RPG.cov`. Accepted files are 8-bit indexed 128 × 128 pixel images preceded by a 512-byte palette, or 16-bit little-endian RGB555. Covers are optional, and missing ones do not prevent browsing.

## Colors

Create `config.ini` at the card root with the optional `background`, `frame`, and `filelist` keys, using hexadecimal RGB colors. The menu reads up to 2048 bytes; without the file, it uses the default colors.

```ini
background = 404040
frame = 606060
filelist = 303030
```

See [validation and troubleshooting](validation.md) for test scenarios and diagnostics.
