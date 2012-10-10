////
// Copyright (c) 2012 Universidad de Concepción, Chile. 
//
// Author: Diego Caro
//
// @UDEC_LICENSE_HEADER_START@ 
//
// @UDEC_LICENSE_HEADER_END@ 

////
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
//
// Author(s): Torsten Suel, Jiangong Zhang, Jinru He
//
// If you have any questions or problems with our code, please contact:
// jhe@cis.poly.edu
//

////
// This is an implementation of PForDelta algorithm for sorted integer arrays.
// The PForDelta coding method is fast but compression efficiency is not as good as Rice
// coding. It is a blockwise coding, so you need to first set the block size to
// either 32, 64, 128, or 256. The default block size is 128. If the input
// buffer to the Compression() function is greater in size than the block size,
// any integers past the block size will be discarded.
// The meta data is stored as an uncompressed integer written at the beginning
// of the buffer; it includes the number of bits used per integer, the block
// size, and the number of integers coded without exceptions.
//
// 1. Original algorithm from:
//     http://citeseerx.ist.psu.edu/viewdoc/summary?doi=10.1.1.101.3316 and 
//     http://dx.doi.org/10.1109/ICDE.2006.150
//
// 2. Optimizacion from:
//     http://www2008.org/papers/pdf/p387-zhangA.pdf
//
// Alternative implementations:
//   * C++ http://code.google.com/p/poly-ir-toolkit/source/browse/trunk/src/compression_toolkit/pfor_coding.cc
//   * Java https://github.com/hyan/kamikaze/blob/master/src/main/java/com/kamikaze/pfordelta/PForDelta.java
//
// This code is based on an implementation in C++ of the Poly-IR-Toolkit. 
// It's available at http://code.google.com/p/poly-ir-toolkit/source/browse/trunk/src/compression_toolkit/pfor_coding.cc
//

#include<stdio.h>
#include<stdlib.h>

#include "pfordelta.h"
#include "pack.h" //for pack function
#include "unpack.h"

extern pf unpack[17]; //array to the unpack functions defined in unpack.h

// from pfor_coding.h
int b;  // b size
int unpack_count; // 
int t; // code for exception size in bits
int start; // first exception

int block_size = 128; // can be 32, 64, 128, 256
                      // depende del tamaño del size <64, <128, <256

//All possible values of b in the PForDelta algorithm
int pfor_cnum[17] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,16,20,32};

float FRAC = 0.1; // percent of exceptions in block_size

//
// Compress an integer array using PForDelta
// Parameters:
//    input pointer to the array of integers to compress
//    output pointer to the array of compressed integers
//    size (not used)
// Returns:
//    the number of 32-bits words used to compress the input
int pfor_compress(unsigned int *input, unsigned int *output, int size) {
  int flag = -1; // ?
  unsigned int* w;
  int k; // ?
  for (k = 0; flag < 0; k++) {
    w = output + 1;
    flag = pfor_encode(&w, input, k);
  }

  *output = flag;
  return w - output;
}

// w: output
// p: input
// j: ?
int pfor_encode(unsigned int** w, unsigned int* p, int num) {
  // bb bit size of exceptions
  // t code for bit size exceptions
  // i index to retrieve all numbers in block size
  // l index for last exception
  // n index for exceptions
  // s
  int i, l, n, bb, t, s;
  unsigned int m; // largest number in sequence
  int b = pfor_cnum[num + 1]; // the b value in pfordelta :)
  int start;  // first exception ;)

  unsigned int out[block_size]; // array for non-exceptions
  unsigned int ex[block_size]; // array for exceptions
  
  //printf("unsing b = %d bits\n",b);
  
  // All numbers in 32bit code, worst case
  if (b == 32) {
    for (i = 0; i < block_size; i++) {
      (*w)[i] = p[i];
    }
    *w += block_size;
    return ((num << 12) + (2 << 10) + block_size);
  }

  // Find the largest number we're encoding.
  for (m = 0, i = 0; i < block_size; i++) {
    if (p[i] > m)
      m = p[i];
  }

  // Setting bit size for exceptions 8, 16 or 32 bits
  if (m < 256) {
    bb = 8;
    t = 0;
  } else if (m < 65536) {
    bb = 16;
    t = 1;
  } else {
    bb = 32;
    t = 2;
  }

  //printf("using %d bits for exceptions\n",bb);

  // Selecting exceptions and non-exceptions
  for (start = 0, n = 0, l = -1, i = 0; i < block_size; i++) {
    if ((p[i] >= (unsigned) (1 << b)) // p[i] fits in b bits?
	|| ((l >= 0) && (i - l == (1 << b))) // the distance between this exception 
	) {                                  // and the last exception fits in b bits
      if (l < 0)
        start = i;
      else
        out[l] = i - l - 1;

      ex[n++] = p[i];
      l = i;
    } else {
      out[i] = p[i];
    }
  }

  if (l >= 0)
    out[l] = (1 << b) - 1;
  else
    start = block_size;

  if ( (double) (n) <= FRAC * (double) (block_size)) {
    // non-exceptions in b bits

    // s*bytes is the size of the b-bits words non-exception
    s = ((b * block_size) >> 5); 
    for (i = 0; i < s; i++) {
      (*w)[i] = 0;
    }
    pack(out, b, block_size, *w);
    *w += s;

    // exceptions in bb bits
    // size*4bytes of the excepcion array
    s = ((bb * n) >> 5) + ((((bb * n) & 31) > 0) ? 1 : 0);
    for (i = 0; i < s; i++) {
      (*w)[i] = 0;
    }
    pack(ex, bb, n, *w);
    *w += s;
    return ((num << 12) + (t << 10) + start); // this is the header!!!
  }

  return -1;
}

//
// Decompress an integer array using PForDelta
// Parameters:
//    input pointer to the array of compressed integers to decompress
//    output pointer to the array of integers
//    size (not used)
// Returns:
//    the number of 32-bits consumed in input
//
int pfor_decompress(unsigned int* input, unsigned int* output, int size) {
  unsigned int* tmp = input;
  int flag = *tmp;
  
  b = pfor_cnum[((flag >> 12) & 15) + 1];
  unpack_count = ((flag >> 12) & 15) + 1;
  t = (flag >> 10) & 3;
  start = flag & 1023;

  tmp++;
  tmp = pfor_decode(output, tmp, flag);
  return tmp - input;
}

unsigned* pfor_decode(unsigned int* _p, unsigned int* _w, int flag) {
  int i, s;
  unsigned int x;
  
  // Esta es una llamada a un arreglo de funciones de unpack.
  // La idea es ahorrarse un if o switch-case por cada una de
  // las funciones que dependenden de unpack_count.
  // La definición de arreglo unpack[] está en "coding.h"
  // El código equivalente con switch-case sería:
  //   switch(unpack_count) { */
  //     case 0: unpack0(_p, _w, block_size); break;
  //     case 1: unpack1(_p, _w, block_size); break;
  //     ...
  //     case n: unpack...; break; }
  (unpack[unpack_count])(_p, _w, block_size);

  _w += ((b * block_size) >> 5);

  switch (t) {
    case 0:
      for (s = start, i = 0; s < block_size; i++) {
        x = _p[s] + 1;
        _p[s] = (_w[i >> 2] >> (24 - ((i & 3) << 3))) & 255;
        s += x;
      }
      _w += (i >> 2);

      if ((i & 3) > 0)
        _w++;
      break;

    case 1:
      for (s = start, i = 0; s < block_size; i++) {
        x = _p[s] + 1;
        _p[s] = (_w[i >> 1] >> (16 - ((i & 1) << 4))) & 65535;
        s += x;
      }
      _w += (i >> 1);
      if ((i & 1) > 0)
        _w++;
      break;

    case 2:
      for (s = start, i = 0; s < block_size; i++) {
        x = _p[s] + 1;
        _p[s] = _w[i];
        s += x;
      }
      _w += i;
      break;
  }
  return _w;
}



