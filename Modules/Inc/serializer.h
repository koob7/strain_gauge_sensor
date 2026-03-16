#include <optional>
#include <utility>

#include "device.h"

#pragma once

extern uint32_t __user_data_start;
extern uint32_t __user_data_end;
extern uint32_t __flash_base;
extern uint32_t __page_size;

#define NUMBER_OF_USER_PAGES 6

// we assume dual bank is dissabled so page size is 2KB
#define PAGE_SIZE 2048

#define FLASH_PAGE_ADDRESS_START(page_number) (flash_storage_addres_start + (page_number) * PAGE_SIZE)
#define FLASH_PAGE_ADDRESS_END(page_number) (flash_storage_addres_start + (page_number + 1) * PAGE_SIZE)

class serializer_t : public device_t
{
    uint32_t const flash_base                 = reinterpret_cast<uint32_t>(&__flash_base);
    uint32_t const flash_storage_addres_start = reinterpret_cast<uint32_t>(&__user_data_start);
    uint32_t const flash_storage_addres_end   = reinterpret_cast<uint32_t>(&__user_data_end);

    struct flash_layout_t
    {
        uint32_t crc;
        command_t command;

        static constexpr uint32_t crc_size     = sizeof(crc);
        static constexpr uint32_t command_size = sizeof(command_t);
        static constexpr uint32_t layout_size  = crc_size + command_size;
    };

  public:
    serializer_t(module_id_t id) : device_t(id) {}

    uint32_t addres_to_store_next_command[NUMBER_OF_USER_PAGES];

    bool init() override;
    void handle() override;
    bool execute_command(command_t command, [[maybe_unused]] uint16_t internal_parm = 0) override;

    uint32_t calculate_crc(uint8_t *data_pointer, uint8_t data_size);

    bool read_from_flash(uint32_t flash_addres, uint8_t *data_buffer, uint8_t buffer_size);
    [[nodiscard]] uint32_t find_addres_to_store_next_command(uint16_t page_number);
    std::optional<std::pair<uint32_t, uint32_t>> calculate_pages_to_erase(uint32_t start_addres, uint32_t end_addres);
    [[nodiscard]] std::optional<uint32_t> write_double_word_to_flash(uint32_t flash_addres, uint64_t value_to_save);
    [[nodiscard]] std::optional<uint32_t> write_memory_to_flash(uint32_t flash_addres, uint8_t *data_pointer,
                                                                uint8_t data_size);

  private:
    bool erase_memory(uint32_t start_addres, uint32_t end_addres);
    bool erase_single_page(uint16_t page_number);

    /*-----------------------------------------------------*/
    /* NICE ATTEMPT */
    /* but not this time - STM32 internal flash allows only for programming double word only one time per address */
    /* in other case FLASH ECC will trigger error and write command will fail*/
    /*-----------------------------------------------------*/
    /*

    uint32_t const serializer_t::flash_base                 = reinterpret_cast<uint32_t>(&__flash_base);
    uint32_t const serializer_t::flash_storage_addres_start = reinterpret_cast<uint32_t>(&__user_data_start);
    uint32_t const serializer_t::flash_storage_addres_end   = reinterpret_cast<uint32_t>(&__user_data_end);

    bool write_byte_to_flash(uint32_t flash_addres, uint8_t value_to_save);
    bool write_double_word_to_flash(uint32_t flash_addres, uint64_t value_to_save);
    bool write_memory_to_flash(uint32_t flash_addres, uint8_t *data_pointer, uint8_t data_size);
    [[nodiscard]] uint32_t find_addres_to_store_next_command();
    */
};

extern serializer_t *g_serializer;