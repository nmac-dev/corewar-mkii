<H1> corewar </H1>
Impementation for the gamification of learning assembly code by creating a version of corewar to assist in learning abstract assembly code.

___
- [1. Getting Started](#1-getting-started)
  - [1.1. How to Build](#11-how-to-build)
  - [1.2. CMake Notes](#12-cmake-notes)
  - [1.3. Requirements](#13-requirements)
- [2. How to Run](#2-how-to-run)
- [3. Playing the Game](#3-playing-the-game)
  - [3.1. Tutorials](#31-tutorials)
  - [3.2. Other Links](#32-other-links)
___

<BR/>

# 1. Getting Started

## 1.1. How to Build
1. Open a terminal in the `corewar/build/` directory containing `.gitkeep`
2. Before running `CMake` please read [CMake Notes](#CMake-Notes): 
    * Run `cmake -S ../ -B .` (or adjust the command to your needs)
3. Run `make` if no errors were reported. 
4. Process Complete: An executable `corewar` should be present in the build directory
   * 4.5. optionally `ctest` can be run to print the results of the corewar test suites

## 1.2. CMake Notes
When running CMake there a two consideration to be made:
1. The Operating System's **`C++ compiler`**
   * Examples...
     - Windows, using `MinGW`, requires appending `-G "MinGW Makefiles"` to the end of the CMake command
     - Linux, using `GCC`, doesn't require extra arguments (typically)
2. Where you want to **build** the program:
   * A standard CMake command is formulated as: `cmake -S <src_dir> -B <build_dir>`
   * The `<src_dir>` **must** be the directory where the **root** `CMakeLists.txt` resides
   * The `<build_dir>` is the target directory where all the generated `Makefile` project files are placed
     - Example: using `cmake -S ../ -B .` from the `corewar/build/` directory will generate the `Makefile` project in `corewar/build/`

## 1.3. Requirements
 * `C++17`
 * `CMake`

<BR/>

# 2. How to Run  
**After**  the [How to Build](#How-to-Build) steps are complete, several elements should be present in the `build/` directory:
1. `corewar` as the executable
2. `warriors/` containing three example assembly programs `rock.asm`, `paper.asm`, and `scissors.asm`
3. `corewar.ini` a configuration file for the following match settings:
  
 Name             | Impact                                                                              
 :-:              | :--                                                                                 
**core_size**     | cannot be configured and is forced to 8000 for stability and to keep the GUI uniform
`max_rounds`      | max number of rounds before the game is concluded                                   
`max_cycles`      |  max number of cycles before the round has been concluded                           
`max_processes`   | max number of processes a single warrior can create                                 
`max_warrior_len` | max instructions a warrior can consist of                                           
`min_separation`  | min distance between warriors at the start of a round                               
  
**Run the Game** by executing `corewar`, then select the desired warriors to use, and finally start a game!  
 * Once familiar with the game, try:
    * Altering the games settings via the `corewar.ini` to see different outcomes
    * Write your own `warriors/` in assembly and have them compete

<BR/>

# 3. Playing the Game
**Warriors** are programs written in an `abstract assembly language` called `Redcode`
The goal of the game is to have multiple warrior oppnents fight for control of the core.

## 3.1. Tutorials
 * [Redcode Tutorial](.md/_REDCODE_TUTORIAL.md)
 * [Corewar Tutorial](.md/_COREWAR_TUTORIAL.md)  

## 3.2. Other Links
 * [Developer Log](.md/_DEVELOPER_LOG.md)

