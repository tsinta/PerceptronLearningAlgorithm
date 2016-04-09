/*Perceptron Learning Algorithm*/
/*Author: tsinta*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "AnalyzeTrainingData.h"
#include "PerceptronLearningAlgorithm.h"

void
setPLACommand(int argc, char *argv[], char **fileName, Bool *isStrict
    , Bool *randomTraining, Bool *showDetail)
{
    *fileName = argv[1];
    
    size_t i;
    
     for (i = 2; i < argc; ++i) {
        if (strcmp(argv[i], "strict") == 0)
            *isStrict = TRUE;
        else if (strcmp(argv[i], "random") == 0) {
            *randomTraining = TRUE;
            srand(time(NULL));
        }
        else if (strcmp(argv[i], "show") == 0)
            *showDetail = TRUE;
    }
}

int main(int argc, char *argv[])
{
    if (argc <= 1) {
        puts("No training data");
        return 0;
    }
    
    char *fileName = NULL;
    Bool isStrict = FALSE;
    Bool randomTraining = FALSE;
    Bool showDetail = FALSE;

    setPLACommand(argc, argv, &fileName, &isStrict, &randomTraining, &showDetail);
    
    int **data = NULL;
    PLAData *pData = NULL;
    Weight wt;
    size_t numData, numPLAVal;
    
    if (!initPLA(fileName, &data, &pData, &wt, &numData, &numPLAVal)) {
        puts("Can't open the file");
        return 0;
    }
    
    size_t startIdx = 0, iter = 0, countAdjust = 0;
    
    while (1) {
        printf("Enter iteration times (or ctrl+Z to exit)= ");
        if (scanf("%u", (unsigned int*)&iter) == EOF)
            break;
        countAdjust += randomTraining
            ? trainingByRandomSequence(pData, &wt, numData
                , numPLAVal, iter, isStrict, showDetail)
            : trainingByNormalSequence(pData, &wt, numData
                , numPLAVal, &startIdx, iter, isStrict, showDetail);
        
        puts("*************************");
        printf("total adjust times = %u\n", (unsigned int)countAdjust);
        if (showTrainingResult(pData, wt, numData, numPLAVal, isStrict) == numData)
            break;
    }
    closePLA(data, pData, wt, numData);
    return 0;
}
