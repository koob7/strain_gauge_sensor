#include "interface.h"

class hx711_t : public interface_t
{
  public:
    hx711_t(interface_id_t id) : interface_t(id, 3) {}
    bool init();
    const char *get_char_name() { return "hx711"; }

  private:
    bool wake_up_interface() override { return true; }
    bool configure_interface(uint8_t param_1) override { return true; }
    int32_t read_bridge_voltage() override { return 123; }
    bool sleep_interface() override { return true; }
};