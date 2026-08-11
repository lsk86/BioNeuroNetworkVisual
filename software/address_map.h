// Memory map for HPS <-> FPGA neuron PIO and VGA buffers.
// Chris Parker & Lucas Keith.

// video display
#define SDRAM_BASE 0xC0000000
#define SDRAM_END 0xC3FFFFFF
#define SDRAM_SPAN 0x04000000
// characters
#define FPGA_CHAR_BASE 0xC9000000
#define FPGA_CHAR_END 0xC9001FFF
#define FPGA_CHAR_SPAN 0x00002000
/* Cyclone V FPGA devices */
#define HW_REGS_BASE 0xff200000
#define HW_REGS_SPAN 0x00001000

// PIO HPS inputs
#define NRN_AB_OFFSET 0x00
#define NRN_AB56_OFFSET 0x290

#define NRN_C1_OFFSET 0x10
#define NRN_C2_OFFSET 0x20
#define NRN_C3_OFFSET 0x120
#define NRN_C4_OFFSET 0x130
#define NRN_C5_OFFSET 0x2A0
#define NRN_C6_OFFSET 0x2B0

#define NRN_D1_OFFSET 0x140
#define NRN_D2_OFFSET 0x150
#define NRN_D3_OFFSET 0x160
#define NRN_D4_OFFSET 0x170
#define NRN_D5_OFFSET 0x2c0
#define NRN_D6_OFFSET 0x2d0
#define NRN_I_OFFSET 0x30
#define NRN_DT_OFFSET 0x240

#define A1_MASK 0x0000000F
#define A1_SHIFT 0
#define A2_MASK 0x000000F0
#define A2_SHIFT 4
#define A3_MASK 0x00000F00
#define A3_SHIFT 8
#define A4_MASK 0x0000F000
#define A4_SHIFT 12
#define B1_MASK 0x000F0000
#define B1_SHIFT 16
#define B2_MASK 0x00F00000
#define B2_SHIFT 20
#define B3_MASK 0x0F000000
#define B3_SHIFT 24
#define B4_MASK 0xF0000000
#define B4_SHIFT 28

#define A5_MASK 0x0000000F
#define A5_SHIFT 0
#define A6_MASK 0x000000F0
#define A6_SHIFT 4
#define B5_MASK 0x00000F00
#define B5_SHIFT 8
#define B6_MASK 0x0000F000
#define B6_SHIFT 12

// `v` shifted by `s` and masked by `m`
#define SHIFTED(v, s, m) (((v) << (s)) & (m))
#define A1(v) (SHIFTED((v), A1_SHIFT, A1_MASK))
#define A2(v) (SHIFTED((v), A2_SHIFT, A2_MASK))
#define A3(v) (SHIFTED((v), A3_SHIFT, A3_MASK))
#define A4(v) (SHIFTED((v), A4_SHIFT, A4_MASK))
#define A5(v) (SHIFTED((v), A5_SHIFT, A5_MASK))
#define A6(v) (SHIFTED((v), A6_SHIFT, A6_MASK))

#define B1(v) (SHIFTED((v), B1_SHIFT, B1_MASK))
#define B2(v) (SHIFTED((v), B2_SHIFT, B2_MASK))
#define B3(v) (SHIFTED((v), B3_SHIFT, B3_MASK))
#define B4(v) (SHIFTED((v), B4_SHIFT, B4_MASK))
#define B5(v) (SHIFTED((v), B5_SHIFT, B5_MASK))
#define B6(v) (SHIFTED((v), B6_SHIFT, B6_MASK))

#define DT_MASK 0x000000FF

#define NRN_V1_INIT_OFFSET 0x180
#define NRN_V2_INIT_OFFSET 0x190
#define NRN_V3_INIT_OFFSET 0x1A0
#define NRN_V4_INIT_OFFSET 0x1B0
#define NRN_V5_INIT_OFFSET 0x250
#define NRN_V6_INIT_OFFSET 0x260

#define NRN_U1_INIT_OFFSET 0x1C0
#define NRN_U2_INIT_OFFSET 0x1D0
#define NRN_U3_INIT_OFFSET 0x1E0
#define NRN_U4_INIT_OFFSET 0x1F0
#define NRN_U5_INIT_OFFSET 0x270
#define NRN_U6_INIT_OFFSET 0x280

// PIO HPS outputs
#define NRN_V1_OFFSET 0x40
#define NRN_V2_OFFSET 0x50
#define NRN_V3_OFFSET 0x60
#define NRN_V4_OFFSET 0x70
#define NRN_V5_OFFSET 0x2F0
#define NRN_V6_OFFSET 0x300
#define NRN_U1_OFFSET 0x200
#define NRN_U2_OFFSET 0x210
#define NRN_U3_OFFSET 0x220
#define NRN_U4_OFFSET 0x230
#define NRN_U5_OFFSET 0x310
#define NRN_U6_OFFSET 0x320

// 8 & 9 reserved for control signals
#define NRN_DU1_OFFSET 0xA0
#define NRN_DV1_OFFSET 0xB0
#define NRN_DU2_OFFSET 0xC0
#define NRN_DV2_OFFSET 0xD0
#define NRN_DU3_OFFSET 0xE0
#define NRN_DV3_OFFSET 0xF0
#define NRN_DU4_OFFSET 0x100
#define NRN_DV4_OFFSET 0x110

#define NRN_DU5_OFFSET 0x330
#define NRN_DV5_OFFSET 0x340
#define NRN_DU6_OFFSET 0x350
#define NRN_DV6_OFFSET 0x360

// PIO HPS control signals
#define NRN_CLK_OFFSET 0x80
#define NRN_RST_OFFSET 0x90

//////////////////////////////////////////////////////////////
