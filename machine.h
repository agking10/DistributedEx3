#pragma once

#include "net_include.h"
#include "messages.h"
#include <random>
#include <vector>
#include "sp.h"

class Machine
{
    using RNG = std::mt19937;

public:
    Machine(int n_packets
        , int machine_index
        , int n_machines
    );

    void start();

private:
    void send_packet_burst();
    void send_packet(int index);
    uint32_t generate_magic_number();
    std::vector<bool> finished_;
    mailbox Mbox_;
    int n_packets_to_send_;
    int id_;
    int n_machines_;
    int last_sent_;
    int ret;
    char group_ [80]= "ypeng22_group";
    const char* spread_name_ = "ypeng22_spread";
    const char* user_ = "ypeng22";
    RNG generator_;
    std::uniform_int_distribution<uint32_t> rng_dst_;
    Message message_buf_;
};
