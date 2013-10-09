We define a base class in the coding component. It defines the abstract interface for higher level. We define two functions

Int Compression (unsigned* input, unsigned* output, int size);
It compressed the numbers into compressed form.
Input: it is the input buffer which consists the numbers need compressing.
Output: output the buffer of compressed numbers.
Size: the number of numbers that needs to be compressed.

Int Decompression (unsigned* input, unsigned* output, int size);
Input the compressed numbers and the function decompresses the numbers
Input: the compressed numbers in the buffer that needs to be decompressed.
Output: the uncompressed output buffer.
Size: the number of compressed numbers to be decompressed.
The function returns the input offset in the decompression phase.

There are also some userful bitwise operating functions in the basic. If you would like to implement your own coding method, you could use our highly hacked bitwise coding function to complete your implementation

In our implementation, we have implemented six basic compression method, Vbyte, Rice, TuboRice, S16, S9 and pfordelta. All the coding methods are implemented as blockwise. In our implementation, each block has a fixed number but the size is not fixed. These methods are hacked by our group and can work efficiently 


1.2.1 Rice Coding
In rice coding, we calculate b for each block in which size is defined by the user, we write b for each block in front of each chunk buffer followed by the compressed data. Basic operations such as Readbit and writebit are defined in this class

1.2.2 TuboRice Coding
TuboRice is a fast rice coding method based on the pfordelta and rice coding.

1.2.2 pfordelta coding
In pfordelta, we write the flag data which including information on how many bits per docid and the exception. in front of each chunk buffer. We remain the flag data uncompressed. Currently, pfordelta block size is only set to some certain number. Such as 32, 64, 128, 256. For the last block in which the size may not less than the block size. We do not pad 0s after real numbers. Instead, we take it as a whole block to compress by using readbit/writebit operation.

Limitation: for users, you need to take care of the whole inverted list. In the Compression function, we do not care about the size. If the size is too large, the coding method with automatically set the block size as 256 and discards the rest of the numbers.

1.2.3 Vbyte coding
Vbyte coding is the coding method that could be compressed and decompressed one by one. The compression and decompression method invokes vbyte encode and decode repeatedly.

1.2.4	S9/S16 coding
In each block, actually, we do not need to pad the last word with zero entries, because the coding method knows the block size and in the implementation, the decompression method will know where should stop.
