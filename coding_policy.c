#include<stdio.h>
#include"pfordelta.h"
#include"coding_policy.h"

extern int block_size;

// The 'input' array size should be at least an upper multiple of 'block_size_'.
int compress_pfordelta(unsigned int *input, unsigned int *output, int num_input_elements, int block_size_) {
  int num_whole_blocks = num_input_elements / block_size_;
  int encoded_offset = 0;
  int unencoded_offset = 0;

  int left_to_encode;
  int pad_until;
  int i;

  block_size = block_size_;

  while (num_whole_blocks-- > 0) {
    encoded_offset += pfor_compress(input + unencoded_offset, output + encoded_offset, block_size_);
    unencoded_offset += block_size_;
  }

  left_to_encode = num_input_elements % block_size_;
  if (left_to_encode != 0) {
    // Encode leftover portion with a blockwise coder, and pad it to the blocksize.
    // Assumption here is that the 'input' array size is at least an upper multiple of 'block_size_'.
    pad_until = block_size_ * ((num_input_elements / block_size_) + 1);

    for (i = num_input_elements; i < pad_until; ++i) {
      input[i] = 0;
    }
    encoded_offset += pfor_compress(input + unencoded_offset, output + encoded_offset, block_size_);
    unencoded_offset += block_size_;
  }

  return encoded_offset;
}


// The 'output' array size should be at least an upper multiple of 'block_size_'.
int decompress_pfordelta(unsigned int* input, unsigned int* output, int num_input_elements, int _block_size) {
  int num_whole_blocks = num_input_elements / _block_size;
  int encoded_offset = 0;
  int unencoded_offset = 0;
  int left_to_encode;

  block_size = _block_size;

  //printf("num_input_elements: %d\n", num_input_elements);
  while (num_whole_blocks-- > 0) {
    encoded_offset += pfor_decompress(input + encoded_offset, output + unencoded_offset, _block_size);
    unencoded_offset += _block_size;
  }

  left_to_encode = num_input_elements % _block_size;
  if (left_to_encode != 0) {
    // Decode leftover portion with a blockwise coder, since it was padded to the blocksize.
    // Assumption here is that the 'output' array size is at least an upper multiple of 'block_size_'.
    encoded_offset += pfor_decompress(input + encoded_offset, output + unencoded_offset, _block_size);
    unencoded_offset += _block_size;
  }

  return encoded_offset;
}
