#include "components/Cpu.hpp"

int main() {
    unsigned char data = 0xEA;
    unsigned short address = 0;
    bool VPB = true; // Vector Pull
    bool RDY = true; // ready: run on high
    bool IRQB = true; // interupt: on low
    bool MLB = true; // memory lock
    bool NMIB = true; // non maskable interupt: on low
    bool SYNC = true; //??
    bool RWB = true; // read or write: high = read, low = write
    bool BE = true; // bus enable
    bool SOB = true;
    Cpu cpu = Cpu(data, address, VPB, RDY, IRQB, MLB, NMIB, SYNC, RWB, BE, SOB);
    cpu.reset();
    while (true) {
        cpu.cycle();
    }
}