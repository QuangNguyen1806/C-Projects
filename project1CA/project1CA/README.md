E20 Simulator 


State of the Work


The assignment is fully completed. The simulator correctly implements the E20 architecture as described in the E20 manual. It supports all instructions, including:


- Arithmetic operations (`add`, `sub`, `addi`)


- Logical operations (`and`, `or`)


- Control flow (`j`, `jal`, `jr`, `jeq`)


- Memory operations (`lw`, `sw`)


- Comparison operations (`slt`, `slti`)





The simulator initializes memory, registers, and the program counter correctly, and it handles edge cases such as:


- Writing to register `$0` (no effect).


- Sign extension for immediate values.


- Memory address wrapping (only the lower 13 bits are used for memory access).


- Halt condition detection (infinite loop when the program counter wraps around).





Known Issues


There are no known bugs or missing features. The simulator has been tested with the provided example programs and additional test cases to ensure correctness.






Resources Used


1. **E20 Manual**: The primary resource for understanding the E20 architecture, instruction set, and memory layout.


2. **Starter Code**: The provided starter code (`sim-starter.cpp`) was used as a foundation for the project.


3. **C++ Standard Library**: Utilized for file I/O, string manipulation, and bitwise operations.


4. **Online Documentation**: Referenced C++ documentation for standard library functions and best practices.



Design Decisions





1. **Memory Representation**


- Memory is represented as an array of `unsigned` integers (`unsigned memory[MEM_SIZE]`), where each element corresponds to a 16-bit memory cell.


- Memory addresses are wrapped using `% MEM_SIZE` to ensure they stay within bounds.



2. **Register Representation**


- Registers are stored in an array (`unsigned registers[NUM_REGS]`), with `$0` hardcoded to always return `0`.


- Writing to `$0` is explicitly ignored to enforce its immutability.



3. **Instruction Decoding**


- The `extract_bits` function is used to decode instruction fields (opcode, registers, immediate values).


- Sign extension is handled by the `sign_extend` function for immediate values in instructions like `addi`, `lw`, `sw`, and `jeq`.


4. **Simulation Loop**


- The main loop fetches, decodes, and executes instructions sequentially.


- The program counter (`pc`) is updated after each instruction, and the halt condition is checked by comparing the next program counter to the current one.



5. **Output Format**


- The `print_state` function outputs the final state of the simulator in the required format, including:


 - Program counter value.


 - Register values (in decimal).


 - First 128 memory locations (in hexadecimal).



Strengths


- **Modularity**: The code is divided into helper functions (`load_machine_code`, `print_state`, `extract_bits`, `sign_extend`) for better readability and maintainability.


- **Error Handling**: The program validates input files and memory addresses, providing clear error messages for invalid inputs.


- **Edge Case Handling**: The simulator correctly handles edge cases like sign extension, register `$0`, and memory address wrapping.



 Weaknesses


- **Performance**: The simulator is not optimized for performance. For very large programs, the sequential execution loop could be slow.


- **Self-Modifying Code**: The simulator does not handle self-modifying code (e.g., writing to memory locations containing instructions) in a pipelined manner, which could lead to unexpected behavior.
