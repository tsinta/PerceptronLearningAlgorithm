/*Perceptron Learning Algorithm*/
/*Author: tsinta*/
#ifndef PERCEPTRON_LEARNING_ALGORITHM
#define PERCEPTRON_LEARNING_ALGORITHM

#ifndef ENUM_BOOL
#define ENUM_BOOL
typedef enum { FALSE, TRUE } Bool;
#endif

#ifndef ENUM_PLASTATUS
#define ENUM_PLASTATUS
typedef enum { BAD, GOOD } PLAStatus;
#endif

#ifndef STRUCT_PLADATA
#define STRUCT_PLADATA
typedef struct {
    int *val;
    PLAStatus isGood;
} PLAData;
#endif

#ifndef STRUCT_WEIGHT
#define STRUCT_WEIGHT
typedef struct {
    int *w;
    int threshold;  /*w * val > threshold -> GOOD; w * val < threshold -> BAD*/
} Weight;
#endif

Bool
initPLA(char *fileName, PLAData **pData
    , Weight *wt, size_t *numData, size_t *numPLAVal);
/*example: fileName = "data.txt", data.txt->"2 1 : 1\n1 0 : -1\n..."*/
/*return: Can the file do PLA?*/

void setShowDetail(Bool showDetail);

extern void setIsStrict(Bool isStrict);

size_t
trainingByNormalSequence(PLAData *pData, Weight *wt
    , size_t numData, size_t numPLAVal, size_t iter);
/*Training iter times from *pData[*startIdx]*/
/*If isStrict == TRUE, wt will be adjusted even if wt*val == threshold*/

size_t
trainingByRandomSequence(PLAData *pData, Weight *wt
    , size_t numData, size_t numPLAVal, size_t iter);
/*Training iter times by random*/

size_t
showTrainingResult(PLAData *pData, Weight wt, size_t numData, size_t numPLAVal);

void closePLA(PLAData *pData, Weight wt, size_t numData);
/*Must add to the end*/

#endif