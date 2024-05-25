#include <stdbool.h>
#include <stdint.h>

#ifndef _INSTR_H
#define _INSTR_H

// [ky723]:
// Every instruction is a GADT that's separated into categories, and every
// category itself is a GADT.
// Using this design allows for finer-tuned type checking and less unclear
// branching, though presumably results in higher memory use, but this is so
// cool I have to try it :)

// A DP Immediate instruction is one of:
//      - arithmetic
//      - wide move
typedef enum { ARITHMETIC, WIDE_MOVE } DpInstrType;
typedef enum { ADD = 0, ADDS = 1, SUB = 2, SUBS = 3 } DpArithmeticType;
typedef enum { MOVN = 0, MOVZ = 2, MOVK = 3 } DpWideMoveType;

typedef struct {
    DpInstrType type;
    union {
        struct {
            bool sf;
            bool sh;
            DpArithmeticType opc;
            uint32_t imm12;
            uint32_t rn;
            uint32_t rd;
        } arithmetic;
        struct {
            bool sf;
            uint32_t hw;
            uint32_t opc;
            uint32_t imm16;
            uint32_t rd;
        } wide_move;
    };
} DpImmed;

// [yw8123]: 

// A Single Data Transfer consists of 
//      - Load 
//      - Store

// The Transfer address is depended on the offset, 
// there are three kinds of offsets: 
//		- Register offset
// 		- Pre/Post-indexed offset
// 		- Unsigned offset

typedef enum {R_OFFSET, P_OFFSET, U_OFFSET} OffsetType; 

typedef struct {
	OffsetType type; 
	union {
		struct {
			uint64_t xm; // Another X-register
		} reg_offset;
		struct {
			bool I; // 1 is pre-indexed, 0 is post-indexed. 
			uint32_t simm9;
		} p_offset;
		struct {
			uint32_t imm12;
		} unisgned_offset;
	};
} Offset;

typedef struct {
	bool sf; // The size of the load, 0 is 32-bit, 1 is 64-bit
	bool L; // The type of data transfer, 0 is store, 1 is load. 
	bool U; // Unsigned offset flag
	uint64_t xn; // The base X-register
	uint64_t rt; // The target register
	Offset offset; 
} SdTrans;

// Load Literal is similar to data transfer. 

typedef struct {
	bool sf;
	uint32_t simm19;
	uint64_t rt; 
} LoadLiter; 


// A GADT Instr(uction) that is one of:
//      - DP Immediate
//      - DP Register
//      - Single Data Transfer
//      - Load Literal
//      - Branch
typedef enum {
    DP_IMMEDIATE,
    DP_REGISTER,
    SINGLE_DATA_TRANSFER,
    LOAD_LITERAL,
    BRANCH
} InstrType;

typedef struct {
    InstrType type;
    union {
        DpImmed dp_immed;
        struct {
            // TODO
        } dp_reg;
        SdTrans sing_data_transfer;
        LoadLiter load_literal;
        struct {
            // TODO
        } branch;
    };
} Instr;

// frees a Instr struct
void instr_free(Instr *instr);

#endif
