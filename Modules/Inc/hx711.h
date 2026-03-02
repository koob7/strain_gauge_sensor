#include "hx711_driver.h"
#include "interface.h"

class hx711_t : public interface_t
{

    hx711_driver_t hx711_driver;

  public:
    hx711_t(interface_id_t id)
        : interface_t(id, 3), hx711_driver(hx711_driver_t::gain_ranges_t::GAIN_64, HX711_DOUT_GPIO_Port, HX711_DOUT_Pin,
                                           HX711_SCK_GPIO_Port, HX711_SCK_Pin)
    {
    }

    bool init();
    const char *get_char_name() { return "hx711"; }

  private:
    bool wake_up_interface() override;
    bool configure_interface(uint8_t param_1) override;
    int32_t read_bridge_voltage() override;
    bool sleep_interface() override;
};