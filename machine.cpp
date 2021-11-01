#include "machine.h"
#include "sp.h"

Machine::Machine(int n_packets, int machine_index, int n_machines) : 
    n_packets_to_send_(n_packets),
    id_(machine_index),
    n_machines_(n_machines) {
    
    finished_ = std::vector<bool>(n_machines, false);
    last_sent_ = 0;
}

void Machine::start() {

    printf("Starting\n");
    ret = SP_connect(spread_name_, user_, 0, 1, &Mbox_, group_);
	if( ret != ACCEPT_SESSION ) 
	{
		printf("unable to join spread\n");
		return;
	 }
	 printf("User: connected to %s with private group %s\n", spread_name_, group_);


}

void Machine::send_packet_burst() {
    for (int i = last_sent_ + 1; i < last_sent_ + PACKET_BURST_SIZE; i++) {
        send_packet(i);
    }
}

void Machine::send_packet(int index) {
    Message& packet = message_buf_;
    packet.message_index = index;
    packet.process_index = id_;
    packet.magic_number = generate_magic_number();
    SP_multicast(Mbox_, AGREED_MESS, group_, 2, sizeof(packet), reinterpret_cast<const char *>(&packet));
}

uint32_t Machine::generate_magic_number()
{
    return rng_dst_(generator_);
}