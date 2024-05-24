#include "decode.h"
#include "stddef.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define nth_bit_set(bits, n) (bits & (1 << (n)))

const int OP0_OFFSET = 25;

// consts for DP (Immediate)
const uint32_t RD_START = 0;
const uint32_t RD_SIZE = 5;
const uint32_t DPI_OPERAND_START = RD_START + RD_SIZE;
const uint32_t DPI_OPERAND_SIZE = 18;
const uint32_t OPI_START = DPI_OPERAND_START + DPI_OPERAND_SIZE;
const uint32_t OPI_SIZE = 3;
const uint32_t OPC_START = 29;
const uint32_t OPC_SIZE = 2;
const uint32_t SF_START = 31;
const uint32_t SF_SIZE = 1;

// the first >> removes all bits before and the & removes all bits after
#define bit_slice(start, size) ((code >> start) & ((1 << (size + 1)) - 1))

Instr *decode(uint32_t code) {
    Instr *result = malloc(sizeof(Instr));
    if (!nth_bit_set(code, OP0_OFFSET + 1) &&
        !nth_bit_set(code, OP0_OFFSET + 2)) {
        // DP (Immediate)
        result->type = DP_IMMEDIATE;
        DpImmed *dpi = malloc(sizeof(DpImmed));
        dpi->rd = bit_slice(RD_START, RD_SIZE);
        dpi->operand = bit_slice(DPI_OPERAND_START, DPI_OPERAND_SIZE);
        dpi->opi = bit_slice(OPI_START, OPI_SIZE);
        dpi->opc = bit_slice(OPC_START, OPC_SIZE);
        dpi->sf = bit_slice(SF_START, SF_SIZE);
        result->dp_immed = dpi;
    }
    return result;
}
