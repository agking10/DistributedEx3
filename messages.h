#pragma once

#include "net_include.h"

struct Message
{
    int process_index;
    int message_index;
    int magic_number; // random number to verify packets are the same
    char data[MAX_MESS_LEN];
};