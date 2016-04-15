/*Perceptron Learning Algorithm*/
/*Author: tsinta*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "PerceptronLearningAlgorithm.h"

void setPLACommand(int argc, char *argv[], char **fileName)
{
    *fileName = argv[1];
    
    size_t i;
    
     for (i = 2; i < argc; ++i) {
        if (strcmp(argv[i], "strict") == 0)
            setIsStrict(TRUE);
        else if (strcmp(argv[i], "random") == 0) {
            setIsRandomTraining(TRUE);
            srand(time(NULL));
        }
        else if (strcmp(argv[i], "show") == 0)
            setShowDetail(TRUE);
        else if (strcmp(argv[i], "adjust") == 0)
            setIsStopByAdjustTimes(TRUE);
        else if (strcmp(argv[i], "pocket") == 0)
            setIsPocket(TRUE);
    }
}

int main(int argc, char *argv[])
{
    if (argc <= 1) {
        puts("No training data");
        return 0;
    }
    
    char *fileName = NULL;

    setPLACommand(argc, argv, &fileName);
    
    PLAData *pData = NULL;
    Weight wt;
    size_t numData, numPLAVal;
    
    if (!initPLA(fileName, &pData, &wt, &numData, &numPLAVal)) {
        printf("Can't open the file \"%s\"", fileName);
        return 0;
    }
    
    size_t iter = 0, countAdjust = 0;
    
    while (1) {
        printf("Enter iteration times (or ctrl+Z to exit)= ");
        if (scanf("%u", (unsigned int*)&iter) == EOF)
            break;
        countAdjust += trainingBySequence(pData, &wt, numData, numPLAVal, iter);
        puts("*************************");
        printf("total adjust times = %u\n", (unsigned int)countAdjust);
        if (showTrainingResult(pData, wt, numData, numPLAVal) == numData)
            break;
    }
    closePLA(pData, wt, numData);
    return 0;
}
