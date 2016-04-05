/*Perceptron Learning Algorithm*/
/*Author: tsinta*/
#ifndef PERCEPTRON_LEARNING_ALGORITHM
#define PERCEPTRON_LEARNING_ALGORITHM

#ifndef ENUM_BOOL
#define ENUM_BOOL
typedef enum { FALSE, TRUE } Bool;
#endif

Bool
initPLA(char *fileName, int ***data, PLAData **pData
    , Weight *wt, size_t *numData, size_t *numPLAVal);
/*example: fileName = "data.txt", data.txt->"2 1 : 1\n1 0 : -1\n..."*/
/*return: Can the file do PLA?*/

size_t
trainingByNormalSequence(PLAData *pData, Weight *wt, size_t numData, size_t numPLAVal
    , size_t *startIdx, size_t iter, Bool isStrict, Bool showDetail);
/*Training iter times from *pData[*startIdx]*/
/*If isStrict == TRUE, wt will be adjusted even if wt*val == threshold*/

size_t
trainingByRandomSequence(PLAData *pData, Weight *wt, size_t numData, size_t numPLAVal
    , size_t iter, Bool isStrict, Bool showDetail);
/*Training iter times by random*/

size_t
showTrainingResult(PLAData *pData, Weight wt, size_t numData, size_t numPLAVal, Bool isStrict);

void closePLA(int **data, Weight wt, size_t numData);
/*Must add to the end*/

#endif