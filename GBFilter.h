#ifndef GB_FILTER_H_
#define GB_FILTER_H_
#include <systemc>
using namespace sc_core;

#include <tlm>
#include <tlm_utils/simple_target_socket.h>

#include "filter_def.h"

class GBFilter : public sc_module {
public:
  tlm_utils::simple_target_socket<GBFilter> t_skt;

  sc_fifo<unsigned char> i_r;
  sc_fifo<unsigned char> i_g;
  sc_fifo<unsigned char> i_b;
  sc_fifo<unsigned char> o_result_r;
  sc_fifo<unsigned char> o_result_g;
  sc_fifo<unsigned char> o_result_b;
  sc_fifo<int> i_x;
  sc_fifo<int> i_y;

  SC_HAS_PROCESS(GBFilter);
  GBFilter(sc_module_name n);
  ~GBFilter();

private:
  void do_filter();

  int val_r, val_g, val_b;
  unsigned char buffer_r[3][256];
  unsigned char buffer_g[3][256];
  unsigned char buffer_b[3][256];
  unsigned int base_offset;
  void blocking_transport(tlm::tlm_generic_payload &payload,
                          sc_core::sc_time &delay);
};
#endif
