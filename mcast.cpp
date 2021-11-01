#include <iostream>
#include "machine.h"

int main(int argc, char* argv[])
{
    if (argc < 4)
    {
        std::cout << "Usage: <num messages> <process index> <num machines>"
        << std::endl;
    }

    int n_packets = std::stoi(argv[1]);
    int machine_index = std::stoi(argv[2]);
    int n_machines = std::stoi(argv[3]);

    Machine m(
        n_packets,
        machine_index,
        n_machines
    );

    m.start();

    return 0;
}