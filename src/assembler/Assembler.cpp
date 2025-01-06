#include "Assembler.hpp"

#include <bit>

const std::map<std::string, unsigned char> accumulatorORImpliedORStack {
    { "asl", 0x0A },
    { "clc", 0x18 },
    { "cld", 0xD8 },
    { "cli", 0x58 },
    { "clv", 0xB8 },
    { "dec", 0xA3 },
    { "dex", 0xCA },
    { "dey", 0x88 },
    { "inx", 0xE8 },
    { "iny", 0xC8 },
    { "lsr", 0x4A },
    { "nop", 0xEA },
    { "rol", 0x2A },
    { "ror", 0x6A },
    { "sec", 0x38 },
    { "sed", 0xF8 },
    { "sei", 0x78 },
    { "stp", 0xDB },
    { "tax", 0xAA },
    { "tay", 0xA8 },
    { "tsx", 0xBA },
    { "txa", 0x8A },
    { "txs", 0x9A },
    { "tya", 0x98 },
    { "wai", 0xCB },
    { "brk", 0x00 },
    { "pha", 0x48 },
    { "php", 0x08 },
    { "phx", 0xDA },
    { "phy", 0x5A },
    { "pla", 0x68 },
    { "plp", 0x28 },
    { "plx", 0xFA },
    { "ply", 0x7A },
    { "rty", 0x40 },
    { "rts", 0x60 }
};
const std::map<std::string, unsigned char> immediate {
    { "adc", 0x69 },
    { "and", 0x29 },
    { "cmp", 0xC9 },
    { "cpx", 0xE0 },
    { "cpy", 0xC0 },
    { "eor", 0x49 },
    { "lda", 0xA9 },
    { "ldx", 0xA2 },
    { "ldy", 0xA0 },
    { "ora", 0x09 },
    { "sbc", 0xE9 }
};
const std::map<std::string, unsigned char> absolute {
    { "adc", 0x6D },
    { "and", 0x2D },
    { "asl", 0x0E },
    { "bit", 0x2C },
    { "cmp", 0xCD },
    { "cpx", 0xEC },
    { "cpy", 0xCC },
    { "dec", 0xCE },
    { "eor", 0x4D },
    { "inc", 0xEE },
    { "jmp", 0x4C },
    { "jsr", 0x20 },
    { "lda", 0xAD },
    { "ldx", 0xAE },
    { "ldy", 0xAC },
    { "lsr", 0x4E },
    { "ora", 0x0D },
    { "rol", 0x2E },
    { "ror", 0x6E },
    { "sbc", 0xED },
    { "sta", 0x8D },
    { "stx", 0x8E },
    { "sty", 0x8C },
    { "stz", 0x9C },
    { "trb", 0x1C },
    { "tsb", 0x0C }
};
const std::map<std::string, unsigned char> absoluteWithXIndirect {
    { "jmp", 0x7C }
};
const std::map<std::string, unsigned char> absoluteWithX {
    { "adc", 0x7D },
    { "and", 0x3D },
    { "asl", 0x1E },
    { "bit", 0x3C },
    { "cmp", 0xDD },
    { "dec", 0xDE },
    { "eor", 0x5D },
    { "inc", 0xFE },
    { "lda", 0xBD },
    { "ldy", 0xBC },
    { "lsr", 0x5E },
    { "ora", 0x1D },
    { "rol", 0x3E },
    { "ror", 0x7E },
    { "sbc", 0xFD },
    { "sta", 0x9D },
    { "stz", 0x9E }
};
const std::map<std::string, unsigned char> absoluteWithY {
    { "adc", 0x79 },
    { "and", 0x39 },
    { "cmp", 0xD9 },
    { "eor", 0x59 },
    { "lda", 0xB9 },
    { "ldx", 0xBE },
    { "ora", 0x19 },
    { "sbc", 0xF9 },
    { "sta", 0x99 }
};
const std::map<std::string, unsigned char> absoluteIndirect {
    { "jmp", 0x6C }
};
const std::map<std::string, unsigned char> zeroPageORRelative {
    { "adc", 0x65 },
    { "and", 0x25 },
    { "asl", 0x06 },
    { "bbr0", 0x0F },
    { "bbr1", 0x1F },
    { "bbr2", 0x2F },
    { "bbr3", 0x3F },
    { "bbr4", 0x4F },
    { "bbr5", 0x5F },
    { "bbr6", 0x6F },
    { "bbr7", 0x7F },
    { "bbs0", 0x8F },
    { "bbs1", 0x9F },
    { "bbs2", 0xAF },
    { "bbs3", 0xBF },
    { "bbs4", 0xCF },
    { "bbs5", 0xDF },
    { "bbs6", 0xEF },
    { "bbs7", 0xFF },
    { "bcc", 0x90 },
    { "bcs", 0xB0 },
    { "beq", 0xF0 },
    { "bit", 0x24 },
    { "bmi", 0x30 },
    { "bne", 0xD0 },
    { "bpl", 0x10 },
    { "bra", 0x80 },
    { "bvc", 0x50 },
    { "bvs", 0x70 },
    { "cmp", 0xC5 },
    { "cpx", 0xE4 },
    { "cpy", 0xC4 },
    { "dec", 0xC6 },
    { "eor", 0x45 },
    { "inc", 0xE6 },
    { "lda", 0xA5 },
    { "ldx", 0xA6 },
    { "ldy", 0xA4 },
    { "lsr", 0x46 },
    { "ora", 0x05 },
    { "rmb0", 0x07 },
    { "rmb1", 0x17 },
    { "rmb2", 0x27 },
    { "rmb3", 0x37 },
    { "rmb4", 0x47 },
    { "rmb5", 0x57 },
    { "rmb6", 0x67 },
    { "rmb7", 0x77 },
    { "rol", 0x26 },
    { "ror", 0x66 },
    { "sbc", 0xE5 },
    { "smb0", 0x87 },
    { "smb1", 0x97 },
    { "smb2", 0xA7 },
    { "smb3", 0xB7 },
    { "smb4", 0xC7 },
    { "smb5", 0xD7 },
    { "smb6", 0xE7 },
    { "smb7", 0xF7 },
    { "sta", 0x85 },
    { "stx", 0x86 },
    { "sty", 0x84 },
    { "stz", 0x64 },
    { "trb", 0x14 },
    { "tsb", 0x04 }
};
const std::map<std::string, unsigned char> zeroPageWithXIndirect {
    { "adc", 0x61 },
    { "and", 0x21 },
    { "cmp", 0xC1 },
    { "eor", 0x41 },
    { "lda", 0xA1 },
    { "ora", 0x01 },
    { "sbc", 0xE1 },
    { "sta", 0x81 }
};
const std::map<std::string, unsigned char> zeroPageWithX {
    { "adc", 0x75 },
    { "and", 0x35 },
    { "asl", 0x16 },
    { "bit", 0x34 },
    { "cmp", 0xD5 },
    { "dec", 0xD6 },
    { "eor", 0x55 },
    { "inc", 0xF6 },
    { "lda", 0xB5 },
    { "ldy", 0xB4 },
    { "lsr", 0x56 },
    { "ora", 0x15 },
    { "rol", 0x36 },
    { "ror", 0x76 },
    { "sbc", 0xF5 },
    { "sta", 0x95 }
};
const std::map<std::string, unsigned char> zeroPageWithY {
    { "ldx", 0xB6 },
    { "stx", 0x96 }
};
const std::map<std::string, unsigned char> zeroPageIndirect {
    { "adc", 0x72 },
    { "and", 0x32 },
    { "cmp", 0xD2 },
    { "eor", 0x52 },
    { "lda", 0xB2 },
    { "ora", 0x12 },
    { "sbc", 0xF2 },
    { "sta", 0x92 }
};
const std::map<std::string, unsigned char> zeroPageIndirectWithY {
    { "adc", 0x71 },
    { "and", 0x31 },
    { "cmp", 0xD1 },
    { "eor", 0x51 },
    { "lda", 0xB1 },
    { "ora", 0x11 },
    { "sbc", 0xF1 },
    { "sta", 0x91 }
};

unsigned long getValue(const std::string& value) {
    if (value[0] == '$') {
        return std::stoul(value.substr(1, value.size()), nullptr, 16);
    }
    if (value[0] == '%') {
        return std::stoul(value.substr(1, value.size()), nullptr, 2);
    }
    if (std::isdigit(value[0])) {
        return std::stoul(value);
    }
    throw std::runtime_error(value + " not a value");
}

unsigned getOperationByteCount(const std::string& line) {
    auto opName = line | std::views::take_while([] (char c) {return std::isgraph(c);}) | std::views::transform([] (char c) {return std::tolower(c);}) | std::ranges::to<std::string>();
    auto address = line | std::views::drop(opName.size()) | std::views::drop_while([] (char c) {return std::isspace(c);}) |
        std::views::take_while([] (char c) {return std::isgraph(c);}) | std::ranges::to<std::string>();

    if (address.size() == 0 || address[0] == '/') {
        if (accumulatorORImpliedORStack.contains(opName)) {
            return 1;
        }
        throw std::runtime_error(opName + " needs parameter or is invalid");
    }
    if (address[0] == '#') {
        if (immediate.contains(opName)) {
            return 2;
        }
        throw std::runtime_error(opName + " is invalid with immediate");
    }
    if (address[0] == '(') {
        auto inBracket = address | std::views::drop(1) | std::views::take_while([] (char c) {return c != ')';}) | std::ranges::to<std::string>();
        auto afterBracket = address | std::views::drop_while([] (char c) {return c != ')';}) | std::views::drop(1) | std::ranges::to<std::string>();
        if (afterBracket.size()) {
            if (afterBracket != ",Y") {
                throw std::runtime_error("invalid address: " + line);
            }
            auto i = getValue(inBracket);
            if (std::bit_width(i) > 8) {
                throw std::runtime_error("invalid address: " + line);
            }
            if (zeroPageIndirectWithY.contains(opName)) {
                return 2;
            }
            throw std::runtime_error(opName + " cannot be called with: Zero Page Indirect Indexed with Y");
        }
        if (inBracket.contains(',')) {
            if (!inBracket.ends_with(",X")) {
                throw std::runtime_error("invalid address: " + line);
            }
            if (std::isalpha(inBracket[0])) { // should be label
                if (absoluteWithXIndirect.contains(opName)) {
                    return 3;
                }
                throw std::runtime_error(opName + " cannot be called with: Absolute Indirect Indexed with X");
            }
            auto i = getValue(inBracket.substr(0, inBracket.find_last_of(',')));
            if (std::bit_width(i) <= 8) {
                if (zeroPageWithXIndirect.contains(opName)) {
                    return 2;
                }
            }
            if (std::bit_width(i) > 16) {
                throw std::runtime_error("invalid address: " + line);
            }
            if (absoluteWithXIndirect.contains(opName)) {
                return 3;
            }
            throw std::runtime_error("something wrong at: " + line);
        }
        if (std::isalpha(inBracket[0])) { // should be label
            if (absoluteIndirect.contains(opName)) {
                return 3;
            }
            throw std::runtime_error(opName + " cannot be called with: Absolute Indirect Indexed with X");
        }
        auto i = getValue(inBracket);
        if (std::bit_width(i) <= 8) {
            if (zeroPageIndirect.contains(opName)) {
                return 2;
            }
        }
        if (std::bit_width(i) > 16) {
            throw std::runtime_error("invalid address: " + line);
        }
        if (absoluteIndirect.contains(opName)) {
            return 3;
        }
        throw std::runtime_error("something wrong at: " + line);
    }
    if (address.contains(',')) {
        if (address.ends_with(",X")) {
            if (std::isalpha(address[0])) { // should be label
                if (absoluteWithX.contains(opName)) {
                    return 3;
                }
                throw std::runtime_error(opName + " cannot be called with: Absolute indirect");
            }
            auto i = getValue(address.substr(0, address.find_last_of(',')));
            if (std::bit_width(i) <= 8) {
                if (zeroPageWithX.contains(opName)) {
                    return 2;
                }
            }
            if (std::bit_width(i) > 16) {
                throw std::runtime_error("invalid address: " + line);
            }
            if (absoluteWithX.contains(opName)) {
                return 3;
            }
            throw std::runtime_error("something wrong at: " + line);
        }
        if (address.ends_with(",Y")) {
            if (std::isalpha(address[0])) { // should be label
                if (absoluteWithY.contains(opName)) {
                    return 3;
                }
                throw std::runtime_error(opName + " cannot be called with: Absolute Indexed with Y");
            }
            auto i = getValue(address.substr(0, address.find_last_of(',')));
            if (std::bit_width(i) <= 8) {
                if (zeroPageWithY.contains(opName)) {
                    return 2;
                }
            }
            if (std::bit_width(i) > 16) {
                throw std::runtime_error("invalid address: " + line);
            }
            if (absoluteWithY.contains(opName)) {
                return 3;
            }
            throw std::runtime_error("something wrong at: " + line);
        }
        throw std::runtime_error("invalid address: " + line);
    }
    if (std::isalpha(address[0])) { // should be label
        if (absolute.contains(opName)) {
            return 3;
        }
        throw std::runtime_error(opName + " cannot be called with: Absolute");
    }
    auto i = getValue(address);
    if (std::bit_width(i) <= 8) {
        if (zeroPageORRelative.contains(opName)) {
            return 2;
        }
    }
    if (std::bit_width(i) > 16) {
        throw std::runtime_error("invalid address: " + line);
    }
    if (absolute.contains(opName)) {
        return 3;
    }
    throw std::runtime_error("something wrong at: " + line);
}

unsigned firstPass(const std::vector<std::string>& file, std::map<std::string, unsigned short>& labels, unsigned short& globalStart) {
    unsigned currentPos = 0;
    bool startSet = false;
    for (const auto& line : file) {
        if (std::none_of(line.begin(), line.end(), [] (char c) {return std::isgraph(c);})) {
            continue;
        }
        if (std::isgraph(line[0])) {
            if (!std::isalpha(line[0])) {
                throw std::runtime_error("invalid label: " + line);
            }
            std::string label = line.substr(0, line.find(':'));
            if (labels.contains(label)) {
                throw std::runtime_error("duplicate label: " + label);
            }
            if (std::any_of(label.begin(), label.end(), [] (char c) {return !std::isalnum(c);})) {
                throw std::runtime_error("invalid label: " + label);
            }
            labels[label] = currentPos;
            startSet = true;
            continue;
        }
        std::string subline = line | std::views::drop_while([] (char c) {return std::isspace(c);}) | std::ranges::to<std::string>();
        if (subline[0] == '.') {
            if (subline.starts_with(".org")) {
                if (subline.size() == 4 || !std::isspace(subline[4])) {
                    throw std::runtime_error("no origin value");
                }
                auto valueLine = subline | std::views::drop(4) | std::views::drop_while([] (char c) {return std::isspace(c);}) |
                    std::views::take_while([] (char c) {return std::isgraph(c);}) | std::ranges::to<std::string>();
                if (valueLine.size() == 0) {
                    throw std::runtime_error("no origin value");
                }
                unsigned long i = getValue(valueLine);
                if (std::bit_width(i) > 16) {
                    throw std::runtime_error("to high origin value");
                }
                currentPos = i;
                if (!startSet) {
                    globalStart = i;
                    startSet = true;
                }
                continue;
            }
            startSet = true;
            if (subline.starts_with(".word")) {
                if (subline.size() == 5 || !std::isspace(subline[5])) {
                    throw std::runtime_error("no word");
                }
                currentPos += 2;
                continue;
            }
            throw std::runtime_error("unknown . command: " + line);
        }
        currentPos += getOperationByteCount(subline);
    }
    return currentPos;
}

// what ever the first pass checked is not checked again here
std::vector<unsigned char> getOperationBytes(const std::string& line, const std::map<std::string, unsigned short>& labels) {
    auto opName = line | std::views::take_while([] (char c) {return std::isgraph(c);}) | std::views::transform([] (char c) {return std::tolower(c);}) | std::ranges::to<std::string>();
    auto address = line | std::views::drop(opName.size()) | std::views::drop_while([] (char c) {return std::isspace(c);}) |
        std::views::take_while([] (char c) {return std::isgraph(c);}) | std::ranges::to<std::string>();

    if (address.size() == 0 || address[0] == '/') {
        return { accumulatorORImpliedORStack.at(opName) };
    }
    if (address[0] == '#') {
        auto i = getValue(address.substr(1, address.size()));
        if (std::bit_width(i) > 8) {
            throw std::runtime_error("immediate value: " + address + " out of 8 bit range");
        }
        return { immediate.at(opName), (unsigned char)i };
    }
    if (address[0] == '(') {
        auto inBracket = address | std::views::drop(1) | std::views::take_while([] (char c) {return c != ')';}) | std::ranges::to<std::string>();
        auto afterBracket = address | std::views::drop_while([] (char c) {return c != ')';}) | std::views::drop(1) | std::ranges::to<std::string>();
        if (afterBracket.size()) {
            auto i = getValue(inBracket);
            return { zeroPageIndirectWithY.at(opName), (unsigned char)i };
        }
        if (inBracket.contains(',')) {
            if (std::isalpha(inBracket[0])) { // should be label
                auto label = inBracket.substr(0, inBracket.find_last_of(','));
                if (!labels.contains(label)) {
                    throw std::runtime_error("label: " + label + " not declared");
                }
                auto s = labels.at(label);
                unsigned char c1 = s >> 8;
                unsigned char c2 = s & 0x00FF;
                return { absoluteWithXIndirect.at(opName), c2, c1 };
            }
            auto i = getValue(inBracket.substr(0, inBracket.find_last_of(',')));
            if (std::bit_width(i) <= 8) {
                if (zeroPageWithXIndirect.contains(opName)) {
                    return { zeroPageWithXIndirect.at(opName), (unsigned char)i };
                }
            }
            unsigned char c1 = i >> 8;
            unsigned char c2 = i & 0x00FF;
            return { absoluteWithXIndirect.at(opName), c2, c1 };
        }
        if (std::isalpha(inBracket[0])) { // should be label
            if (!labels.contains(inBracket)) {
                throw std::runtime_error("label: " + inBracket + " not declared");
            }
            auto s = labels.at(inBracket);
            unsigned char c1 = s >> 8;
            unsigned char c2 = s & 0x00FF;
            return { absoluteIndirect.at(opName), c2, c1 };
        }
        auto i = getValue(inBracket);
        if (std::bit_width(i) <= 8) {
            if (zeroPageIndirect.contains(opName)) {
                return { zeroPageIndirect.at(opName), (unsigned char)i };
            }
        }
        unsigned char c1 = i >> 8;
        unsigned char c2 = i & 0x00FF;
        return { absoluteIndirect.at(opName), c2, c1 };
    }
    if (address.contains(',')) {
        if (address.ends_with(",X")) {
            if (std::isalpha(address[0])) { // should be label
                auto label = address.substr(0, address.find_last_of(','));
                if (!labels.contains(label)) {
                    throw std::runtime_error("label: " + label + " not declared");
                }
                auto s = labels.at(label);
                unsigned char c1 = s >> 8;
                unsigned char c2 = s & 0x00FF;
                return { absoluteWithX.at(opName), c2, c1 };
            }
            auto i = getValue(address.substr(0, address.find_last_of(',')));
            if (std::bit_width(i) <= 8) {
                if (zeroPageWithX.contains(opName)) {
                    return { zeroPageWithX.at(opName), (unsigned char)i };
                }
            }
            unsigned char c1 = i >> 8;
            unsigned char c2 = i & 0x00FF;
            return { absoluteWithX.at(opName), c2, c1 };
            throw std::runtime_error("something wrong at: " + line);
        }
        if (address.ends_with(",Y")) {
            if (std::isalpha(address[0])) { // should be label
                auto label = address.substr(0, address.find_last_of(','));
                if (!labels.contains(label)) {
                    throw std::runtime_error("label: " + label + " not declared");
                }
                auto s = labels.at(label);
                unsigned char c1 = s >> 8;
                unsigned char c2 = s & 0x00FF;
                return { absoluteWithY.at(opName), c2, c1 };
            }
            auto i = getValue(address.substr(0, address.find_last_of(',')));
            if (std::bit_width(i) <= 8) {
                if (zeroPageWithY.contains(opName)) {
                    return { zeroPageWithY.at(opName), (unsigned char)i };
                }
            }
            unsigned char c1 = i >> 8;
            unsigned char c2 = i & 0x00FF;
            return { absoluteWithY.at(opName), c2, c1 };
        }
        throw std::runtime_error("should have been thrown before");
    }
    if (std::isalpha(address[0])) { // should be label
        if (!labels.contains(address)) {
            throw std::runtime_error("label: " + address + " not declared");
        }
        auto s = labels.at(address);
        unsigned char c1 = s >> 8;
        unsigned char c2 = s & 0x00FF;
        return { absolute.at(opName), c2, c1 };
    }
    auto i = getValue(address);
    if (std::bit_width(i) <= 8) {
        if (zeroPageORRelative.contains(opName)) {
            return { zeroPageORRelative.at(opName), (unsigned char)i };
        }
    }
    unsigned char c1 = i >> 8;
    unsigned char c2 = i & 0x00FF;
    return { absolute.at(opName), c2, c1 };
}

// no error checking where already done
std::vector<unsigned char> secondPass(const std::vector<std::string>& file, const std::map<std::string, unsigned short>& labels, unsigned short globalStart, unsigned maxSize) {
    std::vector<unsigned char> rom = std::views::repeat(0, maxSize) | std::ranges::to<std::vector<unsigned char>>();
    unsigned currentPos = globalStart;
    for (const auto& line : file) {
        if (std::none_of(line.begin(), line.end(), [] (char c) {return std::isgraph(c);})) {
            continue;
        }
        if (std::isgraph(line[0])) {
            std::string label = line.substr(0, line.find(':'));
            if (labels.at(label) != currentPos) {
                throw std::runtime_error("error");
            }
            continue;
        }
        std::string subline = line | std::views::drop_while([] (char c) {return std::isspace(c);}) | std::ranges::to<std::string>();
        if (subline[0] == '.') {
            if (subline.starts_with(".org")) {
                auto valueLine = subline | std::views::drop(4) | std::views::drop_while([] (char c) {return std::isspace(c);}) |
                    std::views::take_while([] (char c) {return std::isgraph(c);}) | std::ranges::to<std::string>();
                unsigned long i = getValue(valueLine);
                currentPos = i;
                continue;
            }
            if (subline.starts_with(".word")) {
                auto valueLine = subline | std::views::drop(5) | std::views::drop_while([] (char c) {return std::isspace(c);}) |
                    std::views::take_while([] (char c) {return std::isgraph(c);}) | std::ranges::to<std::string>();
                unsigned long i;
                if (std::isalpha(valueLine[0])) {
                    i = labels.at(valueLine);
                }
                else {
                    i = getValue(valueLine);
                    if (std::bit_width(i) > 16) {
                        throw std::runtime_error("to long for word: " + line);
                    }
                }
                unsigned char c1 = i >> 8;
                unsigned char c2 = i & 0x00FF;
                rom[currentPos - globalStart] = c2;
                rom[currentPos + 1 - globalStart] = c1;
                currentPos += 2;
                continue;
            }
            throw std::runtime_error("unknown . command: " + line);
        }
        auto command = getOperationBytes(subline, labels);
        for (int i = 0; i < command.size(); ++i) {
            rom[currentPos + i - globalStart] = command[i];
        }
        currentPos += command.size();
    }
    return rom;
}
