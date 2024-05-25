#include "decode.h"
#include "stddef.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

// the first >> removes all bits before and the & removes all bits after
#define bit_slice(start, size) ((code >> start) & ((1 << (size + 1)) - 1))
#define nth_bit_set(bits, n) (bits & (1 << (n)))

// Decodes a DP (Immediate) instruction
extern DpImmed *decode_dpi(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);

Instr *decode(uint32_t code) {
    Instr *result = malloc(sizeof(Instr));
    if (!nth_bit_set(code, OP0_OFFSET + 1) &&
        !nth_bit_set(code, OP0_OFFSET + 2)) {
        // DP (Immediate)
        result->type = DP_IMMEDIATE;
        uint32_t rd = bit_slice(RD_START, RD_SIZE);
        uint32_t operand = bit_slice(DPI_OPERAND_START, DPI_OPERAND_SIZE);
        uint32_t opi = bit_slice(OPI_START, OPI_SIZE);
        uint32_t opc = bit_slice(OPC_START, OPC_SIZE);
        uint32_t sf = bit_slice(SF_START, SF_SIZE);
        DpImmed *dpi = decode_dpi(rd, operand, opi, opc, sf);
        result->dp_immed = dpi;
    }
    return result;
}

DpImmed *decode_dpi(uint32_t rd, uint32_t operand, uint32_t opi, uint32_t opc,
                    uint32_t sf) {
    // TODO
}
