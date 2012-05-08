// Copyright (c) 2010, Roman Khmelichek
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//  1. Redistributions of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//  3. Neither the name of Roman Khmelichek nor the names of its contributors
//     may be used to endorse or promote products derived from this software
//     without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
// EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

//==============================================================================================================================================================
// Author(s): Roman Khmelichek
//
// Some of the functions here are macros because many times we need to define local arrays on the stack for performance reasons, and a function is not
// considered an integer compile-time constant (for the purposes of defining the array size).
// These macros are for determining loose upperbounds to satisfy all the various coding policies.
//==============================================================================================================================================================

#ifndef CODING_POLICY_HELPER_H_
#define CODING_POLICY_HELPER_H_

// Determines the size of the input buffer that will be compressed.
// It needs to be a multiple of the block size for when we use blockwise codings, because we need to pad the input buffer with 0s until the block size.
// If the block size is 0, we have a non-blockwise coder, and don't need an upperbound.
#define UncompressedInBufferUpperbound(buffer_size, block_size) ((((block_size) == 0) || ((buffer_size) % (block_size) == 0)) ? (buffer_size) : ((((buffer_size) / (block_size)) + 1) * (block_size)))

// Determines size of the output buffer for decompressing to.
// Normally, this wouldn't be necessary, since if you know the uncompressed length, you wouldn't need to do an upperbound.
// However, S9 and S16 codings have a quirk that requires the output buffer array (to which we decompress) to have at least 28 empty slots;
// this is because there is a case where a word will have a max of 28 integers compressed within, and we will access the output buffer
// for all the 28 integers, even if some of those 28 integers are garbage which we haven't really compressed (but this was the case used to encode them).
// So we need to round the number of compressed integers to a multiple of 28 and make sure there is at least 28 extra space at the end of the array
// to ensure there is ample room for decompression. Here, we just use 32 instead of 28 for convenience, though it produces a slightly larger upperbound.
#define UncompressedOutBufferUpperbound(buffer_size) ((((buffer_size) >> 5) + 2) << 5)

// Determines size of output buffer for compression.
// Here we just double it, but it could really be a tighter bound.
#define CompressedOutBufferUpperbound(buffer_size) ((buffer_size) << 1)

#endif /* CODING_POLICY_HELPER_H_ */
