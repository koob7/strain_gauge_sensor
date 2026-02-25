#include "interface.h"

class adc_t : public interface_t
{
  public:
    adc_t(interface_id_t id) : interface_t(id) {}
    bool init();
    int32_t meassure(uint8_t param_1) { return 0; }
    const char *get_char_name() { return "adc"; }
};