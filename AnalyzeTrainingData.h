/*Analyze Training Data*/
/*Author: tsinta*/
#ifndef ANALYZE_TRAINING_DATA
#define ANALYZE_TRAINING_DATA

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

PLAData* convertToPLAData(DType **data, size_t numData, size_t numVal);
/*example: data = {1,2,1} => pData = {1,2,GOOD}*/

Weight genInitWeight(size_t numPLAVal);
/*malloc weight and set as 0 vector*/

void setIsStrict(Bool isStrict);

Bool checkPLAData(PLAData pData, Weight wt, size_t numPLAVal);
/*example: PLAData->{1, 2, GOOD}, wt->{-1, 1, threshold=-1} => 1*-1+2*1>-1, correct*/
/*return FALSE->don't need adjust, return TRUE->need adjust*/

void adjustWeight(PLAData pData, Weight *wt, size_t numPLAVal);
/*adjust weight by pData*/

DType checkIfWeightIsZero(Weight wt, size_t numPLAVal);
/*return 0->0 vector, return not 0->none 0 vector*/

size_t countNumCorrect(PLAData *pData, Weight wt, size_t numData, size_t numPLAVal);
/*return # of training result is correct*/

void showPLAData(PLAData pData, size_t numPLAVal);

void showWeight(Weight wt, size_t numPLAVal);

void closePLAData(PLAData *pData);

void closeWeight(Weight wt);

#endif