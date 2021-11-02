#include "machine.h"
#include "sp.h"

#define MESS_BUF_LEN 10000

Machine::Machine(int n_packets, int machine_index, int n_machines) : 
    n_packets_to_send_(n_packets),
    id_(machine_index),
    n_machines_(n_machines),
    n_finished_(0) {
    
    finished_ = std::vector<bool>(n_machines, false);
    last_sent_ = 0;
}

void Machine::start() {

    printf("Starting\n");

    ret = SP_connect(spread_name_, user_, 0, 1, &Mbox_, private_group_);
	if( ret != ACCEPT_SESSION ) 
	{
		printf("unable to join spread\n");
		return;
	 }
	 printf("User: connected to %s with private group %s\n", spread_name_, group_);

    join_and_wait ();
    start_protocol ();
}

void Machine::join_and_wait()
{
    join_group();
    while (n_joined_ < n_machines_)
    {
        receive_membership_message();
    }
}

void Machine::receive_membership_message()
{
    int ret;
    char mess[MESS_BUF_LEN];
    char sender[MAX_GROUP_NAME];
    char target_groups[MAX_GROUPS][MAX_GROUP_NAME];
    int service_type;
    int num_groups;
    int16 mess_type;
    int endian_mismatch;
    membership_info  memb_info;
    
    ret = SP_receive(Mbox_, &service_type, sender, 100, &num_groups, target_groups,
        &mess_type, &endian_mismatch, sizeof(mess), mess);
    if (ret < 0)
    {
        if ( (ret == GROUPS_TOO_SHORT) || (ret == BUFFER_TOO_SHORT) ) 
        {
            service_type = DROP_RECV;
            printf("\n========Buffers or Groups too Short=======\n");
            ret = SP_receive(Mbox_, &service_type, sender, n_machines_, &num_groups, target_groups, 
                                &mess_type, &endian_mismatch, sizeof(mess), mess);
        }
        if (ret < 0)
        {
            SP_error(ret);
            printf("\n============================\n");
            printf("\nBye.\n");
        }
        exit(0);
    }
    if ( Is_membership_mess(service_type))
    {
        ret = SP_get_memb_info(mess, service_type, &memb_info);
        if (ret < 0) 
        {
            printf("BUG: membership message does not have valid body\n");
            SP_error( ret );
            exit( 1 );
        }
        if (Is_reg_memb_mess(service_type))
        {
            printf("Received REGULAR membership for group %s with %d members, where I am member %d:\n",
				sender, num_groups, mess_type );
            n_joined_ = num_groups;
        }
    }    
}

void Machine::start_protocol()
{
    printf("Started!");
    while (n_finished_ < n_machines_)
    {
        send_new_packet();
        for (int i = n_machines_ - n_finished_; i >= 0; i--)
        {
            receive_packet();
        }
    }
}

void Machine::join_group()
{
    int ret = SP_join (Mbox_, group_);
    if (ret < 0) SP_error(ret);
}

void Machine::send_packet_burst() {
    for (int i = last_sent_ + 1; i < last_sent_ + PACKET_BURST_SIZE; i++) {
        send_packet(i);
    }
}

void Machine::send_new_packet()
{
    if 
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