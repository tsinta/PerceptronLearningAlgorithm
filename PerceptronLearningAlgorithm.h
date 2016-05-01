/*Perceptron Learning Algorithm*/
/*Author: tsinta*/
#ifndef PERCEPTRON_LEARNING_ALGORITHM
#define PERCEPTRON_LEARNING_ALGORITHM

#ifndef DEFINE_DATA_TYPE
#define DEFINE_DATA_TYPE
    #ifdef USE_DOUBLE
        typedef double DType;
    #else
        typedef int DType;
    #endif
#endif

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
    DType *val;
    PLAStatus isGood;
} PLAData;
#endif

#ifndef STRUCT_WEIGHT
#define STRUCT_WEIGHT
typedef struct {
    DType *w;
    int threshold;  /*w * val > threshold -> GOOD; w * val < threshold -> BAD*/
} Weight;
#endif

Bool initPLA(char *fileName, PLAData **pData, Weight *wt, size_t *numData, size_t *numPLAVal);
/*example: fileName = "data.txt", data.txt->"2 1 : 1\n1 0 : -1\n..."*/
/*return: Can the file do PLA?*/

void setShowDetail(Bool showDetail);

void setIsPocket(Bool isPocket);
/*if isPocket == TRUE, keeping best wt in pocket*/

extern void setIsStrict(Bool isStrict);
/*If isStrict == TRUE, wt will be adjusted even if wt*val == threshold*/

void setIsStopByAdjustTimes(Bool isStopByAdjustTimes);
/*if isStopByAdjustTimes == TRUE, one iterations == one adjust*/
/*if isStopByAdjustTimes == FALSE, one iterations == one data*/

void setIsRandomTraining(Bool isRandomTraining);

size_t
(*trainingBySequence)(PLAData *pData, Weight *wt, size_t numData, size_t numPLAVal, size_t iter);
/*if isRandomTraining == TRUE, Training iter times from *pData[*startIdx]*/
/*if isRandomTraining == FALSE, Training iter times by random*/

size_t showTrainingResult(PLAData *pData, Weight wt, size_t numData, size_t numPLAVal);

void setTestFileName(char *testFileName);

void testResult(Weight wt, size_t numPLAVal);

void closePLA(PLAData *pData, Weight wt, size_t numData);
/*Must add to the end*/

#endif