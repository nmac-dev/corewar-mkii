<H2>Corewar Tutorial Contents</H2>

- [1. Basics](#1-basics)
- [2. Strategy](#2-strategy)
___

[Return to README](../README.md)
___


<BR/><BR/>

# 1. Basics 
**Prerequisite**: [Redcode Tutorial](#Redcode-Tutorial)  
  
Corewar has the following three main `concepts` you will need to know about...  

Concept | Description 
:-: | :--
`Scheduler` | Gets the current player, as warriors take turns executing instruction 
`MARS`      | A `circular` memory simulator where each address stores a `Redcode` instructions
`Core`      | Executes `Redcode` instruction, changing the state of the **Scheduler** and **MARS**

In the order presented they represent a `Fetch` / `Decode` / `Execute` (`FDE`) cycle.  
You don't need to know much about `FDE`, but an interesting point as computers use this cycle to run programs too.    
  
When creating a Warrior you need to keep the `FDE` cycle in mind, as it will play a role in your programs performance...  
**Lets break it down!**  
For each cycle the `Core` talks to the `Scheduler` and `MARS`.
 1. It asks the `Scheduler` to `Fetch` the next warrior in play and thier EXE address for `MARS`
 2. Next `MARS` is asked to `Decode` the warriors EXE's `<admo>` and `<mod>`, getting the **SRC** & **DEST** addresses 
 3. Finally, the `Core` will `Execute` the instruction.
    * Examples...
      - `jmp` will tell the `Scheduler` to change the warriors next EXE
      - `mov.i` updates the DEST ADR in `MARS` with a copy of the SRC

Can you see now how the three `FDE` steps affect your warriors?  

Think about how the core works:  
**"Your"** program is fighting **"my"** program in the same memory simulator (`MARS`)  
 * This means any other program in the simulator can access "**your**" program.  
   - So **"MY"** program, could place a `dat #0, #0` in the middle of **"YOUR"** program, killing its process in the next few cycles

<BR/><BR/>

# 2. Strategy
...