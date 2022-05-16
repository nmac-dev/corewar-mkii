#pragma once

using UUID = int;

// On each call a unique number is created as a new warrior ID
inline static UUID create_uuid() 
{
    static UUID unique_number;
    return    ++unique_number;
}
