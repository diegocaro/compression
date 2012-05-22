#ifndef CODING_POLICY_H_
#define CODING_POLICY_H_

// The 'input' array size should be at least an upper multiple of 'BLOCK_SIZE'.
int compress_pfordelta(unsigned int *input, unsigned int *output, int num_input_elements, int _block_size);

// The 'output' array size should be at least an upper multiple of 'block_size_'.
int decompress_pfordelta(unsigned int* input, unsigned int* output, int num_input_elements, int _block_size);


#endif /* CODING_POLICY_H_ */
