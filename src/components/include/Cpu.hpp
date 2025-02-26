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
    bool lastClockState{true};

    // helpers unclear internal representation
    bool interuptRequested{false};
    bool interuptDemanded{false};
    bool NMIB_Before{true};
    bool inbreak{false};
    unsigned char resetTimer;
    unsigned char instructionBufferLow;
    unsigned char instructionBufferHigh;

    unsigned char writeBackCounter;

    void fetch()noexcept;
    void toBus()noexcept;
    void handleInteruptRequest()noexcept;
    void handleNonMaskebleInterupt()noexcept;
    void negativeZeroCheck(unsigned char value)noexcept;

    // executions
    void addWithCarry(unsigned char value)noexcept;
    void addWithCarry(bool(Cpu::* address)())noexcept;
    void subtractWithCarry(unsigned char value)noexcept;
    void subtractWithCarry(bool(Cpu::* address)())noexcept;
    void orA(bool(Cpu::* address)())noexcept;
    void andA(bool(Cpu::* address)())noexcept;
    void xorA(bool(Cpu::* address)())noexcept;
    void load(unsigned char& cpuRegister, bool(Cpu::* address)())noexcept;
    void store(unsigned char value, bool(Cpu::* address)())noexcept;
    void bitTest(unsigned char second)noexcept;
    void bitTest(bool(Cpu::* address)())noexcept;
    void compare(unsigned char first, unsigned char second)noexcept;
    void compare(unsigned char first, bool(Cpu::* address)())noexcept;
    void testAndSetMemoryBit(bool(Cpu::* address)())noexcept;
    void testAndResetMemoryBit(bool(Cpu::* address)())noexcept;
    void rotateRight(bool(Cpu::* address)())noexcept;
    void rotateLeft(bool(Cpu::* address)())noexcept;
    void shiftRight(bool(Cpu::* address)())noexcept;
    void shiftLeft(bool(Cpu::* address)())noexcept;
    void increment(bool(Cpu::* address)())noexcept;
    void decrement(bool(Cpu::* address)())noexcept;

    void resetMemoryBit(unsigned char bitId)noexcept;
    void setMemoryBit(unsigned char bitId)noexcept;
    void branchOnBitReset(unsigned char bitId)noexcept;
    void branchOnBitSet(unsigned char bitId)noexcept;

    void branchIf(bool condition)noexcept;

    void push(unsigned char data)noexcept;  // after push cpu is in write mode
    void pull()noexcept;    // to dataBuffer

    // addressing
    bool zeroPage()noexcept;                // done in tcu 2 low
    bool zeroPageIndirect()noexcept;        // done in tcu 4 low
    bool zeroPageWithXIndirect()noexcept;   // done in tcu 5 low
    bool zeroPageIndirectWithY()noexcept;   // done in tcu 4 low
    bool zeroPageWithX()noexcept;           // done in tcu 3 low
    bool zeroPageWithY()noexcept;           // done in tcu 3 low
    bool absoluteIndirect()noexcept;        // done in tcu 5 low
    bool absolute()noexcept;                // done in tcu 3 low
    bool absoluteWithX()noexcept;           // done in tcu 3 low
    bool absoluteWithY()noexcept;           // done in tcu 3 low

    // operations
    void breakRequest()noexcept;                // 0x00
    void orWithZeroPageWithXIndirect()noexcept; // 0x01
    void testAndSetMemoryBitZeroPage()noexcept; // 0x04
    void orWithZeroPage()noexcept;              // 0x05
    void shiftLeftZeroPage()noexcept;           // 0x06
    void resetMemoryBit0()noexcept;             // 0x07
    void pushProzessorStatus()noexcept;         // 0x08
    void orWithImmediate()noexcept;             // 0x09
    void shiftLeftA()noexcept;                  // 0x0a
    void testAndSetMemoryBitAbsolute()noexcept; // 0x0c
    void orWithAbsolute()noexcept;              // 0x0d
    void shiftLeftAbsolute()noexcept;           // 0x0e
    void branchOnBitReset0()noexcept;           // 0x0f
    void branchIfResultPlus()noexcept;          // 0x10
    void orWithZeroPageIndirectWithY()noexcept; // 0x11
    void orWithZeroPageIndirect()noexcept;      // 0x12
    void testAndResetMemoryBitZeroPage()noexcept;//0x14
    void orWithZeroPageWithX()noexcept;         // 0x15
    void shiftLeftZeroPageWithX()noexcept;      // 0x16
    void resetMemoryBit1()noexcept;             // 0x17
    void clearCarry()noexcept;                  // 0x18
    void orWithAbsoluteWithY()noexcept;         // 0x19
    void incrementA()noexcept;                  // 0x1a
    void testAndResetMemoryBitAbsolute()noexcept;//0x1c
    void orWithAbsoluteWithX()noexcept;         // 0x1d
    void shiftLeftAbsoluteWithX()noexcept;      // 0x1e
    void branchOnBitReset1()noexcept;           // 0x1f
    void jumpToSubroutine()noexcept;            // 0x20
    void andWithZeroPageWithXIndirect()noexcept;// 0x21
    void bitTestWithZeroPage()noexcept;         // 0x24
    void andWithZeroPage()noexcept;             // 0x25
    void rotateLeftZeroPage()noexcept;          // 0x26
    void resetMemoryBit2()noexcept;             // 0x27
    void pullProcessorStatus()noexcept;         // 0x28
    void andWithImediate()noexcept;             // 0x29
    void rotateLeftA()noexcept;                 // 0x2a
    void bitTestWithAbsolute()noexcept;         // 0x2c
    void andWithAbsolute()noexcept;             // 0x2d
    void rotateLeftAbsolute()noexcept;          // 0x2e
    void branchOnBitReset2()noexcept;           // 0x2f
    void branchIfResultMinus()noexcept;         // 0x30
    void andWithZeroPageIndirectWithY()noexcept;// 0x31
    void andWithZeroPageIndirect()noexcept;     // 0x32
    void bitTestWithZeroPageWithX()noexcept;    // 0x34
    void andWithZeroPageWithX()noexcept;        // 0x35
    void rotateLeftZeroPageWithX()noexcept;     // 0x36
    void resetMemoryBit3()noexcept;             // 0x37
    void setCarry()noexcept;                    // 0x38
    void andWithAbsoluteWithY()noexcept;        // 0x39
    void decrementA()noexcept;                  // 0x3a
    void bitTestWithAbsoluteWithX()noexcept;    // 0x3c
    void andWithAbsoluteWithX()noexcept;        // 0x3d
    void rotateLeftAbsoluteWithX()noexcept;     // 0x3e
    void branchOnBitReset3()noexcept;           // 0x3f
    void returnFromInterupt()noexcept;          // 0x40
    void xorWithZeroPageWithXIndirect()noexcept;// 0x41
    void xorZeroPage()noexcept;                 // 0x45
    void shiftRightZeroPage()noexcept;          // 0x46
    void resetMemoryBit4()noexcept;             // 0x47
    void pushA()noexcept;                       // 0x48
    void xorImediate()noexcept;                 // 0x49
    void shiftRightA()noexcept;                 // 0x4a
    void jumpAbsolute()noexcept;                // 0x4c
    void xorWithAbsolute()noexcept;             // 0x4d
    void shiftRightAbsolute()noexcept;          // 0x4e
    void branchOnBitReset4()noexcept;           // 0x4f
    void branchIfOverflowClear()noexcept;       // 0x50
    void xorWithZeroPageIndirectWithY()noexcept;// 0x51
    void xorWithZeroPageIndirect()noexcept;     // 0x52
    void xorWithZeroPageWithX()noexcept;        // 0x55
    void shiftRightZeroPageWithX()noexcept;     // 0x56
    void resetMemoryBit5()noexcept;             // 0x57
    void clearInteruptDisableFlag()noexcept;    // 0x58
    void xorWithAbsoluteWithY()noexcept;        // 0x59
    void pushY()noexcept;                       // 0x5a
    void xorWithAbsoluteWithX()noexcept;        // 0x5d
    void shiftRightAbsoluteWithX()noexcept;     // 0x5e
    void branchOnBitReset5()noexcept;           // 0x5f
    void returnFromSubroutine()noexcept;        // 0x60
    void addWithZeroPageWithXIndirect()noexcept;// 0x61
    void store0AtZeroPage()noexcept;            // 0x64
    void addWithZeroPage()noexcept;             // 0x65
    void rotateRightZeroPage()noexcept;         // 0x66
    void resetMemoryBit6()noexcept;             // 0x67
    void pullA()noexcept;                       // 0x68
    void addWithImediate()noexcept;             // 0x69
    void rotateRightA()noexcept;                // 0x6a
    void jumpAbsoluteIndirect()noexcept;        // 0x6c
    void addWithAbsolute()noexcept;             // 0x6d
    void rotateRightAbsolute()noexcept;         // 0x6e
    void branchOnBitReset6()noexcept;           // 0x6f
    void branchIfOverflowSet()noexcept;         // 0x70
    void addWithZeroPageIndirectWithY()noexcept;// 0x71
    void addWithZeroPageIndirect()noexcept;     // 0x72
    void store0AtZeroPageWithX()noexcept;       // 0x74
    void addWithZeroPageWithX()noexcept;        // 0x75
    void rotateRightZeroPageWithX()noexcept;    // 0x76
    void resetMemoryBit7()noexcept;             // 0x77
    void setInteruptDisable()noexcept;          // 0x78
    void addWithAbsoluteWithY()noexcept;        // 0x79
    void pullY()noexcept;                       // 0x7a
    void jumpAbsoluteWithXIndirect()noexcept;   // 0x7c
    void addWithAbsoluteWithX()noexcept;        // 0x7d
    void rotateRightAbsoluteWithX()noexcept;    // 0x7e
    void branchOnBitReset7()noexcept;           // 0x7f
    void branchAlways()noexcept;                // 0x80
    void storeAAtZeroPageWithXIndirect()noexcept;//0x81
    void storeYAtZeroPage()noexcept;            // 0x84
    void storeAAtZeroPage()noexcept;            // 0x85
    void storeXAtZeroPage()noexcept;            // 0x86
    void setMemoryBit0()noexcept;               // 0x87
    void decrementY()noexcept;                  // 0x88
    void bitTestWithImediate()noexcept;         // 0x89
    void transferXtoA()noexcept;                // 0x8a
    void storeYAtAbsolute()noexcept;            // 0x8c
    void storeAAtAbsolute()noexcept;            // 0x8d
    void storeXAtAbsolute()noexcept;            // 0x8e
    void branchOnBitSet0()noexcept;             // 0x8f
    void branchIfCarryClear()noexcept;          // 0x90
    void storeAAtZeroPageIndirectWithY()noexcept;//0x91
    void storeAAtZeroPageIndirect()noexcept;    // 0x92
    void storeYAtZeroPageWithX()noexcept;       // 0x94
    void storeAAtZeroPageWithX()noexcept;       // 0x95
    void storeXAtZeroPageWithY()noexcept;       // 0x96
    void setMemoryBit1()noexcept;               // 0x97
    void transferYToA()noexcept;                // 0x98
    void storeAAtAbsoluteWithY()noexcept;       // 0x99
    void transferXToStackpointer()noexcept;     // 0x9a
    void store0AtAbsolute()noexcept;            // 0x9c
    void storeAAtAbsoluteWithX()noexcept;       // 0x9d
    void store0AtAbsoluteWithX()noexcept;       // 0x9e
    void branchOnBitSet1()noexcept;             // 0x9f
    void loadYImediate()noexcept;               // 0xa0
    void loadAZeroPageWithXIndirect()noexcept;  // 0xa1
    void loadXImidiate()noexcept;               // 0xa2
    void loadYZeroPage()noexcept;               // 0xa4
    void loadAZeroPage()noexcept;               // 0xa5
    void loadXZeroPage()noexcept;               // 0xa6
    void setMemoryBit2()noexcept;               // 0xa7
    void transferAToY()noexcept;                // 0xa8
    void loadAImidiate()noexcept;               // 0xa9
    void transferAToX()noexcept;                // 0xaa
    void loadYAbsolute()noexcept;               // 0xac
    void loadAAbsolute()noexcept;               // 0xad
    void loadXAbsolute()noexcept;               // 0xae
    void branchOnBitSet2()noexcept;             // 0xaf
    void branchIfCarrySet()noexcept;            // 0xb0
    void loadAZeroPageIndirectWithY()noexcept;  // 0xb1
    void loadAZeroPageIndirect()noexcept;       // 0xb2
    void loadYZeroPageWithX()noexcept;          // 0xb4
    void loadAZeroPageWithX()noexcept;          // 0xb5
    void loadXZeroPageWithY()noexcept;          // 0xb6
    void setMemoryBit3()noexcept;               // 0xb7
    void clearOverflow()noexcept;               // 0xb8
    void loadAAbsoluteWithY()noexcept;          // 0xb9
    void transferStackpointerToX()noexcept;     // 0xba
    void loadYAbsoluteWithX()noexcept;          // 0xbc
    void loadAAbsoluteWithX()noexcept;          // 0xbd
    void loadXAbsoluteWithY()noexcept;          // 0xbe
    void branchOnBitSet3()noexcept;             // 0xbf
    void compareYWithImediate()noexcept;        // 0xc0
    void compareAWithZeroPageWithXIndirect()noexcept;//0xc1
    void compareYWithZeroPage()noexcept;        // 0xc4
    void compareAWithZeroPage()noexcept;        // 0xc5
    void decrementZeroPage()noexcept;           // 0xc6
    void setMemoryBit4()noexcept;               // 0xc7
    void incrementY()noexcept;                  // 0xc8
    void compareAWithImediate()noexcept;        // 0xc9
    void decrementX()noexcept;                  // 0xca
    void waitForInterupt()noexcept;             // 0xcb
    void compareYWithAbsolute()noexcept;        // 0xcc
    void compareAWithAbsolute()noexcept;        // 0xcd
    void decrementAbsolute()noexcept;           // 0xce
    void branchOnBitSet4()noexcept;             // 0xcf
    void branchNotEqual()noexcept;              // 0xd0
    void compareAWithZeroPageIndirectWithY()noexcept;//0xd1
    void compareAWithZeroPageIndirect()noexcept;// 0xd2
    void compareAWithZeroPageWithX()noexcept;   // 0xd5
    void decrementZeroPageWithX()noexcept;      // 0xd6
    void setMemoryBit5()noexcept;               // 0xd7
    void clearDecimalMode()noexcept;            // 0xd8
    void compareAWithAbsoluteWithY()noexcept;   // 0xd9
    void pushX()noexcept;                       // 0xda
    void stopMode()noexcept;                    // 0xdb
    void compareAWithAbsoluteWithX()noexcept;   // 0xdd
    void decrementAbsoluteWithX()noexcept;      // 0xde
    void branchOnBitSet5()noexcept;             // 0xdf
    void compareXWithImediate()noexcept;        // 0xe0
    void subtractWithZeroPageWithXIndirect()noexcept;//0xe1
    void compareXWithZeroPage()noexcept;        // 0xe4
    void subtractWithZeroPage()noexcept;        // 0xe5
    void incrementZeroPage()noexcept;           // 0xe6
    void setMemoryBit6()noexcept;               // 0xe7
    void incrementX()noexcept;                  // 0xe8
    void subtractWithImediate()noexcept;        // 0xe9
    void noop()noexcept;                        // 0xea
    void compareXWithAbsolute()noexcept;        // 0xec
    void subtractWithAbsolute()noexcept;        // 0xed
    void incrementAbsolute()noexcept;           // 0xee
    void branchOnBitSet6()noexcept;             // 0xef
    void branchIfEqual()noexcept;               // 0xf0
    void subtractWithZeroPageIndirectWithY()noexcept;//0xf1
    void subtractWithZeroPageIndirect()noexcept;// 0xf2
    void subtractWithZeroPageWithX()noexcept;   // 0xf5
    void incrementZeroPageWithX()noexcept;      // 0xf6
    void setMemoryBit7()noexcept;               // 0xf7
    void setDecimalMode()noexcept;              // 0xf8
    void subtractWithAbsoluteWithY()noexcept;   // 0xf9
    void pullX()noexcept;                       // 0xfa
    void subtractWithAbsoluteWithX()noexcept;   // 0xfd
    void incrementAbsoluteWithX()noexcept;      // 0xfe
    void branchOnBitSet7()noexcept;             // 0xff

public:
    Cpu(unsigned char& _data, unsigned short& _address, bool& _VPB, bool& _RDY, const bool& _IRQB, bool& _MLB, const bool& _NMIB,
        bool& _SYNC, bool& _RWB, const bool& _BE, const bool& _SOB, const bool& _PHI2, bool& _PHI1O, bool& _PHI2O)noexcept;
    void cycle()noexcept;
    void reset()noexcept;
};
