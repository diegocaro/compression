/// S.Heman.Super-scalardatabasecompressionbetweenRAMandCPU-cache. MS Thesis, Centrum voor Wiskunde en Informatica (CWI), Amsterdam, 2005.

// M. Zukowski, S. Heman, N. Nes, and P. Boncz. Super-scalar RAM-CPU cache compression. In Proc. of the Int. Conf. on Data Engineering, 2006.

// J. Zhang, X. Long, and T. Suel, “Performance of compressed inverted list caching in search engines,” in WWW '08: Proceeding of the 17th international conference on World Wide Web, 2008.
// Uses this implementation http://www2008.org/papers/pdf/p387-zhangA.pdf

// in java https://github.com/hyan/kamikaze/blob/master/src/main/java/com/kamikaze/pfordelta/PForDelta.java

#include<stdio.h>
#include<stdlib.h>

#include "coding.h" //for pack function


  // from pfor_coding.h
  int b;
  int unpack_count;
  int t;
  int start;

int block_size = 128; // can be 32, 64, 128, 256
                      // depende del tamaño del size <64, <128, <256

//All possible values of b in the PForDelta algorithm
int cnum[17] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,16,20,32};

float FRAC = 0.1; // percent of exceptions in block_size

int pfor_encode(unsigned int **, unsigned int *, int);
unsigned* pfor_decode(unsigned int* _p, unsigned int* _w, int flag);

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
  int b = cnum[num + 2]; // the b value in pfordelta :)
  int start;  // first exception ;)

  unsigned int out[block_size]; // array for non-exceptions
  unsigned int ex[block_size]; // array for exceptions

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



int pfor_decompress(unsigned int* input, unsigned int* output, int size) {
  unsigned int* tmp = input;
  int flag = *tmp;
  
  b = cnum[((flag >> 12) & 15) + 2];
  unpack_count = ((flag >> 12) & 15) + 2;
  t = (flag >> 10) & 3;
  start = flag & 1023;

  tmp++;
  tmp = pfor_decode(output, tmp, flag);
  return tmp - input;
}

unsigned* pfor_decode(unsigned int* _p, unsigned int* _w, int flag) {
  int i, s;
  unsigned int x;
  

  // This can be improved... :)
  //(unpack[unpack_count])(_p, _w, block_size);

  switch(unpack_count) {
  case 0: unpack0(_p, _w, block_size); break;
  case 1: unpack1(_p, _w, block_size); break;
  case 2: unpack2(_p, _w, block_size); break;
  case 3: unpack3(_p, _w, block_size); break;
  case 4: unpack4(_p, _w, block_size); break;
  case 5: unpack5(_p, _w, block_size); break;
  case 6: unpack6(_p, _w, block_size); break;
  case 7: unpack7(_p, _w, block_size); break;
  case 8: unpack8(_p, _w, block_size); break;
  case 9: unpack9(_p, _w, block_size); break;
  case 10: unpack10(_p, _w, block_size); break;
  case 11: unpack11(_p, _w, block_size); break;
  case 12: unpack12(_p, _w, block_size); break;
  case 13: unpack13(_p, _w, block_size); break;
  case 16: unpack16(_p, _w, block_size); break;
  case 20: unpack20(_p, _w, block_size); break;
  case 32: unpack32(_p, _w, block_size); break;
  }


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


// Example of usage
int main(int argc, char *argv[]) {
  //unsigned int input[] = {2322231,2,3,4,5,6,7,8,9,10};
  unsigned int input[128] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,10,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,10,10,10,10,10,10,10,10};
  unsigned int *coded;
  unsigned int *output;

  int size = 128;
  int newsize;
  int finalsize;
  int i;

  coded = (unsigned int *) malloc( size * sizeof(unsigned int) );
  output = (unsigned int *) malloc( size * sizeof(unsigned int) );
  //printf("coded = %X\n", coded);
  //printf("output = %X\n", output);
  newsize = pfor_compress(input, coded, size);
  finalsize = pfor_decompress(coded, output, size);

  printf("Normal size: %d\n", size);
  printf("Compress size: %d\n", newsize);
  printf("Consumed size: %d\n", finalsize);
  for(i = 0; i < size; i++) {
    printf("%u -> %X -> %u\n", input[i], coded[i], output[i]);
  }
  return 0;
}
