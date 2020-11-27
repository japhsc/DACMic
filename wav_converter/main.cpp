/*
g++ main.cpp -g -Wall -o main && ./main
*/
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "./../libraries/wave.h"


/*
181
44198
353584
1
8
240
44198
*/

std::ifstream infile("./../serial/2020_11_14-18_00_57.dat");

bool parse(int &num, bool &good) {
    std::string line;
    good = false;
    if (std::getline(infile, line)) {
        std::istringstream iss(line);

        if (iss >> num) {
            good = true;
        } else {
            std::cout << line << std::endl;
        }
        return true;
    }
    return false;
}

int main() {
    int i = 0, a;
    bool good;
    int count, sampling, bitrate, channel, depth, clock, len, dur;
    
    parse(count, good);
    parse(sampling, good);
    parse(bitrate, good);
    parse(channel, good);
    parse(depth, good);
    parse(clock, good);
    parse(len, good);
    parse(dur, good);
    
    uint8_t data[len];

    while (parse(a, good) && (i<len)) {
        if (good) {
            data[i] = a;
            if (a>255 or a<0) std::cout << a << std::endl;
        }
        ++i;
    }
    infile.close();
    std::cout << i << "/" << len << std::endl;
    
    uint8_t* out = NULL;
    int length = wav_encode(channel, depth/8, sampling, data, len, out);
    if (length>0) {
        std::ofstream outfile("2020_11_14-18_00_57.wav", std::istream::out | std::ios::binary);
        outfile.write((char*) out, length);
        outfile.close();
        free(out);
    }

    /*
    uint8_t* header = NULL;
    size_t length = wav_header(channel, depth/8, sampling, len, header);
    if (length>0) {
        std::ofstream outfile("2020_11_14-18_00_57.wav", std::istream::out | std::ios::binary);
        outfile.write((char*) header,   length);
        outfile.write((char*)   data,      len);
        outfile.close();
        free(header);
    }
    */
    std::cout << length << std::endl;
    return 0;
}

