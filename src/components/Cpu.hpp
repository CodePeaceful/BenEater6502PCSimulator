#pragma once

class Cpu
{
private:
// pin references
    unsigned char& data; // D7 - D0
    unsigned short& address; // A15 - A0
    bool& VPB; // Vector Pull
    const bool& RDY; // ready: run on high
    //bool& PHI1O; // not implemented
    const bool& IRQB; // interupt: on low
    bool& MLB; // memory lock
    const bool& NMIB; // non maskable interupt: on low
    bool& SYNC; //??
    bool& RWB; // read or write: high = read, low = write
    const bool& BE; // bus enable
    const bool& SOB; // set Overflow dont use set high
    //bool& PHI2O; // not implemented

// internals
    unsigned short programCounter;
    unsigned char IR; // instruction register
    unsigned char TCU; // timing control unit

// helpers unclear internal representation
    unsigned char resetTimer;
    unsigned char instructionBufferLow;
    unsigned char instructionBufferHigh;
    void fetch();

// const times
    const unsigned char implied = 2;

public:
    Cpu(unsigned char& _data, unsigned short& _address, bool& _VPB, const bool& _RDY, const bool& _IRQB, bool& _MLB,
        const bool& _NMIB, bool& _SYNC, bool& _RWB, const bool& _BE, const bool& _SOB);
    void cycle();
    void reset();
};