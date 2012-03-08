// Copyright (c) 2008, WEST, Polytechnic Institute of NYU
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//  1. Redistributions of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//  3. Neither the name of WEST, Polytechnic Institute of NYU nor the names
//     of its contributors may be used to endorse or promote products derived
//     from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// Author(s): Torsten Suel, Jiangong Zhang, Jinru He
//
// If you have any questions or problems with our code, please contact:
// jhe@cis.poly.edu
//
// The coding base class. All of the encoding classes inherit from this.
// It includes some basic bitwise operations.

#ifndef CODING_H_
#define CODING_H_

#include "unpack.h"

void pack(unsigned int* v, unsigned int b, unsigned int n, unsigned int* w) {
  unsigned int i;
  int bp, wp, s;

  for (bp = 0, i = 0; i < n; i++, bp += b) {
    wp = bp >> 5;
    s = 32 - b - (bp & 31);
    if (s >= 0)
      w[wp] |= (v[i] << s);
    else {
      s = -s;
      w[wp] |= (v[i] >> s);
      w[wp + 1] = (v[i] << (32 - s));
    }
  }
}

// Pointer to a function
typedef void (*pf)(unsigned int* p, unsigned int* w, int BS);
pf unpack[17] = {unpack0, unpack1, unpack2, unpack3, unpack4, unpack5, unpack6,
		 unpack7, unpack8, unpack9, unpack10, unpack11, unpack12, unpack13,
		 unpack16, unpack20, unpack32};

#endif /* CODING_H_ */
