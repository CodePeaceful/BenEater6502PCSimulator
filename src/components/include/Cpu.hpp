#pragma once

class Cpu
{
private:
    // pin references
    unsigned char& data; // D7 - D0
    unsigned short& address; // A15 - A0
    bool& VPB; // Vector Pull
    bool& RDY; // ready: run on high
    const bool& IRQB; // interupt: on low
    bool& MLB; // memory lock
    const bool& NMIB; // non maskable interupt: on low
    bool& SYNC; //??
    bool& RWB; // read or write: high = read, low = write
    const bool& BE; // bus enable
    const bool& SOB; // set Overflow dont use set high
    const bool& PHI2;
    bool& PHI1O;
    bool& PHI2O;

    // internals
    unsigned short programCounter;
    unsigned char IR; // instruction register
    unsigned char TCU; // timing control unit
    unsigned char A; // A register
    unsigned char X;
    unsigned char Y;
    unsigned char stackPointer;
    unsigned char prozessorStatus;

    unsigned char dataBuffer;
    unsigned short addressBuffer;
    bool readWriteBuffer;
    bool lastClockState{ true };

    // helpers unclear internal representation
    bool interuptRequested{ false };
    bool inbreak{ false };
    unsigned char resetTimer;
    unsigned char instructionBufferLow;
    unsigned char instructionBufferHigh;

    unsigned char writeBackCounter;

    void fetch();
    void toBus();
    void handleInterupt();
    void negativeZeroCheck(unsigned char value);

    // executions
    void addWithCarry(unsigned char value);
    void addWithCarry(bool(Cpu::* address)());
    void subtractWithCarry(unsigned char value);
    void subtractWithCarry(bool(Cpu::* address)());
    void orA(bool(Cpu::* address)());
    void andA(bool(Cpu::* address)());
    void xorA(bool(Cpu::* address)());
    void load(unsigned char& cpuRegister, bool(Cpu::* address)());
    void store(unsigned char value, bool(Cpu::* address)());
    void bitTest(unsigned char second);
    void bitTest(bool(Cpu::* address)());
    void compare(unsigned char first, unsigned char second);
    void compare(unsigned char first, bool(Cpu::* address)());
    void testAndSetMemoryBit(bool(Cpu::* address)());
    void testAndResetMemoryBit(bool(Cpu::* address)());
    void rotateRight(bool(Cpu::* address)());
    void rotateLeft(bool(Cpu::* address)());
    void shiftRight(bool(Cpu::* address)());
    void shiftLeft(bool(Cpu::* address)());
    void increment(bool(Cpu::* address)());
    void decrement(bool(Cpu::* address)());

    void resetMemoryBit(unsigned char bitId);
    void setMemoryBit(unsigned char bitId);
    void branchOnBitReset(unsigned char bitId);
    void branchOnBitSet(unsigned char bitId);

    void branchIf(bool condition);

    void push(unsigned char data);  // after push cpu is in write mode
    void pull();    // to dataBuffer

    // addressing
    bool zeroPage();                // done in tcu 2 low
    bool zeroPageIndirect();        // done in tcu 4 low
    bool zeroPageWithXIndirect();   // done in tcu 5 low
    bool zeroPageIndirectWithY();   // done in tcu 4 low
    bool zeroPageWithX();           // done in tcu 3 low
    bool zeroPageWithY();           // done in tcu 3 low
    bool absoluteIndirect();        // done in tcu 5 low
    bool absolute();                // done in tcu 3 low
    bool absoluteWithX();           // done in tcu 3 low
    bool absoluteWithY();           // done in tcu 3 low

    // operations
    void breakRequest();                // 0x00
    void orWithZeroPageWithXIndirect(); // 0x01
    void testAndSetMemoryBitZeroPage(); // 0x04
    void orWithZeroPage();              // 0x05
    void shiftLeftZeroPage();           // 0x06
    void resetMemoryBit0();             // 0x07
    void pushProzessorStatus();         // 0x08
    void orWithImmediate();             // 0x09
    void shiftLeftA();                  // 0x0a
    void testAndSetMemoryBitAbsolute(); // 0x0c
    void orWithAbsolute();              // 0x0d
    void shiftLeftAbsolute();           // 0x0e
    void branchOnBitReset0();           // 0x0f
    void branchIfResultPlus();          // 0x10
    void orWithZeroPageIndirectWithY(); // 0x11
    void orWithZeroPageIndirect();      // 0x12
    void testAndResetMemoryBitZeroPage();//0x14
    void orWithZeroPageWithX();         // 0x15
    void shiftLeftZeroPageWithX();      // 0x16
    void resetMemoryBit1();             // 0x17
    void clearCarry();                  // 0x18
    void orWithAbsoluteWithY();         // 0x19
    void incrementA();                  // 0x1a
    void testAndResetMemoryBitAbsolute();//0x1c
    void orWithAbsoluteWithX();         // 0x1d
    void shiftLeftAbsoluteWithX();      // 0x1e
    void branchOnBitReset1();           // 0x1f
    void jumpToSubroutine();            // 0x20
    void andWithZeroPageWithXIndirect();// 0x21
    void bitTestWithZeroPage();         // 0x24
    void andWithZeroPage();             // 0x25
    void rotateLeftZeroPage();          // 0x26
    void resetMemoryBit2();             // 0x27
    void pullProcessorStatus();         // 0x28
    void andWithImediate();             // 0x29
    void rotateLeftA();                 // 0x2a
    void bitTestWithAbsolute();         // 0x2c
    void andWithAbsolute();             // 0x2d
    void rotateLeftAbsolute();          // 0x2e
    void branchOnBitReset2();           // 0x2f
    void branchIfResultMinus();         // 0x30
    void andWithZeroPageIndirectWithY();// 0x31
    void andWithZeroPageIndirect();     // 0x32
    void bitTestWithZeroPageWithX();    // 0x34
    void andWithZeroPageWithX();        // 0x35
    void rotateLeftZeroPageWithX();     // 0x36
    void resetMemoryBit3();             // 0x37
    void setCarry();                    // 0x38
    void andWithAbsoluteWithY();        // 0x39
    void decrementA();                  // 0x3a
    void bitTestWithAbsoluteWithX();    // 0x3c
    void andWithAbsoluteWithX();        // 0x3d
    void rotateLeftAbsoluteWithX();     // 0x3e
    void branchOnBitReset3();           // 0x3f
    void returnFromInterupt();          // 0x40
    void xorWithZeroPageWithXIndirect();// 0x41
    void xorZeroPage();                 // 0x45
    void shiftRightZeroPage();          // 0x46
    void resetMemoryBit4();             // 0x47
    void pushA();                       // 0x48
    void xorImediate();                 // 0x49
    void shiftRightA();                 // 0x4a
    void jumpAbsolute();                // 0x4c
    void xorWithAbsolute();             // 0x4d
    void shiftRightAbsolute();          // 0x4e
    void branchOnBitReset4();           // 0x4f
    void branchIfOverflowClear();       // 0x50
    void xorWithZeroPageIndirectWithY();// 0x51
    void xorWithZeroPageIndirect();     // 0x52
    void xorWithZeroPageWithX();        // 0x55
    void shiftRightZeroPageWithX();     // 0x56
    void resetMemoryBit5();             // 0x57
    void clearInteruptDisableFlag();    // 0x58
    void xorWithAbsoluteWithY();        // 0x59
    void pushY();                       // 0x5a
    void xorWithAbsoluteWithX();        // 0x5d
    void shiftRightAbsoluteWithX();     // 0x5e
    void branchOnBitReset5();           // 0x5f
    void returnFromSubroutine();        // 0x60
    void addWithZeroPageWithXIndirect();// 0x61
    void store0AtZeroPage();            // 0x64
    void addWithZeroPage();             // 0x65
    void rotateRightZeroPage();         // 0x66
    void resetMemoryBit6();             // 0x67
    void pullA();                       // 0x68
    void addWithImediate();             // 0x69
    void rotateRightA();                // 0x6a
    void jumpAbsoluteIndirect();        // 0x6c
    void addWithAbsolute();             // 0x6d
    void rotateRightAbsolute();         // 0x6e
    void branchOnBitReset6();           // 0x6f
    void branchIfOverflowSet();         // 0x70
    void addWithZeroPageIndirectWithY();// 0x71
    void addWithZeroPageIndirect();     // 0x72
    void store0AtZeroPageWithX();       // 0x74
    void addWithZeroPageWithX();        // 0x75
    void rotateRightZeroPageWithX();    // 0x76
    void resetMemoryBit7();             // 0x77
    void setInteruptDisable();          // 0x78
    void addWithAbsoluteWithY();        // 0x79
    void pullY();                       // 0x7a
    void jumpAbsoluteWithXIndirect();   // 0x7c
    void addWithAbsoluteWithX();        // 0x7d
    void rotateRightAbsoluteWithX();    // 0x7e
    void branchOnBitReset7();           // 0x7f
    void branchAlways();                // 0x80
    void storeAAtZeroPageWithXIndirect();//0x81
    void storeYAtZeroPage();            // 0x84
    void storeAAtZeroPage();            // 0x85
    void storeXAtZeroPage();            // 0x86
    void setMemoryBit0();               // 0x87
    void decrementY();                  // 0x88
    void bitTestWithImediate();         // 0x89
    void transferXtoA();                // 0x8a
    void storeYAtAbsolute();            // 0x8c
    void storeAAtAbsolute();            // 0x8d
    void storeXAtAbsolute();            // 0x8e
    void branchOnBitSet0();             // 0x8f
    void branchIfCarryClear();          // 0x90
    void storeAAtZeroPageIndirectWithY();//0x91
    void storeAAtZeroPageIndirect();    // 0x92
    void storeYAtZeroPageWithX();       // 0x94
    void storeAAtZeroPageWithX();       // 0x95
    void storeXAtZeroPageWithY();       // 0x96
    void setMemoryBit1();               // 0x97
    void transferYToA();                // 0x98
    void storeAAtAbsoluteWithY();       // 0x99
    void transferXToStackpointer();     // 0x9a
    void store0AtAbsolute();            // 0x9c
    void storeAAtAbsoluteWithX();       // 0x9d
    void store0AtAbsoluteWithX();       // 0x9e
    void branchOnBitSet1();             // 0x9f
    void loadYImediate();               // 0xa0
    void loadAZeroPageWithXIndirect();  // 0xa1
    void loadXImidiate();               // 0xa2
    void loadYZeroPage();               // 0xa4
    void loadAZeroPage();               // 0xa5
    void loadXZeroPage();               // 0xa6
    void setMemoryBit2();               // 0xa7
    void transferAToY();                // 0xa8
    void loadAImidiate();               // 0xa9
    void transferAToX();                // 0xaa
    void loadYAbsolute();               // 0xac
    void loadAAbsolute();               // 0xad
    void loadXAbsolute();               // 0xae
    void branchOnBitSet2();             // 0xaf
    void branchIfCarrySet();            // 0xb0
    void loadAZeroPageIndirectWithY();  // 0xb1
    void loadAZeroPageIndirect();       // 0xb2
    void loadYZeroPageWithX();          // 0xb4
    void loadAZeroPageWithX();          // 0xb5
    void loadXZeroPageWithY();          // 0xb6
    void setMemoryBit3();               // 0xb7
    void clearOverflow();               // 0xb8
    void loadAAbsoluteWithY();          // 0xb9
    void transferStackpointerToX();     // 0xba
    void loadYAbsoluteWithX();          // 0xbc
    void loadAAbsoluteWithX();          // 0xbd
    void loadXAbsoluteWithY();          // 0xbe
    void branchOnBitSet3();             // 0xbf
    void compareYWithImediate();        // 0xc0
    void compareAWithZeroPageWithXIndirect();//0xc1
    void compareYWithZeroPage();        // 0xc4
    void compareAWithZeroPage();        // 0xc5
    void decrementZeroPage();           // 0xc6
    void setMemoryBit4();               // 0xc7
    void incrementY();                  // 0xc8
    void compareAWithImediate();        // 0xc9
    void decrementX();                  // 0xca
    void waitForInterupt();             // 0xcb
    void compareYWithAbsolute();        // 0xcc
    void compareAWithAbsolute();        // 0xcd
    void decrementAbsolute();           // 0xce
    void branchOnBitSet4();             // 0xcf
    void branchNotEqual();              // 0xd0
    void compareAWithZeroPageIndirectWithY();//0xd1
    void compareAWithZeroPageIndirect();// 0xd2
    void compareAWithZeroPageWithX();   // 0xd5
    void decrementZeroPageWithX();      // 0xd6
    void setMemoryBit5();               // 0xd7
    void clearDecimalMode();            // 0xd8
    void compareAWithAbsoluteWithY();   // 0xd9
    void pushX();                       // 0xda
    void stopMode();                    // 0xdb
    void compareAWithAbsoluteWithX();   // 0xdd
    void decrementAbsoluteWithX();      // 0xde
    void branchOnBitSet5();             // 0xdf
    void compareXWithImediate();        // 0xe0
    void subtractWithZeroPageWithXIndirect();//0xe1
    void compareXWithZeroPage();        // 0xe4
    void subtractWithZeroPage();        // 0xe5
    void incrementZeroPage();           // 0xe6
    void setMemoryBit6();               // 0xe7
    void incrementX();                  // 0xe8
    void subtractWithImediate();        // 0xe9
    void noop();                        // 0xea
    void compareXWithAbsolute();        // 0xec
    void subtractWithAbsolute();        // 0xed
    void incrementAbsolute();           // 0xee
    void branchOnBitSet6();             // 0xef
    void branchIfEqual();               // 0xf0
    void subtractWithZeroPageIndirectWithY();//0xf1
    void subtractWithZeroPageIndirect();// 0xf2
    void subtractWithZeroPageWithX();   // 0xf5
    void incrementZeroPageWithX();      // 0xf6
    void setMemoryBit7();               // 0xf7
    void setDecimalMode();              // 0xf8
    void subtractWithAbsoluteWithY();   // 0xf9
    void pullX();                       // 0xfa
    void subtractWithAbsoluteWithX();   // 0xfd
    void incrementAbsoluteWithX();      // 0xfe
    void branchOnBitSet7();             // 0xff

public:
    Cpu(unsigned char& _data, unsigned short& _address, bool& _VPB, bool& _RDY, const bool& _IRQB, bool& _MLB, const bool& _NMIB,
        bool& _SYNC, bool& _RWB, const bool& _BE, const bool& _SOB, const bool& _PHI2, bool& _PHI1O, bool& _PHI2O);
    void cycle();
    void reset();
};
