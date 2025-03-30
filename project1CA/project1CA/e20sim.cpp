/*
 * E20 Machine Simulator
 * Simulates the execution of E20 machine code.
 */

#include <cstddef>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <regex>
#include <cstdlib>

using namespace std;

// ================ Constants ================
size_t const static NUM_REGS = 8;        // Number of registers
size_t const static MEM_SIZE = 1 << 13;  // 8K memory (8192 locations)
size_t const static REG_SIZE = 1 << 16;  // 16-bit registers

// ================ Helper Functions ================

/**
 * Loads E20 machine code into memory.
 * @param f Input file stream containing machine code.
 * @param mem Memory array to populate with instructions.
 */
void load_machine_code(ifstream &f, unsigned mem[]) {
    regex machine_code_re("^ram\\[(\\d+)\\] = 16'b(\\d+);.*$");
    size_t expected_addr = 0;
    string line;

    while (getline(f, line)) {
        smatch sm;
        if (!regex_match(line, sm, machine_code_re)) {
            cerr << "Invalid line format: " << line << endl;
            exit(1);
        }

        size_t addr = stoi(sm[1], nullptr, 10);
        unsigned instr = stoi(sm[2], nullptr, 2);

        // Validate address sequence and bounds
        if (addr != expected_addr) {
            cerr << "Memory addresses out of sequence: " << addr << endl;
            exit(1);
        }
        if (addr >= MEM_SIZE) {
            cerr << "Program too large for memory: " << addr << endl;
            exit(1);
        }

        mem[addr] = instr;
        expected_addr++;
    }
}

/**
 * Prints the final state of the simulator.
 * @param pc Final program counter value.
 * @param regs Final register values.
 * @param memory Final memory contents.
 * @param mem_qty Number of memory locations to display.
 */
void print_state(unsigned pc, unsigned regs[], unsigned memory[], size_t mem_qty) {
    cout << setfill(' ');
    cout << "Final state:" << endl;
    cout << "\tpc=" << setw(5) << pc << endl;

    // Print register values
    for (size_t r = 0; r < NUM_REGS; r++)
        cout << "\t$" << r << "=" << setw(5) << regs[r] << endl;

    // Print memory in hexadecimal format
    cout << hex << setfill('0');
    for (size_t i = 0; i < mem_qty; i++) {
        cout << setw(4) << memory[i] << " ";
        if ((i + 1) % 8 == 0) cout << endl;  // New line every 8 values
    }
    cout << dec << setfill(' ') << endl;
}

/**
 * Extracts a bit field from an instruction.
 * @param instruction 16-bit instruction word.
 * @param start Starting bit position (0 = LSB).
 * @param end Ending bit position (inclusive).
 * @return Extracted bit field as an unsigned integer.
 */
unsigned extract_bits(unsigned instruction, int start, int end) {
    unsigned mask = (1 << (end - start + 1)) - 1;
    return (instruction >> start) & mask;
}

/**
 * Sign-extends a 7-bit value to 16 bits.
 * @param value 7-bit input value.
 * @return 16-bit sign-extended value.
 */
unsigned sign_extend(unsigned value) {
    if (value & 0b1000000) {
        value = value | 0xFF80;
    }
    return value & 0xFFFF;
}

// ================ Main Function ================

int main(int argc, char *argv[]) {
    // ----------------- Command Line Handling -----------------
    char *filename = nullptr;
    bool do_help = false;
    bool arg_error = false;

    for (int i = 1; i < argc; i++) {
        string arg(argv[i]);
        if (arg.rfind("-", 0) == 0) {
            if (arg == "-h" || arg == "--help")
                do_help = true;
            else
                arg_error = true;
        } else {
            if (filename == nullptr)
                filename = argv[i];
            else
                arg_error = true;
        }
    }

    // Display help or error message if necessary
    if (arg_error || do_help || filename == nullptr) {
        cerr << "Usage: " << argv[0] << " [-h] <machine_code_file.bin>" << endl;
        cerr << "Simulates the execution of E20 machine code." << endl;
        cerr << "Options:" << endl;
        cerr << "  -h, --help  Show this help message and exit." << endl;
        return 1;
    }

    // ----------------- Initialization -----------------
    ifstream f(filename);
    if (!f.is_open()) {
        cerr << "Error: Cannot open file " << filename << endl;
        return 1;
    }

    unsigned memory[MEM_SIZE] = {0};    // Initialize memory
    unsigned registers[NUM_REGS] = {0}; // Initialize registers
    unsigned pc = 0;                    // Program counter
    bool halted = false;                // Halt flag

    // Load machine code into memory
    load_machine_code(f, memory);

    // Define opcodes for clarity
    const unsigned ADD = 0b000;
    const unsigned ADDI = 0b001;
    const unsigned J = 0b010;
    const unsigned JAL = 0b011;
    const unsigned LW = 0b100;
    const unsigned SW = 0b101;
    const unsigned JEQ = 0b110;
    const unsigned SLTI = 0b111;

    // ----------------- Simulation Loop -----------------
    while (!halted) {
        unsigned current_pc = pc;
        unsigned instruction = memory[current_pc % MEM_SIZE];
        unsigned opcode = extract_bits(instruction, 13, 15);
        unsigned next_pc = current_pc + 1;

        unsigned regA, regB, regDst, imm, last_four_bit;

        // Decode and execute instruction based on opcode
        switch (opcode) {
            case ADD: {
                regA = extract_bits(instruction, 10, 12);
                regB = extract_bits(instruction, 7, 9);
                regDst = extract_bits(instruction, 4, 6);
                last_four_bit = extract_bits(instruction, 0, 3);

                if (regDst != 0) {
                    switch (last_four_bit) {
                        case 0b0000:  // ADD
                            registers[regDst] = (registers[regA] + registers[regB]) & 0xFFFF;
                            break;
                        case 0b0001:  // SUB
                            registers[regDst] = (registers[regA] - registers[regB]) & 0xFFFF;
                            break;
                        case 0b0010:  // OR
                            registers[regDst] = (registers[regA] | registers[regB]) & 0xFFFF;
                            break;
                        case 0b0011:  // AND
                            registers[regDst] = (registers[regA] & registers[regB]) & 0xFFFF;
                            break;
                        case 0b0100:  // SLT (Set Less Than)
                            registers[regDst] = (registers[regA] < registers[regB]) ? 1 : 0;
                            break;
                    }
                }
                if (last_four_bit == 0b1000) {  // JR (Jump Register)
                    next_pc = registers[regA] & 0x1FFF;
                }
                break;
            }

            case ADDI: {
                regA = extract_bits(instruction, 10, 12);
                regDst = extract_bits(instruction, 7, 9);
                imm = extract_bits(instruction, 0, 6);
                imm = sign_extend(imm);

                if (regDst != 0) {
                    registers[regDst] = (registers[regA] + imm) & 0xFFFF;
                }
                break;
            }

            case J: {
                imm = extract_bits(instruction, 0, 12);
                next_pc = imm;
                break;
            }

            case JAL: {
                imm = extract_bits(instruction, 0, 12);
                registers[7] = (current_pc + 1) & 0xFFFF;  // Save return address
                next_pc = imm;
                break;
            }

            case LW: {
                regA = extract_bits(instruction, 10, 12);
                regDst = extract_bits(instruction, 7, 9);
                imm = extract_bits(instruction, 0, 6);
                imm = sign_extend(imm);

                unsigned addr = (registers[regA] + imm) & 0x1FFF;  // Effective address
                if (regDst != 0) {
                    registers[regDst] = memory[addr];
                }
                break;
            }

            case SW: {
                regA = extract_bits(instruction, 10, 12);
                regB = extract_bits(instruction, 7, 9);
                imm = extract_bits(instruction, 0, 6);
                imm = sign_extend(imm);

                unsigned addr = (registers[regA] + imm) & 0x1FFF;  // Effective address
                memory[addr] = registers[regB];
                break;
            }

            case JEQ: {
                regA = extract_bits(instruction, 10, 12);
                regB = extract_bits(instruction, 7, 9);
                imm = extract_bits(instruction, 0, 6);
                imm = sign_extend(imm);

                if (registers[regA] == registers[regB]) {
                    next_pc = current_pc + 1 + imm;
                }
                break;
            }

            case SLTI: {
                regA = extract_bits(instruction, 10, 12);
                regDst = extract_bits(instruction, 7, 9);
                imm = extract_bits(instruction, 0, 6);
                imm = sign_extend(imm);

                if (regDst != 0) {
                    registers[regDst] = (registers[regA] < imm) ? 1 : 0;
                }
                break;
            }

            default: {
                cerr << "Unknown opcode: " << opcode << " at pc=" << current_pc << endl;
                return EXIT_FAILURE;
            }
        }

        // Ensure $0 remains zero and check for halt condition
        registers[0] = 0;
        halted = (next_pc % MEM_SIZE) == current_pc;
        pc = next_pc;
    }

    // ----------------- Final Output -----------------
    print_state(pc, registers, memory, 128);
    return 0;
}