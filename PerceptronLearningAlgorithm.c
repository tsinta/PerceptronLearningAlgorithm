/*Perceptron Learning Algorithm*/
/*Author: tsinta*/
#include <stdio.h>
#include <stdlib.h>
#include "ReadTrainingData.h"
#include "AnalyzeTrainingData.h"
#include "PerceptronLearningAlgorithm.h"

Bool
initPLA(char *fileName, int ***data, PLAData **pData
    , Weight *wt, size_t *numData, size_t *numPLAVal)
{
    /*out: Can the file do PLA?*/
    size_t numVal;
    *data = ReadTrainingData(fileName, numData, &numVal);
    
    if (data == NULL || numData == 0 || numVal <= 2)
        return FALSE;
    *pData = convertToPLAData(*data, *numData, numVal);
    *numPLAVal = numVal - 1;
    *wt = genInitWeight(*numPLAVal);
    return TRUE;
}

Bool oneTraining(PLAData pData, Weight *wt, size_t numPLAVal, Bool isStrict, Bool showDetail)
{
    /*out: Has wt been changed?*/
    if (checkPLAData(pData, *wt, numPLAVal, isStrict)
        || checkIfWeightIsZero(*wt, numPLAVal) == 0) {
        adjustWeight(pData, wt, numPLAVal);
        if (showDetail) {
            showPLAData(pData, numPLAVal);
            showWeight(*wt, numPLAVal);
        }
        return TRUE;
    }
    return FALSE;
}

size_t
trainingByNormalSequence(PLAData *pData, Weight *wt, size_t numData, size_t numPLAVal
    , size_t *startIdx, size_t iter, Bool isStrict, Bool showDetail)
{
    /*iter: Iteration times*/
    /*out: adjust times of weight*/
    size_t countAdjust = 0;
    
    while (iter > 0) {
        if (*startIdx == numData)
            *startIdx = 0;
        if (oneTraining(pData[(*startIdx)++], wt, numPLAVal, isStrict, showDetail))
            ++countAdjust;
        --iter;
    }
    return countAdjust;
}

size_t
trainingByRandomSequence(PLAData *pData, Weight *wt, size_t numData, size_t numPLAVal
    , size_t iter, Bool isStrict, Bool showDetail)
{
    /*out: adjust times of weight*/
    size_t idx;
    size_t countAdjust = 0;
    
    /*iter: Iteration times*/
    while (iter > 0) {
        idx = (size_t)(rand() % numData);
        if (oneTraining(pData[idx], wt, numPLAVal, isStrict, showDetail))
            ++countAdjust;
        --iter;
    }
    return countAdjust;
}

size_t
showTrainingResult(PLAData *pData, Weight wt, size_t numData, size_t numPLAVal, Bool isStrict)
{
    /*out: # of correct by training*/
    size_t nc = countNumCorrect(pData, wt, numData, numPLAVal, isStrict);
    
    showWeight(wt, numPLAVal);
    printf("correct rate: %u / %u, %g%%\n", (unsigned int)nc
            , (unsigned int)numData, (double)(nc * 100) / (double)numData);
    return nc;
}

void closePLA(int **data, Weight wt, size_t numData)
{
    closeTrainingData(data, numData);
    closeWeight(wt);
}

/*
int main()
{
    char fileName[100];
    int **data = NULL;
    PLAData *pData = NULL;
    Weight wt;
    size_t numData, numPLAVal;
    Bool isStrict = TRUE;
    Bool showDetail = TRUE;
    scanf("%s", fileName);
    initPLA(fileName, &data, &pData, &wt, &numData, &numPLAVal);
    size_t startIdx = 0, iter = 0;
    while (scanf("%u", (unsigned int*)&iter) != EOF) {
        size_t ca = trainingByNormalSequence(pData, &wt, numData
            , numPLAVal, &startIdx, iter, isStrict, showDetail);*/
        /*size_t ca = trainingByRandomSequence(pData, &wt
            , numData, numPLAVal, iter, isStrict, showDetail);*/
/*        puts("*************************");
        printf("adjust times = %u\n", (unsigned int)ca);
        if (showTrainingResult(pData, wt, numData, numPLAVal, isStrict) == numData)
            break;
    }
    closePLA(data, wt, numData);
    return 0;
}
*/
