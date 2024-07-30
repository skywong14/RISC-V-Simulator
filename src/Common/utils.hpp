//
// Created by skywa on 2024/7/29.
//

#ifndef RISC_V_SIMULATOR_UTILS_HPP
#define RISC_V_SIMULATOR_UTILS_HPP

#pragma once
#include <iostream>
#include <cstring>

int toTwosComplement(unsigned int num) {
    // Check if the number is negative in unsigned representation
    if (num & 0x80000000) {
        // If it is, subtract 2^32 to get the signed equivalent
        return static_cast<int>(num - 0x100000000);
    } else {
        // If it is not, simply return the number cast to int
        return static_cast<int>(num);
    }
}
unsigned int toOnesComplement(int num) {
    if (num >= 0) {
        return static_cast<unsigned int>(num);
    } else {
        // Invert the bits of the negative number
        return ~static_cast<unsigned int>(-num) + 1;
    }
}


#endif //RISC_V_SIMULATOR_UTILS_HPP
