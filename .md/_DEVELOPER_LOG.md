<H2> Developer Log Contents </H2>

- [Tools](#tools)
- [TODO List](#todo-list)
___

[Return to README](../README.md)
___


<BR/>

# 1. Tools
 * Coding:
   - VS Code
 * Tutorials:
   - [Draw IO](https://draw.io/)

# 2. TODO List
<!-- File Loader -->
* [x] File Loader: 
  - Remove...
    + [x] blank strings  
    + [x] comment characters
<!-- Settings -->
* [x] Settings: `corewar.ini`
  - [x] Decide parameters:
    + Constant:
      * `core_size`
    + Configurable: 
      * `max_rounds`
      * `max_cycles`
      * `max_processes`
      * `max_warrior_len`
      * `min_separation`
  - [x] Load `.ini` file
<!-- Warriors -->
* [x] Warriors: `warriors/`
  - Create Warrior examples...
    + [x] Rock
    + [x] Paper
    + [x] Scissors
  - [x] Implement Warrior class & Instruction struct
  - [x] Load `.asm` files (contents)
<!-- Parser -->
* [x] Parser: Assembly code (`Redcode`)
  - [x] Clean (format and remove noise)
    + Format: `<label>_[code].<mod>_<admo>[A],<admo>[B]` ("`_`" = space)
  - [x] Parse **line** to Instruction object
  - [x] Parse file to Warrior object
<!-- OS -->
* [x] Operating System: `Core` using an `FDE` cycle (**Fetch**, **Decode**, **Execute**)
  <!-- Scheduler -->
  - [x] Process `Scheduler`: (`Fetch`)
    + [x] Processss: (`PCBs`)
      * [x] Add + kill 
      * [x] Queue for each warrior
    + [x] Round Robin class: (`RR`)
      * [x] Manage warrior queue rotation
      * [x] Automate indexing internaly (private members managed by function calls)
  <!-- MARS -->
  - [x] Memory Array Redcode `Simulator`: (`Decode`) 
    + [x] Implement **circular** `Random Access Memory` (`RAM`)
      * [x] Automate subscript operator index bounds safety (deduct/add size)
    + [x] Load warrior instruction objects into RAM
    + Decoding...
      * [x] Implement: `Control Unit` with `Register` structs
      * [x] Decode `RAM` index:
      * [x] Decode `<admo>`
      * [x] Decode `<mod>`
  <!-- Core -->
  - [x] Execution `Core`: (`Execute`)
    + [x] Utilises `Scheduler` + `Simulator`
    + [x] Functions for opcode types:
      * [x] `Read/Write`
      * [x] `Comparision`
      * [x] `Arithmetic`
      * [x] `Jump`
<!-- Corewar -->
* [x] Corewar: run a full game consisting of `i` rounds and `n` cycles
<!-- Build -->
* [x] `CMake` as a build system (cross platform)
<!-- Test -->
* [ ] `Test/`
  - [ ] Parser
  - [ ] Core
  - [ ] MARS
  - [ ] Scheduler
<!-- GUI -->
* [ ] GUI Implementation
  - [ ] Select and load Warriors
  - [ ] Display live simulation
  - [ ] Display Warrior statistics
  - [ ] Set simulation speed