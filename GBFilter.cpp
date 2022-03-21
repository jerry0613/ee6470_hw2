#include <cmath>
#include <iomanip>

#include "GBFilter.h"

GBFilter::GBFilter(sc_module_name n)
    : sc_module(n), t_skt("t_skt"), base_offset(0) {
  SC_THREAD(do_filter);

  t_skt.register_b_transport(this, &GBFilter::blocking_transport);
}

GBFilter::~GBFilter() = default;

void GBFilter::do_filter() {
  int x, y;
  for (int i = 0; i < 256; ++i) {
    buffer_r[0][i] = 0;
    buffer_r[1][i] = 0;
    buffer_r[2][i] = 0;
    buffer_g[0][i] = 0;
    buffer_g[1][i] = 0;
    buffer_g[2][i] = 0;
    buffer_b[0][i] = 0;
    buffer_b[1][i] = 0;
    buffer_b[2][i] = 0;
  }

  while (true) {
    x = i_x.read();
    y = i_y.read();
    
    val_r = 0;
    val_g = 0;
    val_b = 0;
    
    // row buffer
    buffer_r[0][x] = buffer_r[1][x];
    buffer_r[1][x] = buffer_r[2][x];
    buffer_r[2][x] = i_r.read();
    
    buffer_g[0][x] = buffer_g[1][x];
    buffer_g[1][x] = buffer_g[2][x];
    buffer_g[2][x] = i_g.read();
    
    buffer_b[0][x] = buffer_b[1][x];
    buffer_b[1][x] = buffer_b[2][x];
    buffer_b[2][x] = i_b.read();
  
    // do GB filter
    if (y >= 1 && x >= 1) {
      for (unsigned int v = 0; v < MASK_Y; ++v) {
        for (unsigned int u = 0; u < MASK_X; ++u) {
          if ((u + x - 1) >= 0 && (u + x - 1) < 256) {
            val_r += buffer_r[v][u + x - 1] * mask[v][u];
            val_g += buffer_g[v][u + x - 1] * mask[v][u];
            val_b += buffer_b[v][u + x - 1] * mask[v][u];
          }
        }
      }
    
      val_r = val_r / 16;
      val_g = val_g / 16;
      val_b = val_b / 16;

      o_result_r.write(val_r);
      o_result_g.write(val_g);
      o_result_b.write(val_b);
    }
  }
}

void GBFilter::blocking_transport(tlm::tlm_generic_payload &payload,
                                     sc_core::sc_time &delay) {
  sc_dt::uint64 addr = payload.get_address();
  addr -= base_offset;
  unsigned char *mask_ptr = payload.get_byte_enable_ptr();
  unsigned char *data_ptr = payload.get_data_ptr();
  int *int_ptr = (int*) data_ptr;
  word buffer;
  switch (payload.get_command()) {
  case tlm::TLM_READ_COMMAND:
    switch (addr) {
    case GB_FILTER_RESULT_ADDR:
      buffer.uc[0] = o_result_r.read();
      buffer.uc[1] = o_result_g.read();
      buffer.uc[2] = o_result_b.read();
      break;
    case GB_FILTER_CHECK_ADDR:
      buffer.uc[0] = o_result_r.num_available();
      buffer.uc[1] = o_result_g.num_available();
      buffer.uc[2] = o_result_b.num_available();
      break;
    default:
      std::cerr << "Error! GBFilter::blocking_transport: address 0x"
                << std::setfill('0') << std::setw(8) << std::hex << addr
                << std::dec << " is not valid" << std::endl;
      buffer.uc[0] = 0;
      buffer.uc[1] = 0;
      buffer.uc[2] = 0;
    }
    data_ptr[0] = buffer.uc[0];
    data_ptr[1] = buffer.uc[1];
    data_ptr[2] = buffer.uc[2];
    
    break;
  case tlm::TLM_WRITE_COMMAND:
    switch (addr) {
    case GB_FILTER_R_ADDR:
      if (mask_ptr[0] == 0xff) {
        i_r.write(data_ptr[0]);
      }
      if (mask_ptr[1] == 0xff) {
        i_g.write(data_ptr[1]);
      }
      if (mask_ptr[2] == 0xff) {
        i_b.write(data_ptr[2]);
      }
      break;
    case GB_FILTER_X_ADDR:
      i_x.write(*int_ptr);
      break;
    case GB_FILTER_Y_ADDR:
      i_y.write(*int_ptr);
      break;  
    default:
      std::cerr << "Error! GBFilter::blocking_transport: address 0x"
                << std::setfill('0') << std::setw(8) << std::hex << addr
                << std::dec << " is not valid" << std::endl;
    }
    break;
  case tlm::TLM_IGNORE_COMMAND:
    payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
    return;
  default:
    payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
    return;
  }
  payload.set_response_status(tlm::TLM_OK_RESPONSE); // Always OK
}