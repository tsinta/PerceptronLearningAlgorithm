/*Analyze Training Data*/
/*Author: tsinta*/
#include <stdio.h>
#include <stdlib.h>
#include "AnalyzeTrainingData.h"

PLAData* convertToPLAData(DType **data, size_t numData, size_t numVal)
{
    /*data: input data from file*/
    /*out: Converted data*/
    size_t i;   /*idx of numData*/
    
    PLAData *pData = (PLAData*)malloc(sizeof(PLAData) * numData);
    
    if (pData == NULL) {
        fprintf(stderr, "Failed to malloc in convertToPLAData\n");
        return NULL;
    }
    for (i = 0; i < numData; ++i) {
        pData[i].val = data[i];
        pData[i].isGood = (data[i][numVal - 1] >= 0) ? GOOD : BAD;
    }
    return pData;
}

Weight genInitWeight(size_t numPLAVal)
{
    /*out: init weight*/
    size_t i;   /*idx of wt.w*/
    Weight wt;  /*init weight, it will malloc wt.w*/
    
    wt.w = (DType*)malloc(sizeof(DType) * numPLAVal);
    if(wt.w == NULL) {
        fprintf(stderr, "Failed to malloc in genInitWeight\n");
        wt.threshold = 0;
        return wt;
    }
    for (i = 0; i < numPLAVal; ++i) {
        #ifdef USE_DOUBLE
        wt.w[i] = 0.0;
        #else
        wt.w[i] = 0;
        #endif
    }
    wt.threshold = 0;
    return wt;
}

static Bool g_isStrict = FALSE;

void setIsStrict(Bool isStrict)
{
    g_isStrict = isStrict;
}

Bool checkPLAData(PLAData pData, Weight wt, size_t numPLAVal)
{
    /*return FALSE->don't need adjust, return TRUE->need adjust*/
    size_t i;   /*idx of pData.val*/
    DType innerProduct = 0;
    
    for (i = 0; i < numPLAVal; ++i)
        innerProduct += pData.val[i] * wt.w[i];
    if (innerProduct == wt.threshold)
        return g_isStrict ? TRUE : FALSE;
    return (((innerProduct > wt.threshold) ? GOOD : BAD) != pData.isGood) ? TRUE : FALSE;
}

void adjustWeight(PLAData pData, Weight *wt, size_t numPLAVal)
{
    size_t i;   /*idx of pData.val*/
    
    for (i = 0; i < numPLAVal; ++i)
        wt->w[i] += ((pData.isGood == GOOD) ? pData.val[i] : -pData.val[i]);
    wt->threshold += ((pData.isGood == GOOD) ? -1 : 1);
}

#define TOL 0.0000001

DType checkIfWeightIsZero(Weight wt, size_t numPLAVal)
{
    /*Check if the weight is 0 vector*/
    /*out: 0->0 vector, not 0->none 0 vector*/
    size_t i;   /*idx of wt.w*/
    
    for (i = 0; i < numPLAVal; ++i) {
        #ifdef USE_DOUBLE
        if (-TOL > wt.w[i] || wt.w[i] > TOL)
        #else
        if (wt.w[i] != 0)
        #endif
            return wt.w[i];
    }
    return (DType)wt.threshold;
}

size_t countNumCorrect(PLAData *pData, Weight wt, size_t numData, size_t numPLAVal)
{
    /*out: # of training result is correct*/
    if (checkIfWeightIsZero(wt, numPLAVal) == 0)
        return 0;
    
    size_t i;   /*idx of pData*/
    size_t numCorrect = 0;
    
    for (i = 0; i < numData; ++i) {
        if (!checkPLAData(pData[i], wt, numPLAVal))
            ++numCorrect;
    }
    return numCorrect;
}

void showPLAData(PLAData pData, size_t numPLAVal)
{
    if (numPLAVal == 0)
        return;
    #ifdef USE_DOUBLE
    printf("%g", pData.val[0]);
    #else
    printf("%d", pData.val[0]);
    #endif
    
    size_t i;
    
    for (i = 1; i < numPLAVal; ++i) {
        #ifdef USE_DOUBLE
        printf(", %g", pData.val[i]);
        #else
        printf(", %d", pData.val[i]);
        #endif
    }
    printf(" : %c\n", (pData.isGood ? '+' : '-'));
}

void showWeight(Weight wt, size_t numPLAVal)
{
    if (numPLAVal == 0)
        return;
    #ifdef USE_DOUBLE
    printf("%g*X1", wt.w[0]);
    #else
    printf("%d*X1", wt.w[0]);
    #endif
    
    size_t i;
    
    for (i = 1; i < numPLAVal; ++i) {
        #ifdef USE_DOUBLE
        if (wt.w[i] >= 0.0)
            printf(" + %g*X%u", wt.w[i], (unsigned int)i + 1);
        else
            printf(" - %g*X%u", -wt.w[i], (unsigned int)i + 1);
        #else
        if (wt.w[i] >= 0)
            printf(" + %d*X%u", wt.w[i], (unsigned int)i + 1);
        else
            printf(" - %d*X%u", -wt.w[i], (unsigned int)i + 1);
        #endif
    }
    printf(" > %d -> +\n", wt.threshold);
    #ifdef USE_DOUBLE
    printf("%g*X1", wt.w[0]);
    for(i = 1; i < numPLAVal; ++i) {
        if (wt.w[i] >= 0.0)
            printf(" + %g*X%u", wt.w[i], (unsigned int)i + 1);
        else
            printf(" - %g*X%u", -wt.w[i], (unsigned int)i + 1);
    }
    #else
    printf("%d*X1", wt.w[0]);
    for(i = 1; i < numPLAVal; ++i) {
        if (wt.w[i] >= 0)
            printf(" + %d*X%u", wt.w[i], (unsigned int)i + 1);
        else
            printf(" - %d*X%u", -wt.w[i], (unsigned int)i + 1);
    }
    #endif
    printf(" < %d -> -\n", wt.threshold);
}

void closePLAData(PLAData *pData)
{
    if (pData != NULL)
        free(pData);
}

void closeWeight(Weight wt)
{
    if (wt.w != NULL) {
        free(wt.w);
        wt.w = NULL;
    }
}

/*
#include "ReadTrainingData.h"

int main()
{
    char fileName[100];
    scanf("%s", fileName);
    size_t numData = 0, numVal = 0;
    DType **data = readTrainingData(fileName, &numData, &numVal);
    PLAData *pData = convertToPLAData(data, numData, numVal);
    size_t i,j;
    setIsStrict(TRUE);
    for (i = 0; i < numData; ++i) {
        for (j = 0; j < numVal - 1; ++j) {
            #ifdef USE_DOUBLE
            printf("%g , ", pData[i].val[j]);
            #else
            printf("%d , ", pData[i].val[j]);
            #endif
        }
        printf("%c\n", (pData[i].isGood == GOOD) ? '+' : '-');
    }
    size_t numPLAVal = numVal - 1;
    int idx;
    Weight wt = genInitWeight(numPLAVal);
    while(1) {
        showWeight(wt, numPLAVal);
        scanf("%d", &idx);
        if (idx >= numData)
            break;
        Bool check = 0;
        printf("check = %d\n", (check = checkPLAData(pData[idx], wt, numPLAVal)));
        #ifdef USE_DOUBLE
        double zeroWeightCheck;
        #endif
        if (check 
            || 
            #ifdef USE_DOUBLE
            (-TOL < (zeroWeightCheck = checkIfWeightIsZero(wt, numPLAVal))
                && zeroWeightCheck < TOL)
            #else
            (checkIfWeightIsZero(wt, numPLAVal) == 0)
            #endif
            ) {
            showPLAData(pData[idx], numPLAVal);
            adjustWeight(pData[idx], &wt, numPLAVal);
        }
    }
    size_t nc = countNumCorrect(pData, wt, numData, numPLAVal);
    printf("correct: %u / %u, %g%%\n", (unsigned int)nc
        , (unsigned int)numData, (double)(nc * 100) / (double)numData);
    closePLAData(pData);
    closeWeight(wt);
    if (data != NULL)
        closeTrainingData(data, numData);
    return 0;
}
*/
