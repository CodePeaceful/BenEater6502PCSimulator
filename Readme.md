Project based on the youtube series by Ben Eater
[text](https://www.youtube.com/playlist?list=PLowKtXNTBypFbtuVMUVXNR0z1mu7dp7eH)

External full featured assembler should be used
vasm is used in all testing vasm is available at [text](http://www.compilers.de/vasm.html)
the 6502 version with oldstyle syntax
the flag: -dotdir to allow dot directives
the flag: -wdc02 for all the features
the flag: -Fbin to get a binary file

Simulation:
    to prevent data races cycles have already be split in a low and a high phase

    undefined behavior:
        multiple chips writing to the same line in one cycle
        tring to access the bus while disabled
