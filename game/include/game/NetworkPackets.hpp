#pragma once

#include <cstdint>

enum Requests : uint8_t
{
    CONNECTION_REQUEST,
    CONNECTION_ACCEPTED,
    DISCONNECTION_REQUEST,
};
