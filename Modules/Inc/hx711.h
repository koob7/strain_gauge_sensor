#include "interface.h"

class hx711_t : public interface_t
{
  public:
    hx711_t(interface_id_t id) : interface_t(id) {}
    bool init();
    int32_t meassure(uint8_t param_1) { return 0; }
    const char *get_char_name() { return "hx711"; }
};