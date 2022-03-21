#ifndef FILTER_DEF_H_
#define FILTER_DEF_H_

// GB mask parameters
const int MASK_X = 3;
const int MASK_Y = 3;

// GB Filter inner transport addresses
// Used between blocking_transport() & do_filter()
const int GB_FILTER_R_ADDR = 0x00000000;
const int GB_FILTER_RESULT_ADDR = 0x00000004;
const int GB_FILTER_X_ADDR = 0x00000008;
const int GB_FILTER_Y_ADDR = 0x0000000c;
const int GB_FILTER_CHECK_ADDR = 0x00000010;

union word {
  int sint;
  //unsigned int uint;
  unsigned char uc[4];
};

// GB mask
const int mask[MASK_X][MASK_Y] = {{1, 2, 1}, {2, 4, 2}, {1, 2, 1}};
#endif
