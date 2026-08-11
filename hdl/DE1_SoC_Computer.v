// DE1-SoC top: Bruce Land Computer System template + pyloric neuron network.
// Adapted by Chris Parker and Lucas Keith
// https://people.ece.cornell.edu/land/courses/ece5760/DDA/NeuronIndex.htm

module DE1_SoC_Computer (
    ////////////////////////////////////
    // FPGA Pins
    ////////////////////////////////////

    // Clock pins
    CLOCK_50,
    CLOCK2_50,
    CLOCK3_50,
    CLOCK4_50,

    // ADC
    ADC_CS_N,
    ADC_DIN,
    ADC_DOUT,
    ADC_SCLK,

    // Audio
    AUD_ADCDAT,
    AUD_ADCLRCK,
    AUD_BCLK,
    AUD_DACDAT,
    AUD_DACLRCK,
    AUD_XCK,

    // SDRAM
    DRAM_ADDR,
    DRAM_BA,
    DRAM_CAS_N,
    DRAM_CKE,
    DRAM_CLK,
    DRAM_CS_N,
    DRAM_DQ,
    DRAM_LDQM,
    DRAM_RAS_N,
    DRAM_UDQM,
    DRAM_WE_N,

    // I2C Bus for Configuration of the Audio and Video-In Chips
    FPGA_I2C_SCLK,
    FPGA_I2C_SDAT,

    // 40-Pin Headers
    GPIO_0,
    GPIO_1,

    // Seven Segment Displays
    HEX0,
    HEX1,
    HEX2,
    HEX3,
    HEX4,
    HEX5,

    // IR
    IRDA_RXD,
    IRDA_TXD,

    // Pushbuttons
    KEY,

    // LEDs
    LEDR,

    // PS2 Ports
    PS2_CLK,
    PS2_DAT,

    PS2_CLK2,
    PS2_DAT2,

    // Slider Switches
    SW,

    // Video-In
    TD_CLK27,
    TD_DATA,
    TD_HS,
    TD_RESET_N,
    TD_VS,

    // VGA
    VGA_B,
    VGA_BLANK_N,
    VGA_CLK,
    VGA_G,
    VGA_HS,
    VGA_R,
    VGA_SYNC_N,
    VGA_VS,

    ////////////////////////////////////
    // HPS Pins
    ////////////////////////////////////

    // DDR3 SDRAM
    HPS_DDR3_ADDR,
    HPS_DDR3_BA,
    HPS_DDR3_CAS_N,
    HPS_DDR3_CKE,
    HPS_DDR3_CK_N,
    HPS_DDR3_CK_P,
    HPS_DDR3_CS_N,
    HPS_DDR3_DM,
    HPS_DDR3_DQ,
    HPS_DDR3_DQS_N,
    HPS_DDR3_DQS_P,
    HPS_DDR3_ODT,
    HPS_DDR3_RAS_N,
    HPS_DDR3_RESET_N,
    HPS_DDR3_RZQ,
    HPS_DDR3_WE_N,

    // Ethernet
    HPS_ENET_GTX_CLK,
    HPS_ENET_INT_N,
    HPS_ENET_MDC,
    HPS_ENET_MDIO,
    HPS_ENET_RX_CLK,
    HPS_ENET_RX_DATA,
    HPS_ENET_RX_DV,
    HPS_ENET_TX_DATA,
    HPS_ENET_TX_EN,

    // Flash
    HPS_FLASH_DATA,
    HPS_FLASH_DCLK,
    HPS_FLASH_NCSO,

    // Accelerometer
    HPS_GSENSOR_INT,

    // General Purpose I/O
    HPS_GPIO,

    // I2C
    HPS_I2C_CONTROL,
    HPS_I2C1_SCLK,
    HPS_I2C1_SDAT,
    HPS_I2C2_SCLK,
    HPS_I2C2_SDAT,

    // Pushbutton
    HPS_KEY,

    // LED
    HPS_LED,

    // SD Card
    HPS_SD_CLK,
    HPS_SD_CMD,
    HPS_SD_DATA,

    // SPI
    HPS_SPIM_CLK,
    HPS_SPIM_MISO,
    HPS_SPIM_MOSI,
    HPS_SPIM_SS,

    // UART
    HPS_UART_RX,
    HPS_UART_TX,

    // USB
    HPS_CONV_USB_N,
    HPS_USB_CLKOUT,
    HPS_USB_DATA,
    HPS_USB_DIR,
    HPS_USB_NXT,
    HPS_USB_STP
);

  //=======================================================
  //  PARAMETER declarations
  //=======================================================


  //=======================================================
  //  PORT declarations
  //=======================================================

  ////////////////////////////////////
  // FPGA Pins
  ////////////////////////////////////

  // Clock pins
  input CLOCK_50;
  input CLOCK2_50;
  input CLOCK3_50;
  input CLOCK4_50;

  // ADC
  inout ADC_CS_N;
  output ADC_DIN;
  input ADC_DOUT;
  output ADC_SCLK;

  // Audio
  input AUD_ADCDAT;
  inout AUD_ADCLRCK;
  inout AUD_BCLK;
  output AUD_DACDAT;
  inout AUD_DACLRCK;
  output AUD_XCK;

  // SDRAM
  output [12:0] DRAM_ADDR;
  output [1:0] DRAM_BA;
  output DRAM_CAS_N;
  output DRAM_CKE;
  output DRAM_CLK;
  output DRAM_CS_N;
  inout [15:0] DRAM_DQ;
  output DRAM_LDQM;
  output DRAM_RAS_N;
  output DRAM_UDQM;
  output DRAM_WE_N;

  // I2C Bus for Configuration of the Audio and Video-In Chips
  output FPGA_I2C_SCLK;
  inout FPGA_I2C_SDAT;

  // 40-pin headers
  inout [35:0] GPIO_0;
  inout [35:0] GPIO_1;

  // Seven Segment Displays
  output [6:0] HEX0;
  output [6:0] HEX1;
  output [6:0] HEX2;
  output [6:0] HEX3;
  output [6:0] HEX4;
  output [6:0] HEX5;

  // IR
  input IRDA_RXD;
  output IRDA_TXD;

  // Pushbuttons
  input [3:0] KEY;

  // LEDs
  output [9:0] LEDR;

  // PS2 Ports
  inout PS2_CLK;
  inout PS2_DAT;

  inout PS2_CLK2;
  inout PS2_DAT2;

  // Slider Switches
  input [9:0] SW;

  // Video-In
  input TD_CLK27;
  input [7:0] TD_DATA;
  input TD_HS;
  output TD_RESET_N;
  input TD_VS;

  // VGA
  output [7:0] VGA_B;
  output VGA_BLANK_N;
  output VGA_CLK;
  output [7:0] VGA_G;
  output VGA_HS;
  output [7:0] VGA_R;
  output VGA_SYNC_N;
  output VGA_VS;



  ////////////////////////////////////
  // HPS Pins
  ////////////////////////////////////

  // DDR3 SDRAM
  output [14:0] HPS_DDR3_ADDR;
  output [2:0] HPS_DDR3_BA;
  output HPS_DDR3_CAS_N;
  output HPS_DDR3_CKE;
  output HPS_DDR3_CK_N;
  output HPS_DDR3_CK_P;
  output HPS_DDR3_CS_N;
  output [3:0] HPS_DDR3_DM;
  inout [31:0] HPS_DDR3_DQ;
  inout [3:0] HPS_DDR3_DQS_N;
  inout [3:0] HPS_DDR3_DQS_P;
  output HPS_DDR3_ODT;
  output HPS_DDR3_RAS_N;
  output HPS_DDR3_RESET_N;
  input HPS_DDR3_RZQ;
  output HPS_DDR3_WE_N;

  // Ethernet
  output HPS_ENET_GTX_CLK;
  inout HPS_ENET_INT_N;
  output HPS_ENET_MDC;
  inout HPS_ENET_MDIO;
  input HPS_ENET_RX_CLK;
  input [3:0] HPS_ENET_RX_DATA;
  input HPS_ENET_RX_DV;
  output [3:0] HPS_ENET_TX_DATA;
  output HPS_ENET_TX_EN;

  // Flash
  inout [3:0] HPS_FLASH_DATA;
  output HPS_FLASH_DCLK;
  output HPS_FLASH_NCSO;

  // Accelerometer
  inout HPS_GSENSOR_INT;

  // General Purpose I/O
  inout [1:0] HPS_GPIO;

  // I2C
  inout HPS_I2C_CONTROL;
  inout HPS_I2C1_SCLK;
  inout HPS_I2C1_SDAT;
  inout HPS_I2C2_SCLK;
  inout HPS_I2C2_SDAT;

  // Pushbutton
  inout HPS_KEY;

  // LED
  inout HPS_LED;

  // SD Card
  output HPS_SD_CLK;
  inout HPS_SD_CMD;
  inout [3:0] HPS_SD_DATA;

  // SPI
  output HPS_SPIM_CLK;
  input HPS_SPIM_MISO;
  output HPS_SPIM_MOSI;
  inout HPS_SPIM_SS;

  // UART
  input HPS_UART_RX;
  output HPS_UART_TX;

  // USB
  inout HPS_CONV_USB_N;
  input HPS_USB_CLKOUT;
  inout [7:0] HPS_USB_DATA;
  input HPS_USB_DIR;
  input HPS_USB_NXT;
  output HPS_USB_STP;

  //=======================================================
  //  REG/WIRE declarations
  //=======================================================

  wire [19:0] hex3_hex0;
  //wire			[15: 0]	hex5_hex4;

  //assign HEX0 = ~hex3_hex0[ 6: 0]; // hex3_hex0[ 6: 0]; 
  //assign HEX1 = ~hex3_hex0[14: 8];
  //assign HEX2 = ~hex3_hex0[22:16];
  //assign HEX3 = ~hex3_hex0[30:24];
  //assign HEX4 = 7'b1111111;
  assign HEX5 = 7'b1111111;

  HexDigit Digit0 (
      HEX0,
      hex3_hex0[3:0]
  );
  HexDigit Digit1 (
      HEX1,
      hex3_hex0[7:4]
  );
  HexDigit Digit2 (
      HEX2,
      hex3_hex0[11:8]
  );
  HexDigit Digit3 (
      HEX3,
      hex3_hex0[15:12]
  );

  HexDigit Digit4 (
      HEX4,
      hex3_hex0[19:16]
  );

  // Neuron parameters and signals
  wire [3:0] a1, a2, a3, a4, a5, a6;
  wire [3:0] b1, b2, b3, b4, b5, b6;
  wire [7:0] dt_init;
  wire signed [17:0] v1_init, v2_init, v3_init, v4_init, v5_init, v6_init;
  wire signed [17:0] u1_init, u2_init, u3_init, u4_init, u5_init, u6_init;
  wire signed [17:0] c1, c2, c3, c4, c5, c6;
  wire signed [17:0] d1, d2, d3, d4, d5, d6;
  wire signed [17:0] Ibase, I1, I2, I3, I4, I5, I6;

  wire signed [17:0] v1, v2, v3, v4, v5, v6;
  wire signed [17:0] u1, u2, u3, u4, u5, u6;
  wire signed [17:0] dv1, dv2, dv3, dv4, dv5, dv6;
  wire signed [17:0] du1, du2, du3, du4, du5, du6;
  wire signed [17:0] neu1In, neu2In, neu3In, neu4In, neu5In, neu6In;
  wire s1, s2, s3, s4, s5, s6;

  wire [31:0] pio_out0, pio_out1, pio_out2, pio_out3, pio_out4, pio_out5, pio_out6, pio_out7, pio_out8, pio_out9, pio_out10, pio_out11, pio_out12, pio_out13, pio_out14, pio_out15, pio_out16, pio_out17, pio_out18, pio_out19, pio_out20, pio_out21, pio_out22, pio_out23, pio_out24, pio_out25, pio_out26, pio_out27, pio_out28;
  wire [31:0] pio_in0, pio_in1, pio_in2, pio_in3, pio_in4, pio_in5, pio_in6, pio_in7, pio_in8, pio_in9, pio_in10, pio_in11, pio_in12, pio_in13, pio_in14, pio_in15, pio_in16, pio_in17, pio_in18, pio_in19, pio_in20, pio_in21, pio_in22, pio_in23;


  wire pio_clk, pio_rst;

  //input parameters for neurons

  assign v1_init = {{14{pio_out10[17]}}, pio_out10[17:0]};
  assign v2_init = {{14{pio_out11[17]}}, pio_out11[17:0]};
  assign v3_init = {{14{pio_out12[17]}}, pio_out12[17:0]};
  assign v4_init = {{14{pio_out13[17]}}, pio_out13[17:0]};

  assign v5_init = {{14{pio_out19[17]}}, pio_out19[17:0]};
  assign v6_init = {{14{pio_out20[17]}}, pio_out20[17:0]};

  assign u1_init = {{14{pio_out14[17]}}, pio_out14[17:0]};
  assign u2_init = {{14{pio_out15[17]}}, pio_out15[17:0]};
  assign u3_init = {{14{pio_out16[17]}}, pio_out16[17:0]};
  assign u4_init = {{14{pio_out17[17]}}, pio_out17[17:0]};

  assign u5_init = {{14{pio_out21[17]}}, pio_out21[17:0]};
  assign u6_init = {{14{pio_out22[17]}}, pio_out22[17:0]};


  assign a1 = pio_out0[3:0];
  assign a2 = pio_out0[7:4];
  assign a3 = pio_out0[11:8];
  assign a4 = pio_out0[15:12];
  assign a5 = pio_out23[3:0];
  assign a6 = pio_out23[7:4];


  assign b1 = pio_out0[19:16];
  assign b2 = pio_out0[23:20];
  assign b3 = pio_out0[27:24];
  assign b4 = pio_out0[31:28];
  assign b5 = pio_out23[11:8];
  assign b6 = pio_out23[15:12];



  assign c1 = {{14{pio_out1[17]}}, pio_out1[17:0]};
  assign c2 = {{14{pio_out2[17]}}, pio_out2[17:0]};
  assign c3 = {{14{pio_out4[17]}}, pio_out4[17:0]};  //skip 3 because it's already assigned to Ibase
  assign c4 = {{14{pio_out5[17]}}, pio_out5[17:0]};
  assign c5 = {{14{pio_out24[17]}}, pio_out24[17:0]};
  assign c6 = {{14{pio_out25[17]}}, pio_out25[17:0]};

  assign d1 = {{14{pio_out6[17]}}, pio_out6[17:0]};
  assign d2 = {{14{pio_out7[17]}}, pio_out7[17:0]};
  assign d3 = {{14{pio_out8[17]}}, pio_out8[17:0]};
  assign d4 = {{14{pio_out9[17]}}, pio_out9[17:0]};
  assign d5 = {{14{pio_out26[17]}}, pio_out26[17:0]};
  assign d6 = {{14{pio_out27[17]}}, pio_out27[17:0]};

  wire [17:0] pio_i;
  assign pio_i = {{14{pio_out3[17]}}, pio_out3[17:0]};  //didn't want to change to avoid breaking code

  wire [17:0] sw_i;
  assign sw_i = {{4{SW[9]}}, SW[9:1], 4'd0};

  assign pio_in0 = {{14{v1[17]}}, v1};
  assign pio_in1 = {{14{v2[17]}}, v2};
  assign pio_in2 = {{14{v3[17]}}, v3};
  assign pio_in3 = {{14{v4[17]}}, v4};

  assign pio_in16 = {{14{v5[17]}}, v5};
  assign pio_in17 = {{14{v6[17]}}, v6};

  assign pio_in12 = {{14{u1[17]}}, u1};
  assign pio_in13 = {{14{u2[17]}}, u2};
  assign pio_in14 = {{14{u3[17]}}, u3};
  assign pio_in15 = {{14{u4[17]}}, u4};

  assign pio_in18 = {{14{u5[17]}}, u5};
  assign pio_in19 = {{14{u6[17]}}, u6};

  //something

  assign pio_in4 = {{14{du1[17]}}, du1};
  assign pio_in5 = {{14{dv1[17]}}, dv1};
  assign pio_in6 = {{14{du2[17]}}, du2};
  assign pio_in7 = {{14{dv2[17]}}, dv2};
  assign pio_in8 = {{14{du3[17]}}, du3};
  assign pio_in9 = {{14{dv3[17]}}, dv3};
  assign pio_in10 = {{14{du4[17]}}, du4};
  assign pio_in11 = {{14{dv4[17]}}, dv4};
  assign pio_in20 = {{14{du5[17]}}, du5};
  assign pio_in21 = {{14{dv5[17]}}, dv5};
  assign pio_in22 = {{14{du6[17]}}, du6};
  assign pio_in23 = {{14{dv6[17]}}, dv6};

  wire neuronClock;
  wire neuronReset;
  assign neuronClock = SW[0] ? pio_clk : CLOCK_50;
  assign neuronReset = SW[0] ? pio_rst : ~KEY[0];
  assign dt_init     = SW[0] ? pio_out18[7:0] : 8'd8;

  assign Ibase       = pio_i;

  assign LEDR[0]     = neuronClock;
  assign LEDR[1]     = neuronReset;

  assign LEDR[9]     = s1;
  assign LEDR[8]     = s2;
  assign LEDR[7]     = s3;
  assign LEDR[6]     = s4;
  assign LEDR[5]     = s5;
  assign LEDR[4]     = s6;

  assign hex3_hex0   = sw_i;

  // Bias current assignment
  //assign Ibase = 17'h0_0d00;
  assign I1          = KEY[1] ? Ibase + 30 : Ibase + 30 + sw_i;
  assign I2          = Ibase + 30;
  assign I3          = Ibase;
  assign I4          = Ibase;
  assign I5          = Ibase;
  assign I6          = KEY[1] ? Ibase : Ibase + sw_i;

  // Burster parameters
  //assign a1 = 6;  // 0.016
  //assign b = 2;  // 0.25
  // SCALE c, d, and I by 0.01 from Izhikevich table values
  //assign c = 18'sh3_8000;  // -0.5 Izhikevich table value (50) times 0.01
  //assign d = 18'sh0_051E;  // 0.02 Izhikevich table value (2) times 0.01
  //assign a2 = 6;

  // ----------------------------------------------------------------
  // Network Instantiation
  // ----------------------------------------------------------------
  // Connection map (PRE_POST naming):
  //AB is bias-driven only (neu1In = I1).
  //VD receives: AB_VD -> IC_VD -> LP_VD -> neu2In
  //IC receives: AB_IC -> VD_IC -> PY_IC -> PD_IC -> neu3In
  //PY receives: AB_PY -> VD_PY -> LP_PY -> PD_PY -> neu4In
  //LP receives: AB_LP -> VD_LP -> PY_LP -> PD_LP -> neu5In
  //PD receives: LP_PD -> VD_PD -> neu6In
  //Notes:
  //Instance name PRE_POST means spike from PRE contributes to POST path.
  //Each chain's final stage output drives neuXIn for that postsynaptic neuron.

  //AB Neuron
  wire signed [17:0] chem1In, chem2In, chem3In, chem4In, chem5In, chem6In;
  wire signed [17:0] elec1In, elec2In, elec3In, elec4In, elec5In, elec6In;
  assign chem1In = I1;
  Iz_neuron AB (
      v1,
      u1,
      s1,
      du1,
      dv1,
      a1,
      b1,
      c1,
      d1,
      v1_init,
      u1_init,
      neu1In,
      neuronClock,
      neuronReset,
      dt_init
  );

  // Daisy-chain staging wires for postsynaptic neuron input
  wire signed [17:0] vd_in_1, vd_in_2;
  wire signed [17:0] ic_in_1, ic_in_2, ic_in_3;
  wire signed [17:0] py_in_1, py_in_2, py_in_3;
  wire signed [17:0] lp_in_1, lp_in_2, lp_in_3;
  //Electrical wires
  wire signed [17:0] elec_ab_0, elec_ab_1, elec_ab_2;
  wire signed [17:0] elec_vd_0, elec_vd_1, elec_vd_2;
  wire signed [17:0] elec_pd_0, elec_pd_1, elec_pd_2;
  wire signed [17:0] elec_py_0, elec_py_1;
  wire signed [17:0] elec_lp_0, elec_lp_1;
  wire signed [17:0] elec_ic_0;

  assign elec_ab_0 = 18'sh0;
  assign elec_vd_0 = 18'sh0;
  assign elec_pd_0 = 18'sh0;
  assign elec_py_0 = 18'sh0;
  assign elec_lp_0 = 18'sh0;
  assign elec_ic_0 = 18'sh0;

  //Electrical links (rect=1 => current from v1 source to v2 target).
  //AB_VD: rectify VD -> AB
  single_electrical_synapse AB_VD_ELEC (
      elec_ab_1,
      elec_vd_1,
      v1,
      v2,
      4'd6,
      4'd1,
      elec_ab_0,
      elec_vd_0
  );
  single_electrical_synapse AB_PD_ELEC (
      elec_pd_1,
      elec_ab_2,
      v6,
      v1,
      4'd6,
      4'd1,
      elec_pd_0,
      elec_ab_1
  );
  //PY_LP: rectify LP -> PY
  single_electrical_synapse PY_LP_ELEC (
      elec_py_1,
      elec_lp_1,
      v4,
      v5,
      4'd6,
      4'd1,
      elec_py_0,
      elec_lp_0
  );
  //VD_PD: rectify VD -> PD
  single_electrical_synapse VD_PD_ELEC (
      elec_pd_2,
      elec_vd_2,
      v6,
      v2,
      4'd6,
      4'd1,
      elec_pd_1,
      elec_vd_1
  );

  assign elec1In = elec_ab_2;
  assign elec2In = elec_vd_2;
  assign elec3In = elec_ic_0;
  assign elec4In = elec_py_1;
  assign elec5In = elec_lp_1;
  assign elec6In = elec_pd_2;

  assign neu1In  = chem1In + elec1In;
  assign neu2In  = chem2In + elec2In;
  assign neu3In  = chem3In + elec3In;
  assign neu4In  = chem4In + elec4In;
  assign neu5In  = chem5In + elec5In;
  assign neu6In  = chem6In + elec6In;

  //VD neuron (AB -> IC -> LP -> VD)
  single_synapse AB_VD (
      vd_in_1,
      s1,
      18'sh3_f000,
      4'd4,
      I2,
      dt_init,
      neuronClock,
      neuronReset
  );  //inhibitory
  single_synapse IC_VD (
      vd_in_2,
      s3,
      18'sh3_f000,
      4'd4,
      vd_in_1,
      dt_init,
      neuronClock,
      neuronReset
  );  //inhibitory
  single_synapse LP_VD (
      chem2In,
      s5,
      18'sh3_f000,
      4'd4,
      vd_in_2,
      dt_init,
      neuronClock,
      neuronReset
  );  // inhibitory
  Iz_neuron VD (
      v2,
      u2,
      s2,
      du2,
      dv2,
      a2,
      b2,
      c2,
      d2,
      v2_init,
      u2_init,
      neu2In,
      neuronClock,
      neuronReset,
      dt_init
  );

  //IC Neuron (AB -> VD -> PY -> PD -> IC)
  single_synapse AB_IC (
      ic_in_1,
      s1,
      18'sh3_f000,
      4'd4,
      I3,
      dt_init,
      neuronClock,
      neuronReset
  );  //inhibitory
  single_synapse VD_IC (
      ic_in_2,
      s2,
      18'sh0_1000,
      4'd4,
      ic_in_1,
      dt_init,
      neuronClock,
      neuronReset
  );  //excitatory
  single_synapse PY_IC (
      ic_in_3,
      s4,
      18'sh3_f000,
      4'd4,
      ic_in_2,
      dt_init,
      neuronClock,
      neuronReset
  );  //inhibitory
  single_synapse PD_IC (
      chem3In,
      s6,
      18'sh0_1000,
      4'd4,
      ic_in_3,
      dt_init,
      neuronClock,
      neuronReset
  );  //excitatory
  Iz_neuron IC (
      v3,
      u3,
      s3,
      du3,
      dv3,
      a3,
      b3,
      c3,
      d3,
      v3_init,
      u3_init,
      neu3In,
      neuronClock,
      neuronReset,
      dt_init
  );

  //PY Neuron (AB -> VD -> LP -> PD -> PY)
  single_synapse AB_PY (
      py_in_1,
      s1,
      18'sh3_f000,
      4'd4,
      I4,
      dt_init,
      neuronClock,
      neuronReset
  );  //inhibitory
  single_synapse VD_PY (
      py_in_2,
      s2,
      18'sh0_1000,
      4'd4,
      py_in_1,
      dt_init,
      neuronClock,
      neuronReset
  );  //excitatory
  single_synapse LP_PY (
      py_in_3,
      s5,
      18'sh3_f000,
      4'd4,
      py_in_2,
      dt_init,
      neuronClock,
      neuronReset
  );  //inhibitory
  single_synapse PD_PY (
      chem4In,
      s6,
      18'sh0_1000,
      4'd4,
      py_in_3,
      dt_init,
      neuronClock,
      neuronReset
  );  //excitatory

  Iz_neuron PY (
      v4,
      u4,
      s4,
      du4,
      dv4,
      a4,
      b4,
      c4,
      d4,
      v4_init,
      u4_init,
      neu4In,
      neuronClock,
      neuronReset,
      dt_init
  );

  //LP Neuron (AB -> VD -> PY -> PD -> LP)
  single_synapse AB_LP (
      lp_in_1,
      s1,
      18'sh3_f000,
      4'd4,
      I5,
      dt_init,
      neuronClock,
      neuronReset
  );  //inhibitory
  single_synapse VD_LP (
      lp_in_2,
      s2,
      18'sh0_1000,
      4'd4,
      lp_in_1,
      dt_init,
      neuronClock,
      neuronReset
  );  //excitatory
  single_synapse PY_LP (
      lp_in_3,
      s4,
      18'sh3_f000,
      4'd4,
      lp_in_2,
      dt_init,
      neuronClock,
      neuronReset
  );  //inhibitory
  single_synapse PD_LP (
      chem5In,
      s6,
      18'sh0_1000,
      4'd4,
      lp_in_3,
      dt_init,
      neuronClock,
      neuronReset
  );  //excitatory
  Iz_neuron LP (
      v5,
      u5,
      s5,
      du5,
      dv5,
      a5,
      b5,
      c5,
      d5,
      v5_init,
      u5_init,
      neu5In,
      neuronClock,
      neuronReset,
      dt_init
  );

  //PD Neuron (LP -> VD -> PD)
  single_synapse LP_PD (
      chem6In,
      s5,
      18'sh3_f000,
      4'd4,
      I6,
      dt_init,
      neuronClock,
      neuronReset
  );  // inhibitory; VD->PD is electrical (VD_PD_ELEC)
  Iz_neuron PD (
      v6,
      u6,
      s6,
      du6,
      dv6,
      a6,
      b6,
      c6,
      d6,
      v6_init,
      u6_init,
      neu6In,
      neuronClock,
      neuronReset,
      dt_init
  );


  //=======================================================
  //  Structural coding
  //=======================================================

  Computer_System The_System (
      ////////////////////////////////////
      // FPGA Side
      ////////////////////////////////////

      // Global signals
      .system_pll_ref_clk_clk    (CLOCK_50),
      .system_pll_ref_reset_reset(1'b0),

      // AV Config
      .av_config_SCLK(FPGA_I2C_SCLK),
      .av_config_SDAT(FPGA_I2C_SDAT),

      // VGA Subsystem
      .vga_pll_ref_clk_clk    (CLOCK2_50),
      .vga_pll_ref_reset_reset(1'b0),
      .vga_CLK                (VGA_CLK),
      .vga_BLANK              (VGA_BLANK_N),
      .vga_SYNC               (VGA_SYNC_N),
      .vga_HS                 (VGA_HS),
      .vga_VS                 (VGA_VS),
      .vga_R                  (VGA_R),
      .vga_G                  (VGA_G),
      .vga_B                  (VGA_B),

      // SDRAM
      .sdram_clk_clk(DRAM_CLK),
      .sdram_addr   (DRAM_ADDR),
      .sdram_ba     (DRAM_BA),
      .sdram_cas_n  (DRAM_CAS_N),
      .sdram_cke    (DRAM_CKE),
      .sdram_cs_n   (DRAM_CS_N),
      .sdram_dq     (DRAM_DQ),
      .sdram_dqm    ({DRAM_UDQM, DRAM_LDQM}),
      .sdram_ras_n  (DRAM_RAS_N),
      .sdram_we_n   (DRAM_WE_N),

      ////////////////////////////////////
      // HPS Side
      ////////////////////////////////////
      // DDR3 SDRAM
      .memory_mem_a   (HPS_DDR3_ADDR),
      .memory_mem_ba   (HPS_DDR3_BA),
      .memory_mem_ck   (HPS_DDR3_CK_P),
      .memory_mem_ck_n  (HPS_DDR3_CK_N),
      .memory_mem_cke  (HPS_DDR3_CKE),
      .memory_mem_cs_n  (HPS_DDR3_CS_N),
      .memory_mem_ras_n  (HPS_DDR3_RAS_N),
      .memory_mem_cas_n  (HPS_DDR3_CAS_N),
      .memory_mem_we_n  (HPS_DDR3_WE_N),
      .memory_mem_reset_n (HPS_DDR3_RESET_N),
      .memory_mem_dq   (HPS_DDR3_DQ),
      .memory_mem_dqs  (HPS_DDR3_DQS_P),
      .memory_mem_dqs_n  (HPS_DDR3_DQS_N),
      .memory_mem_odt  (HPS_DDR3_ODT),
      .memory_mem_dm   (HPS_DDR3_DM),
      .memory_oct_rzqin  (HPS_DDR3_RZQ),

      // Ethernet
      .hps_io_hps_io_gpio_inst_GPIO35(HPS_ENET_INT_N),
      .hps_io_hps_io_emac1_inst_TX_CLK(HPS_ENET_GTX_CLK),
      .hps_io_hps_io_emac1_inst_TXD0(HPS_ENET_TX_DATA[0]),
      .hps_io_hps_io_emac1_inst_TXD1(HPS_ENET_TX_DATA[1]),
      .hps_io_hps_io_emac1_inst_TXD2(HPS_ENET_TX_DATA[2]),
      .hps_io_hps_io_emac1_inst_TXD3(HPS_ENET_TX_DATA[3]),
      .hps_io_hps_io_emac1_inst_RXD0(HPS_ENET_RX_DATA[0]),
      .hps_io_hps_io_emac1_inst_MDIO(HPS_ENET_MDIO),
      .hps_io_hps_io_emac1_inst_MDC(HPS_ENET_MDC),
      .hps_io_hps_io_emac1_inst_RX_CTL(HPS_ENET_RX_DV),
      .hps_io_hps_io_emac1_inst_TX_CTL(HPS_ENET_TX_EN),
      .hps_io_hps_io_emac1_inst_RX_CLK(HPS_ENET_RX_CLK),
      .hps_io_hps_io_emac1_inst_RXD1(HPS_ENET_RX_DATA[1]),
      .hps_io_hps_io_emac1_inst_RXD2(HPS_ENET_RX_DATA[2]),
      .hps_io_hps_io_emac1_inst_RXD3(HPS_ENET_RX_DATA[3]),

      // Flash
      .hps_io_hps_io_qspi_inst_IO0(HPS_FLASH_DATA[0]),
      .hps_io_hps_io_qspi_inst_IO1(HPS_FLASH_DATA[1]),
      .hps_io_hps_io_qspi_inst_IO2(HPS_FLASH_DATA[2]),
      .hps_io_hps_io_qspi_inst_IO3(HPS_FLASH_DATA[3]),
      .hps_io_hps_io_qspi_inst_SS0(HPS_FLASH_NCSO),
      .hps_io_hps_io_qspi_inst_CLK(HPS_FLASH_DCLK),

      // Accelerometer
      .hps_io_hps_io_gpio_inst_GPIO61(HPS_GSENSOR_INT),

      //.adc_sclk                        (ADC_SCLK),
      //.adc_cs_n                        (ADC_CS_N),
      //.adc_dout                        (ADC_DOUT),
      //.adc_din                         (ADC_DIN),

      // General Purpose I/O
      .hps_io_hps_io_gpio_inst_GPIO40(HPS_GPIO[0]),
      .hps_io_hps_io_gpio_inst_GPIO41(HPS_GPIO[1]),

      // I2C
      .hps_io_hps_io_gpio_inst_GPIO48(HPS_I2C_CONTROL),
      .hps_io_hps_io_i2c0_inst_SDA(HPS_I2C1_SDAT),
      .hps_io_hps_io_i2c0_inst_SCL(HPS_I2C1_SCLK),
      .hps_io_hps_io_i2c1_inst_SDA(HPS_I2C2_SDAT),
      .hps_io_hps_io_i2c1_inst_SCL(HPS_I2C2_SCLK),

      // Pushbutton
      .hps_io_hps_io_gpio_inst_GPIO54(HPS_KEY),

      // LED
      .hps_io_hps_io_gpio_inst_GPIO53(HPS_LED),

      // SD Card
      .hps_io_hps_io_sdio_inst_CMD(HPS_SD_CMD),
      .hps_io_hps_io_sdio_inst_D0 (HPS_SD_DATA[0]),
      .hps_io_hps_io_sdio_inst_D1 (HPS_SD_DATA[1]),
      .hps_io_hps_io_sdio_inst_CLK(HPS_SD_CLK),
      .hps_io_hps_io_sdio_inst_D2 (HPS_SD_DATA[2]),
      .hps_io_hps_io_sdio_inst_D3 (HPS_SD_DATA[3]),

      // SPI
      .hps_io_hps_io_spim1_inst_CLK (HPS_SPIM_CLK),
      .hps_io_hps_io_spim1_inst_MOSI(HPS_SPIM_MOSI),
      .hps_io_hps_io_spim1_inst_MISO(HPS_SPIM_MISO),
      .hps_io_hps_io_spim1_inst_SS0 (HPS_SPIM_SS),

      // UART
      .hps_io_hps_io_uart0_inst_RX(HPS_UART_RX),
      .hps_io_hps_io_uart0_inst_TX(HPS_UART_TX),

      // USB
      .hps_io_hps_io_gpio_inst_GPIO09(HPS_CONV_USB_N),
      .hps_io_hps_io_usb1_inst_D0(HPS_USB_DATA[0]),
      .hps_io_hps_io_usb1_inst_D1(HPS_USB_DATA[1]),
      .hps_io_hps_io_usb1_inst_D2(HPS_USB_DATA[2]),
      .hps_io_hps_io_usb1_inst_D3(HPS_USB_DATA[3]),
      .hps_io_hps_io_usb1_inst_D4(HPS_USB_DATA[4]),
      .hps_io_hps_io_usb1_inst_D5(HPS_USB_DATA[5]),
      .hps_io_hps_io_usb1_inst_D6(HPS_USB_DATA[6]),
      .hps_io_hps_io_usb1_inst_D7(HPS_USB_DATA[7]),
      .hps_io_hps_io_usb1_inst_CLK(HPS_USB_CLKOUT),
      .hps_io_hps_io_usb1_inst_STP(HPS_USB_STP),
      .hps_io_hps_io_usb1_inst_DIR(HPS_USB_DIR),
      .hps_io_hps_io_usb1_inst_NXT(HPS_USB_NXT),

      // Generic PIOs
      .pio_out0_export (pio_out0),
      .pio_out1_export (pio_out1),
      .pio_out2_export (pio_out2),
      .pio_out3_export (pio_out3),
      .pio_out4_export (pio_out4),
      .pio_out5_export (pio_out5),
      .pio_out6_export (pio_out6),
      .pio_out7_export (pio_out7),
      .pio_out8_export (pio_out8),
      .pio_out9_export (pio_out9),
      .pio_in0_export  (pio_in0),
      .pio_in1_export  (pio_in1),
      .pio_in2_export  (pio_in2),
      .pio_in3_export  (pio_in3),
      .pio_in4_export  (pio_in4),
      .pio_in5_export  (pio_in5),
      .pio_in6_export  (pio_in6),
      .pio_in7_export  (pio_in7),
      .pio_in8_export  (pio_in8),
      .pio_in9_export  (pio_in9),
      .pio_in10_export (pio_in10),
      .pio_in11_export (pio_in11),
      .pio_in12_export (pio_in12),
      .pio_in13_export (pio_in13),
      .pio_in14_export (pio_in14),
      .pio_in15_export (pio_in15),
      .pio_in16_export (pio_in16),
      .pio_in17_export (pio_in17),
      .pio_in18_export (pio_in18),
      .pio_in19_export (pio_in19),
      .pio_in20_export (pio_in20),
      .pio_in21_export (pio_in21),
      .pio_in22_export (pio_in22),
      .pio_in23_export (pio_in23),
      .pio_out10_export(pio_out10),
      .pio_out11_export(pio_out11),
      .pio_out12_export(pio_out12),
      .pio_out13_export(pio_out13),
      .pio_out14_export(pio_out14),
      .pio_out15_export(pio_out15),
      .pio_out16_export(pio_out16),
      .pio_out17_export(pio_out17),
      .pio_out18_export(pio_out18),
      .pio_out19_export(pio_out19),
      .pio_out20_export(pio_out20),
      .pio_out21_export(pio_out21),
      .pio_out22_export(pio_out22),
      .pio_out23_export(pio_out23),
      .pio_out24_export(pio_out24),
      .pio_out25_export(pio_out25),
      .pio_out26_export(pio_out26),
      .pio_out27_export(pio_out27),
      .pio_out28_export(pio_out28),
      .pio_clock_export(pio_clk),
      .pio_reset_export(pio_rst)
      //////need to add more pio_out for phase portrait
      //add pio for neuron to plot phase portrait
      //add pio for intial Vs
      //add pio for initial Us
      //add pio for beginning plot
      //add pio for reset plot
      //add pio for V when doing phase plot
      //add pio for U when doing phase plot


  );


endmodule




// Electrical (gap) synapse; cond is a right-shift, rect 0/1/2, daisy-chain via in1/in2.

module single_electrical_synapse (
    I2_out,
    I1_out,
    v2,
    v1,
    cond,
    rect,
    in2,
    in1
);
  output reg signed [17:0] I2_out, I1_out;  // the simulated synaptic current
  input signed [17:0] in1, in2;  // the input currents for daisy-chaining
  input [3:0] cond, rect;  // conductance and rectification settings
  input signed [17:0] v1, v2;

  wire signed [17:0] v_diff;
  reg signed  [17:0] rect_v_diff;
  assign v_diff = v1 - v2;
  always @(*) begin
    case (rect)
      0: rect_v_diff = v_diff;
      1: rect_v_diff = v_diff > 0 ? v_diff : 18'd0;
      2: rect_v_diff = v_diff < 0 ? v_diff : 18'd0;
    endcase
    I2_out = ((rect_v_diff) >>> cond) + in2;
    I1_out = ((-rect_v_diff) >>> cond) + in1;
  end
endmodule



// Chemical synapse: spike-gated exponential current; daisy-chained through in.
module single_synapse (
    out,
    s1,
    w1,
    tau,
    in,
    dt,
    clk,
    reset
);
  output signed [17:0] out;
  input signed [17:0] in;
  input [3:0] tau;
  input [7:0] dt;
  
  input s1;
  input signed [17:0] w1;
  input clk, reset;

  reg signed  [17:0] v1;
  wire signed [17:0] v1new;

  always @(posedge clk) begin
    if (reset) v1 <= 18'sh0;
    else v1 <= v1new;
  end

  assign v1new = v1 + ((-v1) >>> tau + dt) + (s1 ? w1 : 0);
  assign out   = v1 + in;
endmodule

// Izhikevich neuron (Q2.16). a/b are shift indices; scale c,d,I by 0.01 from tables.
module Iz_neuron (
    out,  //the simulated membrane voltage
    u,  //the membrane recovery variable
    spike,  // the action potential output
    du,  //the derivative of the membrane voltage
    dv,  //the derivative of the membrane recovery variable
    a,  //the index for the membrane voltage shift
    b,  //the index for the membrane recovery variable shift
    c,  //the constant for the membrane voltage
    d,  //the constant for the membrane recovery variable
    v1_init,  //the initial membrane voltage
    u1_init,  //the initial membrane recovery variable
    I,  //the input current
    clk,  //the clock signal
    reset,  //the reset signal
    dt  //the time step
);

  output [17:0] out;
  output [17:0] u;
  output spike;
  input signed [17:0] v1_init, u1_init;
  output signed [17:0] du, dv;
  input signed [17:0] c, d, I;
  input [3:0] a, b;
  input [7:0] dt;
  input clk, reset;

  reg spike;
  reg signed [17:0] v1, u1;
  wire signed [17:0] u1reset, v1new, u1new, du1, dv1;
  wire signed [17:0] v1xv1, v1xb;
  wire signed [17:0] p, c14;

  assign p   = 18'sh0_4CCC;  // 0.30
  assign c14 = 18'sh1_6666;  // 1.4

  assign out = v1;
  assign u   = u1;
  assign du  = du1;  //out of sync with v1
  assign dv  = dv1;  //out of sync with v1

  always @(posedge clk) begin
    if (reset) begin
      v1 <= v1_init;  //18'sh3_4CCD;  // -0.7 change to be input from PIO
      u1 <= u1_init;  //18'sh3_CCCD;  // -0.2 change to be input from PIO
      spike <= 0;
    end else begin
      if (v1 > p) begin
        v1 <= c;
        u1 <= u1reset;
        spike <= 1;
      end else begin
        v1 <= v1new;
        u1 <= u1new;
        spike <= 0;
      end
    end
  end

  signed_mult v1sq (
      v1xv1,
      v1,
      v1
  );

  // dt = 1/16 or 2>>4
  // v1(n+1) = v1(n) + dt*(4*(v1(n)^2) + 5*v1(n) +1.40 - u1(n) + I)
  // but note that what is actually coded is
  // v1(n+1) = v1(n) + (v1(n)^2) + 5/4*v1(n) +1.40/4 - u1(n)/4 + I/4)/4

  assign dv1 = ((v1xv1 + v1 + (v1 >>> 2) + (c14 >>> 2) - (u1 >>> 2) + (I >>> 2)));
  // Integrate v with (dt>>2); large dt can zero an 18-bit dv1.
  assign v1new = v1 + (dv1 >>> (dt >> 2));
  assign v1xb = v1 >>> b;

  // u1(n+1) = u1 + dt*a*(b*v1(n) - u1(n)); SW dt is shift count for u only (4 ~= old >>>4)
  assign du1 = (v1xb - u1) >>> a;
  assign u1new = u1 + (du1 >>> dt);
  assign u1reset = u1 + d;
endmodule





module signed_mult (
    out,
    a,
    b
);
  output [17:0] out;
  input signed [17:0] a;
  input signed [17:0] b;

  wire signed [35:0] mult_out;

  assign mult_out = a * b;
  assign out = {mult_out[35], mult_out[32:16]};
endmodule
