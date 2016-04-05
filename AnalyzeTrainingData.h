/*Analyze Training Data*/
/*Author: tsinta*/
#ifndef ANALYZE_TRAINING_DATA
#define ANALYZE_TRAINING_DATA

#ifndef ENUM_BOOL
#define ENUM_BOOL
typedef enum { FALSE, TRUE } Bool;
#endif

typedef enum { BAD, GOOD } PLAStatus;

typedef struct {
    int *val;
    PLAStatus isGood;
} PLAData;

typedef struct {
    int *w;
    int threshold;  /*w * val > threshold -> GOOD; w * val < threshold -> BAD*/
} Weight;

PLAData* convertToPLAData(int **data, size_t numData, size_t numVal);
/*example: data = {1,2,1} => pData = {1,2,GOOD}*/

Weight genInitWeight(size_t numPLAVal);
/*malloc weight and set as 0 vector*/

Bool checkPLAData(PLAData pData, Weight wt, size_t numPLAVal, Bool isStrict);
/*example: PLAData->{1, 2, GOOD}, wt->{-1, 1, threshold=-1} => 1*-1+2*1>-1, correct*/
/*return FALSE->don't need adjust, return TRUE->need adjust*/

void adjustWeight(PLAData pData, Weight *wt, size_t numPLAVal);
/*adjust weight by pData*/

int checkIfWeightIsZero(Weight wt, size_t numPLAVal);
/*return 0->0 vector, return not 0->none 0 vector*/

size_t countNumCorrect(PLAData *pData, Weight wt, size_t numData, size_t numPLAVal, Bool isStrict);
/*return # of training result is correct*/

void showPLAData(PLAData pData, size_t numPLAVal);

void showWeight(Weight wt, size_t numPLAVal);

void closeWeight(Weight wt);

#endif