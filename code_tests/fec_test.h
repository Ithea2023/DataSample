#include "../cm256/cm256.h"



static double GetPerfFrequencyInverse();
void initializeBlocks(cm256_block originals[256], int blockCount, int blockBytes);
bool validateSolution(cm256_block_t* blocks, int blockCount, int blockBytes);
// common test
bool ExampleFileUsage();
bool ExampleFileUsage(uint32_t media_count, uint32_t recovery_count);
bool CheckMemSwap();
bool FinerPerfTimingTest();
bool BulkPerfTesting();
int fec_test();