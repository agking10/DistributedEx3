#include "machine.h"
#include "sp.h"

#define MESS_BUF_LEN 100000

Machine::Machine(int n_packets,
                 int machine_index,
                 int n_machines)
    : n_packets_to_send_(n_packets),
      id_(machine_index),
      n_machines_(n_machines),
      n_finished_(0)
{

    finished_ = std::vector<bool>(n_machines, false);
    last_sent_ = 0;
    done_sending_ = false;
}

void Machine::start()
{

    //printf("Starting\n");

    ret = SP_connect(spread_name_, user_, 0, 1, &Mbox_, private_group_);
    if (ret != ACCEPT_SESSION)
    {
        //printf("unable to join spread\n");
        return;
    }
    //printf("User: connected to %s with private group %s\n", spread_name_, group_);
    //fflush(0);
    std::string fname =  "../../../../tmp/" + std::to_string(id_) + "y.txt"; //"../../../../tmp/" +
    fd_ = fopen(fname.c_str(), "w");
    //printf("here\n");
    fflush(0);

    join_and_wait();
    start_protocol();
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
    membership_info memb_info;

    ret = SP_receive(Mbox_, &service_type, sender, 100, &num_groups, target_groups,
                     &mess_type, &endian_mismatch, sizeof(mess), mess);
    if (ret < 0)
    {
        if ((ret == GROUPS_TOO_SHORT) || (ret == BUFFER_TOO_SHORT))
        {
            service_type = DROP_RECV;
            //printf("\n========Buffers or Groups too Short=======\n");
            ret = SP_receive(Mbox_, &service_type, sender, n_machines_, &num_groups, target_groups,
                             &mess_type, &endian_mismatch, sizeof(mess), mess);
        }
        if (ret < 0)
        {
            SP_error(ret);
            //printf("\n============================\n");
            //printf("\nBye.\n");
        }
        exit(0);
    }
    if (Is_membership_mess(service_type))
    {
        ret = SP_get_memb_info(mess, service_type, &memb_info);
        if (ret < 0)
        {
            //printf("BUG: membership message does not have valid body\n");
            SP_error(ret);
            exit(1);
        }
        if (Is_reg_memb_mess(service_type))
        {
            n_joined_ = num_groups;
        }
    }
}

void Machine::start_protocol()
{
    auto start_time = std::chrono::high_resolution_clock::now();
    if (n_packets_to_send_ == 0)
    {
        last_sent_ = -1;
        done_sending_ = true;
        send_packet(-1);
    }
    while (n_finished_ < n_machines_)
    {
        if (!done_sending_)
        {
            send_packet_burst();
            while (!receive_packet())
                continue;
        }
        else
        {
            receive_packet();
        }
    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<
        std::chrono::microseconds>(stop - start_time);
    std::cout << "Duration: " << duration.count() << " us" << std::endl;
    fclose(fd_);
    printf("All finished!\n");
}

bool Machine::receive_packet()
{
    int ret;
    char sender[MAX_GROUP_NAME];
    char target_groups[MAX_GROUPS][MAX_GROUP_NAME];
    int service_type;
    int num_groups;
    int16 mess_type;
    int endian_mismatch;
    ret = SP_receive(Mbox_, &service_type, sender, n_machines_, &num_groups, target_groups,
                     &mess_type, &endian_mismatch, sizeof(message_buf_), reinterpret_cast<char *>(&message_buf_));
    if (ret < 0) {
        SP_error(ret);
        exit(1);
    }
    //printf("received\n");
    //fflush(0);
    int sender_process = message_buf_.process_index;
    int msg_index = message_buf_.message_index;
    deliver_packet(message_buf_);
    if (msg_index == -1)
    {
        finished_[sender_process - 1] = true;
        printf("Sender %d finished\n", sender_process);
        n_finished_++;
    }
    if (sender_process == id_ && msg_index == last_sent_)
        return true;
    return false;
}

void Machine::join_group()
{
    int ret = SP_join(Mbox_, group_);
    if (ret < 0)
        SP_error(ret);
}

void Machine::send_packet_burst()
{
    for (int i = last_sent_ + 1; i < last_sent_ + PACKET_BURST_SIZE; i++)
    {
        if (i == n_packets_to_send_)
        {
            send_packet(-1);
            last_sent_ = -1;
            done_sending_ = true;
            return;
        }
        if (i % 10000 == 0)
        {
            printf("Sent packet %d\n", i);
        }
        send_packet(i);
    }
    last_sent_ = last_sent_ + PACKET_BURST_SIZE - 1;
}

void Machine::send_packet(int index)
{
    Message &packet = message_buf_;
    packet.message_index = index;
    packet.process_index = id_;
    packet.magic_number = generate_magic_number();
    SP_multicast(Mbox_, SAFE_MESS, group_, 2, sizeof(packet), reinterpret_cast<const char *>(&packet));
    //printf("sending packet %d\n", index);
    // fflush(0);
}

uint32_t Machine::generate_magic_number()
{
    return rng_dst_(generator_);
}

void Machine::deliver_packet(Message msg)
{
    fprintf(fd_, "%2d, %8d, %8d\n", msg.process_index, msg.process_index, msg.magic_number);
}