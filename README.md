# C Chip-8 Emulator
This is a simple weekend project to emulate the basic specs of the Chip-8 fantasy computer.
Most of it is working, audio only works in Windows and the Super Chip-48 Instructions have not been implemented.

## Usage
Compile and drop a rom into the executable or run it from CLI passing the ROM as argument.
It'll try to run "[WorkingDir]/ROMS/PONG.ch8" if no rom is dropped/passed to it.

## Input
The keyboard is emulated in the WASD zone like this:

|   |   |   |   |    |   |   |   |   |
|---|---|---|---|----|---|---|---|---|
| 1 | 2 | 3 | C | -> | 1 | 2 | 3 | 4 |
| 4 | 5 | 6 | D | -> | Q | W | E | R |
| 7 | 8 | 9 | E | -> | A | S | D | F |
| A | 0 | B | F | -> | Z | X | C | V |

Esc closes the program.

## Debugging
if you enable the Debugging flag in Globals.h it'll print every instruction it executes and will dump the RAM, ROM and Screen buffer when closed.
