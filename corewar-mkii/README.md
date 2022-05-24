<H1> corewar-mk2 </H1>

`corewar-mk2` is a game based on the original `1984 corewar`.  

It features far richer insite into the internal operating system's data in `realtime` to help users learn `assembly code`.  
Users write assembly programs 'AKA' `warriors` in `Redcode` and load them into `corewar-mk2` to 'fight for control of the core'.  
When the game is running the user can see a visualisation of their program `realtime`, controls are given so the user can freeze the `simulation` and inspect the systems data and view each instruction in `memory`.  
The main goal of the project is to gamifiy learning assembly code.  
<BR>

___
- [1. Getting Started](#1-getting-started)
  - [1.1. How to Build](#11-how-to-build)
  - [1.2. CMake Notes](#12-cmake-notes)
  - [1.3. Requirements](#13-requirements)
  - [1.4. Vendors](#14-vendors)
- [2. How to Run](#2-how-to-run)
- [3. Playing the Game](#3-playing-the-game)
  - [3.1. Redcode Tutorial](#31-redcode-tutorial)
- [4. Screenshots](#4-screenshots)
  - [4.1. Live Memory](#41-live-memory)
  - [4.2. Control Panel](#42-control-panel)
___

<BR/>

# 1. Getting Started

## 1.1. How to Build
1. Open a terminal in the `build/` directory containing `.gitkeep`
2. Before running `CMake` please read [CMake Notes](#12-cmake-notes): 
    * Run `cmake -S ../ -B .` (or adjust the command to your needs)
3. Run `make` if no errors were reported. 
4. Process Complete: An executable `corewar` should be present in the build directory
   * 4.5. optionally `ctest` can be run to print the results of the corewar test suites

## 1.2. CMake Notes
CMake should be packaged with `Make` and `CTest`.  
When running CMake there a two consideration to be made:
1. The Operating System's **`C++ compiler`**
   * Examples...
     - Windows, using `MinGW`, requires appending `-G "MinGW Makefiles"` to the end of the CMake command
     - Linux, using `GCC`, doesn't require extra arguments (typically)
2. Where you want to **build** the program:
   * A standard CMake command is formulated as: `cmake -S <src_dir> -B <build_dir>`
   * The `<src_dir>` **must** be the directory where the **root** `CMakeLists.txt` resides
   * The `<build_dir>` is the target directory where all the generated `Makefile` project files are placed
     - Example: using `cmake -S ../ -B .` from the `build/` directory will generate the `Makefile` project in `build/`

## 1.3. Requirements
 * Compiler that supports `C++17`
 * `CMake`
 * `Make`
 * `CTest` (optional)

## 1.4. Vendors
 * Found under [VENDORS.md](vendor/VENDORS.md) in `vendor/` 

<BR/>

# 2. How to Run  
**After**  the [How to Build](#11-how-to-build) steps are complete, several elements should be present in the `build/` directory:
1. `corewar-mk2` as the executable
2. `warriors/` containing example assembly programs `imp.asm`, `rock.asm`, `paper.asm`, and `scissors.asm`
3. `corewar.ini` a configuration file for the following match settings:
  
 Name             | Impact                                                                              
 :-:              | :--                                                                                 
**core_size**     | cannot be configured and is forced to `8192` for stability and to keep the GUI uniform
`max_rounds`      | max number of rounds before the game is concluded                                   
`max_cycles`      | max number of cycles before the round has been concluded                           
`max_processes`   | max number of processes a single warrior can create                                 
`max_warrior_len` | max instructions a warrior can consist of                                           
`min_separation`  | min distance between warriors at the start of a round                               

<BR/>

**Run the Game** by executing `corewar-mk2`, then select the desired warriors to use, click `load selected` and finally start a game!  
* Once familiar with the game, try:
    * Altering the games settings via the `corewar.ini` to see different outcomes
    * Write your own `warriors/` in assembly and have them compete
    
**Important!** `warriors/`, `corewar.ini`, and `.gui.ini` MUST be present with the executable

<BR/>

# 3. Playing the Game
**Warriors** are programs written in an `abstract assembly language` called `Redcode`  

The goal of the game is to have multiple warrior oppnents 'fight for control of the core'  

As the player you want to write a program to kill all other programs in the game who also occupy the same `memory` space.

## 3.1. Redcode Tutorial
Check out this [Redcode Tutorial](docs/REDCODE_TUTORIAL.md) to get started on writing a program.  
Once you feel ready, boot up `corewar-mk2` and load your program to fight one of the example programs.  
 * You can also have a friend write a program to fight your programs

Thanks and have fun learning to code in assembly!  

# 4. Screenshots
## 4.1. Live Memory
You can view the memory of the operating system in `realtime`, including the assembly instruction of each address.  

<IMG src="docs/.screenshots/live_memory.png"/>

## 4.2. Control Panel
Here you can control all the aspects of the game. Look for the `(Help ?)` tool tip to explain each section.
 * When ready, select warriors from the `warriors/` folder
   * Multiple of the same warrior can be selected
   * Minimum: `2`
   * Maximum: `9`
 * Then click `Load Selected` and play the game

<IMG src="docs/.screenshots/control_panel.png"/>
