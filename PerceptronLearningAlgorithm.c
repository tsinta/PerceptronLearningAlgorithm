/*Perceptron Learning Algorithm*/
/*Author: tsinta*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ReadTrainingData.h"
#include "AnalyzeTrainingData.h"
#include "PerceptronLearningAlgorithm.h"

static int **g_data = NULL;

Bool
initPLA(char *fileName, PLAData **pData
    , Weight *wt, size_t *numData, size_t *numPLAVal)
{
    /*out: Can the file do PLA?*/
    size_t numVal;
    
    g_data = readTrainingData(fileName, numData, &numVal);
    if (g_data == NULL || numData == 0 || numVal < 2)
        return FALSE;
    *pData = convertToPLAData(g_data, *numData, numVal);
    *numPLAVal = numVal - 1;
    *wt = genInitWeight(*numPLAVal);
    return TRUE;
}

static Bool g_showDetail;

void setShowDetail(Bool showDetail)
{
    g_showDetail = showDetail;
}

static Bool
oneTraining(PLAData pData, Weight *wt, size_t numPLAVal)
{
    /*out: Has wt been changed?*/
    if (checkPLAData(pData, *wt, numPLAVal)
        || checkIfWeightIsZero(*wt, numPLAVal) == 0) {
        adjustWeight(pData, wt, numPLAVal);
        if (g_showDetail) {
            showPLAData(pData, numPLAVal);
            showWeight(*wt, numPLAVal);
        }
        return TRUE;
    }
    return FALSE;
}

size_t
trainingByNormalSequence(PLAData *pData, Weight *wt, size_t numData, size_t numPLAVal, size_t iter)
{
    /*iter: Iteration times*/
    /*out: adjust times of weight*/
    static size_t startIdx = 0; /*start from pData[0]*/
    static size_t countUp = 0;  /*if up to numData, end the iteration*/
    size_t countAdjust = 0;     /*count # of data used to adjust weight*/
    
    while (iter > 0) {
        if (startIdx == numData)
            startIdx = 0;
        if (oneTraining(pData[startIdx++], wt, numPLAVal)) {
            countUp = 0;
            ++countAdjust;
        }
        else if (++countUp == numData)
            break;
        --iter;
    }
    return countAdjust;
}

static size_t *g_wrongDataIdx = NULL;   /*idx of check result is wrong*/

static size_t* resetWrongDataIdx(size_t numData)
{
    /*out: array of wrong data idx*/
    if (g_wrongDataIdx == NULL)
        g_wrongDataIdx = (size_t*)malloc(sizeof(size_t) * numData);    
    if (g_wrongDataIdx == NULL) {
        fprintf(stderr, "Failed to malloc in resetBadDataIdx\n");
        return NULL;
    }
    
    size_t i;
    
    for (i = 0; i < numData; ++i)
        (g_wrongDataIdx)[i] = i;
    return g_wrongDataIdx;
}

size_t
trainingByRandomSequence(PLAData *pData, Weight *wt, size_t numData, size_t numPLAVal, size_t iter)
{
    /*iter: Iteration times*/
    /*out: adjust times of weight*/
    static size_t countUp = 0;  /*if up to numData, end the iteration*/
    size_t countAdjust = 0;     /*count # of data used to adjust weight*/
    size_t idx;
    
    if (g_wrongDataIdx == NULL && resetWrongDataIdx(numData) == NULL)
        return 0;
    while (iter > 0) {
        idx = (size_t)(rand() % (numData - countUp));
        if (oneTraining(pData[g_wrongDataIdx[idx]], wt, numPLAVal)) {
            countUp = 0;
            if (resetWrongDataIdx(numData) == NULL)
                return 0;
            ++countAdjust;
        }
        else if (++countUp < numData) {
            memmove(g_wrongDataIdx + idx, g_wrongDataIdx + idx + 1
                , sizeof(size_t) * (numData - countUp - idx));  /*remove correct data idx*/
        }
        else
            break;
        --iter;
    }
    return countAdjust;
}

size_t
showTrainingResult(PLAData *pData, Weight wt, size_t numData, size_t numPLAVal)
{
    /*out: # of correct by training*/
    size_t nc = countNumCorrect(pData, wt, numData, numPLAVal);
    
    showWeight(wt, numPLAVal);
    printf("correct rate: %u / %u, %g%%\n", (unsigned int)nc
            , (unsigned int)numData, (double)(nc * 100) / (double)numData);
    return nc;
}

void closePLA(PLAData *pData, Weight wt, size_t numData)
{
    closeTrainingData(g_data, numData);
    closePLAData(pData);
    closeWeight(wt);
    if (g_wrongDataIdx != NULL) {
        free(g_wrongDataIdx);
        g_wrongDataIdx = NULL;
    }
}

/*
int main()
{
    char fileName[100];
    PLAData *pData = NULL;
    Weight wt;
    size_t numData, numPLAVal;
    setIsStrict(TRUE);
    setShowDetail(TRUE);
    scanf("%s", fileName);
    initPLA(fileName, &pData, &wt, &numData, &numPLAVal);
    size_t iter = 0;
    while (scanf("%u", (unsigned int*)&iter) != EOF) {
        size_t ca = trainingByNormalSequence(pData, &wt, numData
            , numPLAVal, iter);*/
        /*size_t ca = trainingByRandomSequence(pData, &wt
            , numData, numPLAVal, iter);*/
/*        puts("*************************");
        printf("adjust times = %u\n", (unsigned int)ca);
        if (showTrainingResult(pData, wt, numData, numPLAVal) == numData)
            break;
    }
    closePLA(pData, wt, numData);
    return 0;
}
*/
