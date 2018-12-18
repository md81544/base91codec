#pragma once

// Base91 encoding is based on code which is:
// copyright (c) 2011..2016 r-lyeh (https://github.com/r-lyeh), which in turn
// is based on code by Joachim Henke {
// Copyright (c) 2000-2006 Joachim Henke
// http://base91.sourceforge.net/ (v0.6.0)
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//  - Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
//  - Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//  - Neither the name of Joachim Henke nor the names of his contributors may
//    be used to endorse or promote products derived from this software without
//    specific prior written permission.
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#pragma once
#include <cstddef>
#include <string>
#include <vector>

namespace base91
{

void encode(std::vector<uint8_t>& out, const std::vector<uint8_t>& binary)
{
    const uint8_t enctab[91] = {
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', //00..12
        'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', //13..25
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', //26..38
        'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', //39..51
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '!', '#', '$', //52..64
        '%', '&', '(', ')', '*', '+', ',', '.', '/', ':', ';', '-', '=', //65..77
        '\\','?', '@', '[', ']', '^', '_', '`', '{', '|', '}', '~', '\'' //78..90
    };

    out.clear();
    const uint8_t* ib = binary.data();

    unsigned long queue = 0;
    unsigned int nbits = 0;

    for (size_t len = binary.size(); len--; )
    {
        queue |= *ib++ << nbits;
        nbits += 8;
        if (nbits > 13)  // enough bits in queue
        {
            unsigned int val = queue & 8191;

            if (val > 88)
            {
                queue >>= 13;
                nbits -= 13;
            }
            else  // we can take 14 bits
            {
                val = queue & 16383;
                queue >>= 14;
                nbits -= 14;
            }
            out.push_back(enctab[val % 91]);
            out.push_back(enctab[val / 91]);
        }
    }

    // process remaining bits from bit queue; write up to 2 bytes
    if (nbits)
    {
        out.push_back(enctab[queue % 91]);
        if (nbits > 7 || queue > 90)
        {
            out.push_back( enctab[ queue / 91 ] );
        }
    }

}


void decode(std::vector<uint8_t>& out, const std::vector<uint8_t>& text)
{
    const uint8_t dectab[256] = {
        91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91,
        91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91,
        91, 62, 91, 63, 64, 65, 66, 90, 67, 68, 69, 70, 71, 76, 72, 73,
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 74, 75, 91, 77, 91, 79,
        80,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 81, 78, 82, 83, 84,
        85, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 86, 87, 88, 89, 91,
        91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91,
        91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91,
        91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91,
        91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91,
        91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91,
        91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91,
        91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91,
        91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91 
    };

    out.clear();

    unsigned long queue = 0;
    unsigned int nbits = 0;
    int val = -1;

    for ( auto c : text )
    {
        unsigned int d = dectab[c];
        if ( d == 91 )
        {
            continue; // ignore non-alphabet chars
        }
        if (val == -1)
        {
            val = d;  // start next value
        }
        else {
            val += d * 91;
            queue |= val << nbits;
            nbits += (val & 8191) > 88 ? 13 : 14;
            do {
                out.push_back(char(queue));
                queue >>= 8;
                nbits -= 8;
            } while (nbits > 7);
            val = -1;   // mark value complete
        }
    }

    // process remaining bits; write at most 1 byte
    if (val != -1)
    {
        out.push_back( char( queue | val << nbits ) );
    }
}

} // namespace base91
