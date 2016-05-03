/*Perceptron Learning Algorithm*/
/*Author: tsinta*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ReadTrainingData.h"
#include "AnalyzeTrainingData.h"
#include "PerceptronLearningAlgorithm.h"

static DType **g_data = NULL;
static Weight g_pocketWt;    /*used for pocket*/
static Bool g_isPocket = FALSE;

void setIsPocket(Bool isPocket)
{
    g_isPocket = isPocket;
}

Bool initPLA(char *fileName, PLAData **pData, Weight *wt, size_t *numData, size_t *numPLAVal)
{
    /*out: Can the file do PLA?*/
    size_t numVal;
    
    g_data = readTrainingData(fileName, numData, &numVal);
    if (g_data == NULL || numData == 0 || numVal < 2)
        return FALSE;
    *pData = convertToPLAData(g_data, *numData, numVal);
    *numPLAVal = numVal - 1;
    *wt = genInitWeight(*numPLAVal);
    if (g_isPocket)
        g_pocketWt = genInitWeight(*numPLAVal);
    return TRUE;
}

static Bool g_showDetail = FALSE;

void setShowDetail(Bool showDetail)
{
    g_showDetail = showDetail;
}

static size_t g_pocketNumCorrect = 0;

static Bool
oneTraining(PLAData pData, Weight *wt, size_t numPLAVal, PLAData *allPData, size_t numData)
{
    /*out: Has wt been changed?*/
    if (checkPLAData(pData, *wt, numPLAVal) || checkIfWeightIsZero(*wt, numPLAVal) == 0) {
        adjustWeight(pData, wt, numPLAVal);
        
        size_t nc;
        Bool updatePocket = FALSE;
            
        if (g_isPocket
            && (nc = countNumCorrect(allPData, *wt, numData, numPLAVal)) > g_pocketNumCorrect) {
            g_pocketNumCorrect = nc;
            memcpy(g_pocketWt.w, wt->w, sizeof(DType) * numPLAVal);
            g_pocketWt.threshold = wt->threshold;
            updatePocket = TRUE;
        }
        if (g_showDetail) {
            showPLAData(pData, numPLAVal);
            showWeight(*wt, numPLAVal);
            if (g_isPocket && updatePocket) {
                printf("correct rate: %u / %u, %g%%\n", (unsigned int)g_pocketNumCorrect
                    , (unsigned int)numData, (double)(g_pocketNumCorrect * 100) / (double)numData);
            }
        }
        return TRUE;
    }
    return FALSE;
}

static Bool g_isStopByAdjustTimes = FALSE;

void setIsStopByAdjustTimes(Bool isStopByAdjustTimes)
{
    g_isStopByAdjustTimes = isStopByAdjustTimes;
}

static size_t
trainingByNormalSequence(PLAData *pData, Weight *wt, size_t numData, size_t numPLAVal, size_t iter)
{
    /*iter: Iteration times*/
    /*out: adjust times of weight*/
    static size_t startIdx = 0; /*start from pData[0]*/
    static size_t countUp = 0;  /*if up to numData, end the iteration*/
    size_t countAdjust = 0;     /*count # of data used to adjust wt*/
    
    while (iter > 0) {
        if (startIdx == numData)
            startIdx = 0;
        if (oneTraining(pData[startIdx++], wt, numPLAVal, pData, numData)) {
            /*wt has been updated*/
            countUp = 0;
            ++countAdjust;
            if (g_isStopByAdjustTimes)
                --iter;
        }
        else if (++countUp == numData)
            break;
        if (!g_isStopByAdjustTimes)
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
        g_wrongDataIdx[i] = i;
    return g_wrongDataIdx;
}

static size_t
trainingByRandomSequence(PLAData *pData, Weight *wt, size_t numData, size_t numPLAVal, size_t iter)
{
    /*iter: Iteration times*/
    /*out: adjust times of weight*/
    static size_t countUp = 0;  /*if up to numData, end the iteration*/
    size_t countAdjust = 0;     /*count # of data used to adjust wt*/
    
    if (g_wrongDataIdx == NULL && resetWrongDataIdx(numData) == NULL)
        return 0;
    while (iter > 0) {
        size_t idx = (size_t)(rand() % (numData - countUp));
        
        if (oneTraining(pData[g_wrongDataIdx[idx]], wt, numPLAVal, pData, numData)) {
            /*wt has been updated*/
            countUp = 0;
            if (resetWrongDataIdx(numData) == NULL)
                return 0;
            ++countAdjust;
            if (g_isStopByAdjustTimes)
                --iter;
        }
        else if (++countUp < numData) {
            memmove(g_wrongDataIdx + idx, g_wrongDataIdx + idx + 1
                , sizeof(size_t) * (numData - countUp - idx));  /*remove correct data idx*/
        }
        else
            break;
        if (!g_isStopByAdjustTimes)
            --iter;
    }
    return countAdjust;
}

size_t
(*trainingBySequence)(PLAData *pData, Weight *wt, size_t numData, size_t numPLAVal, size_t iter)
    = trainingByNormalSequence;

void setIsRandomTraining(Bool isRandomTraining)
{
    trainingBySequence = isRandomTraining ? trainingByRandomSequence : trainingByNormalSequence;
}

size_t showTrainingResult(PLAData *pData, Weight wt, size_t numData, size_t numPLAVal)
{
    /*out: # of correct by training*/
    size_t nc = g_isPocket ? g_pocketNumCorrect : countNumCorrect(pData, wt, numData, numPLAVal);
    
    showWeight(g_isPocket ? g_pocketWt : wt, numPLAVal);
    printf("correct rate: %u / %u, %g%%\n", (unsigned int)nc
            , (unsigned int)numData, (double)(nc * 100) / (double)numData);
    return nc;
}

static char *g_testFileName = NULL;

void setTestFileName(char *testFileName)
{
    g_testFileName = testFileName;
}

void testResult(Weight wt, size_t numPLAVal)
{
    /*out: # of correct by testing*/
    if (g_testFileName == NULL)
        return;
    
    size_t numTestData, numTestVal;
    DType **testData = readTrainingData(g_testFileName, &numTestData, &numTestVal);
    
    if (testData == NULL || numTestData == 0 || numTestVal != numPLAVal + 1) {
        fprintf(stderr, "Can't read the testing file: %s\n", g_testFileName);
        return;
    }
    
    PLAData *pTestData = convertToPLAData(testData, numTestData, numTestVal);
    size_t nc = countNumCorrect(pTestData, (g_isPocket ? g_pocketWt : wt), numTestData, numPLAVal);
    
    printf("correct rate of testing data: %u / %u, %g%%\n"
    , (unsigned int)nc, (unsigned int)numTestData, (double)(nc * 100) / (double)numTestData);
    closeTrainingData(testData, numTestData);
    testData = NULL;
    closePLAData(pTestData);
    pTestData = NULL;
}

void closePLA(PLAData *pData, Weight wt, size_t numData)
{
    closeTrainingData(g_data, numData);
    g_data = NULL;
    closePLAData(pData);
    closeWeight(wt);
    if (g_wrongDataIdx != NULL) {
        free(g_wrongDataIdx);
        g_wrongDataIdx = NULL;
    }
    if (g_isPocket)
        closeWeight(g_pocketWt);
}

/*
int main()
{
    char fileName[100], testFileName[100];
    PLAData *pData = NULL;
    Weight wt;
    size_t numData, numPLAVal;
    setIsStrict(TRUE);
    setShowDetail(TRUE);
    setIsRandomTraining(TRUE);
    setIsStopByAdjustTimes(TRUE);
    setIsPocket(TRUE);
    scanf("%s %s", fileName, testFileName);
    setTestFileName(testFileName);
    initPLA(fileName, &pData, &wt, &numData, &numPLAVal);
    size_t iter = 0;
    while (scanf("%u", (unsigned int*)&iter) != EOF) {
        size_t ca = trainingBySequence(pData, &wt, numData, numPLAVal, iter);*/
        /*size_t ca = trainingByNormalSequence(pData, &wt, numData, numPLAVal, iter);*/
        /*size_t ca = trainingByRandomSequence(pData, &wt, numData, numPLAVal, iter);*/
/*        puts("*************************");
        printf("adjust times = %u\n", (unsigned int)ca);
        if (showTrainingResult(pData, wt, numData, numPLAVal) == numData)
            break;
    }
    if (g_testFileName != NULL)
        testResult(wt, numPLAVal);
    closePLA(pData, wt, numData);
    return 0;
}
*/
