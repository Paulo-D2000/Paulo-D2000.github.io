#include "uintN_t.h"  // uintN_t types for any N
#include "intN_t.h" // intN_t types for any N

// Install+configure synthesis tool then specify part here
// #pragma PART "xc7a35ticsg324-1l" 

// Generate top level debug ports with associated pipelinec_verilator.h
#include "debug_port.h"
DEBUG_OUTPUT_DECL(uint8_t, phase_debug)
DEBUG_OUTPUT_DECL(int16_t, cos_debug)
DEBUG_OUTPUT_DECL(int16_t, sin_debug)

// complex int16_t struct
typedef struct ci16{
  int16_t real; // I component (real part)
  int16_t imag;  // Q component (imaginary part)
} ci16;

// nco state struct
typedef struct nco_state{
  ci16 m_out; // output iq
  uint8_t m_phase; // phase accumulator
  uint8_t m_control; // control word 
} nco_state;

// NCO cosine table
#define TABLESIZE 256
#define OFFSET TABLESIZE/4
int16_t table[TABLESIZE] = {32767,  32757,  32727,  32677,  32607,  32518,  32409,  32280,
        32132,  31964,  31777,  31570,  31345,  31100,  30836,  30554,
        30253,  29934,  29596,  29241,  28868,  28477,  28069,  27644,
        27202,  26744,  26269,  25779,  25273,  24751,  24215,  23663,
        23098,  22518,  21925,  21318,  20699,  20067,  19422,  18766,
        18099,  17420,  16731,  16032,  15323,  14605,  13878,  13142,
        12399,  11648,  10890,  10125,   9354,   8578,   7796,   7009,
         6219,   5424,   4626,   3826,   3023,   2218,   1412,    605,
         -201,  -1009,  -1815,  -2621,  -3425,  -4226,  -5026,  -5822,
        -6615,  -7403,  -8187,  -8967,  -9740, -10508, -11270, -12024,
       -12772, -13511, -14243, -14965, -15679, -16383, -17077, -17761,
       -18434, -19096, -19746, -20384, -21010, -21623, -22223, -22810,
       -23382, -23941, -24485, -25014, -25528, -26026, -26509, -26975,
       -27425, -27859, -28275, -28674, -29056, -29421, -29767, -30096,
       -30406, -30697, -30970, -31225, -31460, -31676, -31873, -32050,
       -32209, -32347, -32466, -32565, -32645, -32704, -32744, -32764,
       -32764, -32744, -32704, -32645, -32565, -32466, -32347, -32209,
       -32050, -31873, -31676, -31460, -31225, -30970, -30697, -30406,
       -30096, -29767, -29421, -29056, -28674, -28275, -27859, -27425,
       -26975, -26509, -26026, -25528, -25014, -24485, -23941, -23382,
       -22810, -22223, -21623, -21010, -20384, -19746, -19096, -18434,
       -17761, -17077, -16383, -15679, -14965, -14243, -13511, -12772,
       -12024, -11270, -10508,  -9740,  -8967,  -8187,  -7403,  -6615,
        -5822,  -5026,  -4226,  -3425,  -2621,  -1815,  -1009,   -201,
          605,   1412,   2218,   3023,   3826,   4626,   5424,   6219,
         7009,   7796,   8578,   9354,  10125,  10890,  11648,  12399,
        13142,  13878,  14605,  15323,  16032,  16731,  17420,  18099,
        18766,  19422,  20067,  20699,  21318,  21925,  22518,  23098,
        23663,  24215,  24751,  25273,  25779,  26269,  26744,  27202,
        27644,  28069,  28477,  28868,  29241,  29596,  29934,  30253,
        30554,  30836,  31100,  31345,  31570,  31777,  31964,  32132,
        32280,  32409,  32518,  32607,  32677,  32727,  32757,  32767};


nco_state next_nco(nco_state state){
  state.m_phase += state.m_control;
  state.m_out.real = table[state.m_phase];
  state.m_out.imag = table[state.m_phase+OFFSET % TABLESIZE];
  return state;
}

// Output iq (sine & cosine)
nco_state state;

#pragma MAIN main
ci16 main()
{
  // Connect to cocotb debug wires
  cos_debug = state.m_out.real;
  sin_debug = state.m_out.imag;
  phase_debug = state.m_phase;

  state.m_control = 8;
  state = next_nco(state);
  return state.m_out;
}