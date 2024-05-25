#include "instr.h"
#include <stdint.h>

Instr *decode(uint32_t code);

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
const uint32_t ARITHMETIC_OPI = 0x2;
const uint32_t WIDE_MOVE_OPI = 0x5;
const uint32_t SH_OFFSET = 22;
const uint32_t DPI_ARIT_RN_START = 0;
const uint32_t DPI_ARIT_RN_SIZE = 5;
const uint32_t IMM12_START = DPI_ARIT_RN_START + DPI_ARIT_RN_SIZE;
const uint32_t IMM12_SIZE = 12;
const uint32_t IMM16_START = 0;
const uint32_t IMM16_SIZE = 16;
const uint32_t HW_START = IMM12_START + IMM16_SIZE;
const uint32_t HW_SIZE = 2;

// consts for DP (Register)
const uint32_t DPR_RN_START = RD_START + RD_SIZE;
const uint32_t DPR_RN_SIZE = 5;
const uint32_t DPR_OPERAND_START = DPR_RN_START + DPR_RN_SIZE;
const uint32_t DPR_OPERAND_SIZE = 6;
const uint32_t RM_START = DPR_OPERAND_START + DPR_OPERAND_SIZE;
const uint32_t RM_SIZE = 5;
const uint32_t OPR_START = RM_START + RM_SIZE;
const uint32_t OPR_SIZE = 4;
const uint32_t M_START = 28;
const uint32_t M_SIZE = 1;
