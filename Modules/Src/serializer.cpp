#include "serializer.h"
#include "crc.h"
#include "helper.h"
#include "main.h"
#include "scheduler.h"
#include <algorithm>

serializer_t *g_serializer = nullptr;

bool serializer_t::init()
{
    state       = state_t::READY;
    bool result = true;

    addres_to_store_next_command = find_addres_to_store_next_command();
    if (addres_to_store_next_command == 0 || addres_to_store_next_command < flash_storage_addres_start ||
        addres_to_store_next_command >= flash_storage_addres_end)
    {
        command_t clear_flash;
        clear_flash.parameters[as_int(command_t::default_command_layout_t::COMMAND_ID)] =
            as_int(command_t::command_code_t::ERASE_FLASH);
        result = execute_command(clear_flash); // ERASE_FLASH command set addres_to_store_next_command
    }

    if (!result)
    {
        state = state_t::ERROR;
    }
    return true;
}

void serializer_t::handle()
{

    if (state == state_t::READY)
    {
        return;
    }

    state       = state_t::READY; // READY state is required to execute command
    bool result = true;
    command_t clear_flash;
    clear_flash.parameters[as_int(command_t::default_command_layout_t::COMMAND_ID)] =
        as_int(command_t::command_code_t::ERASE_FLASH);

    result = execute_command(clear_flash);

    if (!result)
    {
        state = state_t::ERROR;
    }
}

bool serializer_t::execute_command(command_t command)
{
    using code_t   = command_t::command_code_t;
    using layout_t = command_t::default_command_layout_t;

    if (state == state_t::ERROR)
    {
        return false;
    }

    switch (command.parameters[as_int(layout_t::COMMAND_ID)])
    {
    case as_int(code_t::SAVE_COMMAND_TO_FLASH):
    {
        if (addres_to_store_next_command + flash_layout_t::layout_size >= flash_storage_addres_end)
            return false;

        uint32_t crc_result = calculate_crc(command.get_parameters_pointer(), command.get_parameters_size());

        flash_layout_t flash_layout;
        flash_layout.crc     = crc_result;
        flash_layout.command = command;

        std::optional<uint32_t> result;
        result = write_memory_to_flash(addres_to_store_next_command, reinterpret_cast<uint8_t *>(&flash_layout),
                                       flash_layout.layout_size)
                     .value();

        if (!result.has_value())
        {
            state = state_t::ERROR;
            return false;
        }

        addres_to_store_next_command = result.value();
        return true;
    }

    case as_int(code_t::RESTORE_SERIALIZED_COMMANDS):
    {
        bool result = true;

        // if parameter 2 is equal to 1 then we remove already scheduled command
        if (command.parameters[1] == 1)
        {
            command_t remove_command;
            remove_command.parameters[as_int(layout_t::COMMAND_ID)] = as_int(code_t::REMOVE_SCHEDULED_MEASUREMENTS);
            result &= g_scheduler->execute_command(remove_command);
        }

        if (!result)
        {
            return result;
        }

        flash_layout_t FF_layout{};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
        memset(&FF_layout, 0xff, flash_layout_t::layout_size);
#pragma GCC diagnostic pop

        flash_layout_t flash_layout;

        uint32_t read_addres = flash_storage_addres_start;
        while (read_addres <= flash_storage_addres_end)
        {
            result &=
                read_from_flash(read_addres, reinterpret_cast<uint8_t *>(&flash_layout), flash_layout_t::layout_size);

            if (!result)
            {
                return false;
            }

            if (memcmp(&FF_layout, &flash_layout, flash_layout_t::layout_size) == 0)
            {
                return true;
            }

            result &= flash_layout.crc == calculate_crc(flash_layout.command.get_parameters_pointer(),
                                                        flash_layout.command.get_parameters_size());

            if (!result)
            {
                return false;
            }

            flash_layout.command.parameters[as_int(layout_t::COMMAND_ID)] = as_int(code_t::SCHEDULE_MEASUREMENT);
            result &= g_device_modules[as_int(module_id_t::SCHEDULER)]->execute_command(flash_layout.command);

            if (!result)
            {
                return false;
            }

            read_addres += flash_layout_t::layout_size + (8 - flash_layout_t::layout_size % 8);
        }

        if (!result)
        {
            state = state_t::ERROR;
        }

        return result;
    }

    case as_int(code_t::ERASE_FLASH):
    {

        std::optional<std::pair<uint32_t, uint32_t>> flash_pages =
            calculate_pages_to_erase(flash_storage_addres_start, flash_storage_addres_end - 1);

        if (!flash_pages.has_value())
        {
            return false;
        }

        FLASH_EraseInitTypeDef erase;
        erase.TypeErase = FLASH_TYPEERASE_PAGES;
        erase.Page      = flash_pages.value().first;
        erase.NbPages   = flash_pages.value().second;

        uint32_t page_error;

        HAL_FLASH_Unlock();
        HAL_StatusTypeDef erase_status = HAL_FLASHEx_Erase(&erase, &page_error);
        HAL_FLASH_Lock();

        if (erase_status != HAL_OK && page_error != 0xFFFFFFFF)
        {
            state = state_t::ERROR;
            return false;
        }

        uint32_t empty_address = find_addres_to_store_next_command();

        if (empty_address != flash_storage_addres_start)
        {
            state = state_t::ERROR;
            return false;
        }

        addres_to_store_next_command = flash_storage_addres_start;

        return true;
    }

    default:
        return false;
    }

    return false;
}

uint32_t serializer_t::calculate_crc(uint8_t *data_pointer, uint8_t data_size)
{
    if (data_pointer == nullptr || data_size == 0)
    {
        return 0;
    }

    hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;

    return HAL_CRC_Calculate(&hcrc, reinterpret_cast<uint32_t *>(data_pointer), data_size);
}

std::optional<std::pair<uint32_t, uint32_t>> serializer_t::calculate_pages_to_erase(uint32_t start_address,
                                                                                    uint32_t end_address)
{
    if (start_address > end_address)
    {
        return std::nullopt;
    }

    if (start_address < flash_storage_addres_start)
    {
        return std::nullopt;
    }

    if (end_address >= flash_storage_addres_end)
    {
        return std::nullopt;
    }

    // we assume dual bank is dissabled so page size is 2KB
    uint32_t first_page      = (start_address - flash_base) / page_size;
    uint32_t last_page       = (end_address - flash_base) / page_size;
    uint32_t number_of_pages = last_page - first_page + 1;

    return std::make_pair(first_page, number_of_pages);
}

uint32_t serializer_t::find_addres_to_store_next_command()
{
    uint32_t current_addres = flash_storage_addres_start;

    if (current_addres % 8 != 0)
    {
        return 0;
    }

    flash_layout_t FF_layout{};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
    memset(&FF_layout, 0xff, flash_layout_t::layout_size);
#pragma GCC diagnostic pop

    flash_layout_t dummy_reading;

    while (current_addres + flash_layout_t::layout_size < flash_storage_addres_end)
    {
        dummy_reading = *reinterpret_cast<flash_layout_t *>(current_addres);
        if (memcmp(&dummy_reading, &FF_layout, flash_layout_t::layout_size) == 0)
        {
            break;
        }
        current_addres += flash_layout_t::layout_size;
        current_addres += 8 - current_addres % 8;
    }

    if (current_addres + flash_layout_t::layout_size >= flash_storage_addres_end)
        return 0;

    if (current_addres % 8 != 0)
        return 0;

    return current_addres;
}

bool serializer_t::read_from_flash(uint32_t flash_addres, uint8_t *data_buffer, uint8_t buffer_size)
{
    if (data_buffer == nullptr)
    {
        return false;
    }
    memcpy(data_buffer, reinterpret_cast<const uint8_t *>(flash_addres), buffer_size);
    return true;
}

std::optional<uint32_t> serializer_t::write_double_word_to_flash(uint32_t flash_addres, uint64_t value_to_save)
{
    if (flash_addres % 8 != 0)
    {
        return std::nullopt;
    }

    HAL_FLASH_Unlock();
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, flash_addres, value_to_save) != HAL_OK)
    {
        HAL_FLASH_Lock();
        return std::nullopt;
    }
    HAL_FLASH_Lock();

    if (memcmp(reinterpret_cast<uint8_t *>(flash_addres), &value_to_save, sizeof(value_to_save)) != 0)
    {
        return std::nullopt;
    }

    return flash_addres + 8;
}

std::optional<uint32_t> serializer_t::write_memory_to_flash(uint32_t flash_addres, uint8_t *data_pointer,
                                                            uint8_t data_size)
{
    if (flash_addres < flash_storage_addres_start || flash_addres + data_size >= flash_storage_addres_end)
    {
        return std::nullopt;
    }

    if (data_pointer == nullptr || data_size == 0)
    {
        return std::nullopt;
    }

    if (flash_addres % 8 != 0)
    {
        return std::nullopt;
    }

    uint32_t data_offset = 0;

    std::optional<uint32_t> result = flash_addres;
    for (uint8_t i = 0; result.has_value() && data_offset < data_size && i < data_size / 8; i++)
    {

        // nice try but pointer data_pointer have to be alligned
        uint64_t word;
        memcpy(&word, data_pointer + data_offset, sizeof(word));
        result = write_double_word_to_flash(result.value(), word);
        data_offset += 8;
    }

    if (result.has_value() && data_offset < data_size)
    {
        uint64_t word = UINT64_MAX;
        memcpy(&word, data_pointer + data_offset, data_size - data_offset);
        result = write_double_word_to_flash(result.value(), word);
    }

    return result;
}

/*-----------------------------------------------------*/
/* NICE ATTEMPT */
/* but not this time - STM32 internal flash allows only for programming double word only one time per address */
/* in other case FLASH ECC will trigger error and write command will fail*/
/*-----------------------------------------------------*/
/*
bool write_double_word_to_flash(uint32_t flash_addres, uint64_t value_to_save)
{
    if (flash_addres % 8 != 0)
    {
        return false;
    }

    HAL_FLASH_Unlock();
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, flash_addres, value_to_save) != HAL_OK)
    {
        HAL_FLASH_Lock();
        return false;
    }
    HAL_FLASH_Lock();

    if (memcmp(reinterpret_cast<uint8_t *>(flash_addres), &value_to_save, sizeof(value_to_save)) != 0)
    {
        return false;
    }

    return true;
}

bool write_byte_to_flash(uint32_t flash_addres, uint8_t value_to_save)
{
    uint32_t padding_offset  = flash_addres % 8;
    uint32_t alligned_addres = flash_addres - padding_offset;

    // we do not have to compare whole double-word but for dev test is better to keep that
    uint64_t flash_content = *reinterpret_cast<uint64_t *>(alligned_addres);

    // memcpy(reinterpret_cast<uint8_t *>(&flash_content) + padding_offset, &value_to_save, sizeof(value_to_save));
    reinterpret_cast<uint8_t *>(&flash_content)[padding_offset] = value_to_save;

    HAL_FLASH_Unlock();
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, alligned_addres, flash_content) != HAL_OK)
    {
        HAL_FLASH_Lock();
        return false;
    }
    HAL_FLASH_Lock();

    if (memcmp(reinterpret_cast<uint8_t *>(alligned_addres), &flash_content, sizeof(flash_content)) != 0)
    {
        return false;
    }

    return true;
}

bool write_memory_to_flash(uint32_t flash_addres, uint8_t *data_pointer, uint8_t data_size)
{
    if (flash_addres < flash_storage_addres_start || flash_addres + data_size >= flash_storage_addres_end)
    {
        return false;
    }

    if (data_pointer == nullptr || data_size == 0)
    {
        return false;
    }

    uint32_t padding_offset      = flash_addres % 8;
    uint32_t padding_difference  = padding_offset == 0 ? 0 : 8 - padding_offset;
    uint32_t data_pointer_offset = 0;

    bool result = true;

    for (uint8_t i = 0; i < padding_difference && result && data_pointer_offset < data_size; i++)
    {
        result &= write_byte_to_flash(flash_addres + data_pointer_offset, *(data_pointer + data_pointer_offset));
        data_pointer_offset++;
    }

    if (!result)
    {
        return false;
    }
    if (data_pointer_offset == data_size)
    {
        return true;
    }

    for (uint8_t i = 0; i < (data_size - padding_difference) / 8 && result && data_pointer_offset < data_size; i++)
    {

        nie można tak robić, co jak data_pointer nie będzie alligned

        result &= write_double_word_to_flash(flash_addres + data_pointer_offset,
                                             *reinterpret_cast<uint64_t *>(data_pointer + data_pointer_offset));
        data_pointer_offset += 8;
    }

    if (!result)
    {
        return false;
    }
    if (data_pointer_offset == data_size)
    {
        return true;
    }

    while (data_pointer_offset < data_size && result)
    {
        result &= write_byte_to_flash(flash_addres + data_pointer_offset, *(data_pointer + data_pointer_offset));
        data_pointer_offset++;
    }

    if (!result)
    {
        return false;
    }
    if (data_pointer_offset == data_size)
    {
        return true;
    }

    return false;
}

uint32_t serializer_t::find_addres_to_store_next_command()
{
    uint32_t current_addres = flash_storage_addres_start;
    flash_layout_t FF_layout{};
    memset(&FF_layout, 0xff, flash_layout_t::layout_size);
    flash_layout_t dummy_reading;

    while (current_addres + flash_layout_t::layout_size < flash_storage_addres_end)
    {

        // memcpy(&dummy_reading, current_addres, flash_layout_t::layout_size);
        dummy_reading = *reinterpret_cast<flash_layout_t *>(current_addres);
        if (memcmp(&dummy_reading, &FF_layout, flash_layout_t::layout_size) == 0)
        {
            return current_addres;
        }
        current_addres += flash_layout_t::layout_size;
    }
    return 0;
}
*/
