#pragma once

#include "net_include.h"
#include "messages.h"
#include <random>
#include <vector>
#include "sp.h"

#define MAX_GROUPS 30
class Machine
{
    using RNG = std::mt19937;

public:
    Machine(int n_packets
        , int machine_index
        , int n_machines
    );

    void start();
    void join_and_wait();
    void join_group();
    void receive_membership_message();
    void start_protocol();

private:
    void send_packet_burst();
    void send_packet(int index);
    uint32_t generate_magic_number();
    bool receive_packet();
    void deliver_packet(Message);
    std::vector<bool> finished_;
    mailbox Mbox_;
    FILE * fd_;
    int n_joined_;
    int n_packets_to_send_;
    int id_;
    int n_machines_;
    int n_finished_;
    bool done_sending_;
    int last_sent_;
    int ret;
    char group_ [MAX_GROUP_NAME]= "ypeng22_group";
    char private_group_[MAX_GROUP_NAME];
    char spread_name_[80];
    char user_[80];
    RNG generator_;
    std::uniform_int_distribution<uint32_t> rng_dst_;
    Message message_buf_;
};
