#include "interface.h"

class adc_opamp_t : public interface_t
{
  public:
    adc_opamp_t(interface_id_t id) : interface_t(id, 1) {}
    bool init();
    const char *get_char_name() { return "adc_opamp"; }

  private:
    bool wake_up_interface() override;
    bool configure_interface(uint8_t param_1) override;
    int32_t read_bridge_voltage() override;
    bool sleep_interface() override;
};