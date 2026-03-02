#include "serializer.h"

serializer_t *g_serializer = nullptr;

bool serializer_t::init()
{
    state = state_t::READY;
    return true;
}