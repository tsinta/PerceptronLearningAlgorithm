/*Read Training Data*/
/*Author: tsinta*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "ReadTrainingData.h"

#define MAXDATA 100
#define MAXVAL 11
#define MAXCHAR 50

void* revisedRealloc(void **ptr, size_t sz, char *funcName)
{
    void *bufptr = realloc(*ptr, sz);
    if (bufptr == NULL) {
        fprintf(stderr, "Failed to realloc in %s\n", funcName);
        free(*ptr);
        *ptr = NULL;
    }
    else
        *ptr = bufptr;
    return bufptr;
}

void* revisedMalloc(size_t sz, char *funcName)
{
    void *bufptr = malloc(sz);
    if (bufptr == NULL)
        fprintf(stderr, "Failed to malloc in %s\n", funcName);
    return bufptr;
}

size_t assignOneLineToInts(char *lineData, int **val, size_t numVal)
{
    /*lineData: One line of data*/
    /*val: Need to put int from lineData, it would be malloc if it's NULL*/
    /*numVal: check the # of value. it's 0 means it's
        the first time to read, and *val must be NULL*/
    /*out: # of int*/
    if (numVal == 0) {
        if (*val != NULL)
            free(*val);
        if ((*val = (int*)revisedMalloc(sizeof(int) * MAXVAL, "assignOneLineToInts")) == NULL)
            return 0;
    }
    
    size_t i = 0;           /*idx of lineData*/
    size_t numInts = 0;     /*count the # of int in one line*/
    char c[2] = " ";        /*two continuous chars read from lineData*/
    size_t multiMaxVal = 1; /*multiple of MAXVAL*/
    
    for (i = 0; lineData[i] != '\n' && lineData[i] != '\0'; ++i) {
        if (!isdigit(c[1] = lineData[i])) {
            c[0] = c[1];
            continue;
        }
        if (numVal == 0 && numInts >= MAXVAL * multiMaxVal) {
            if (revisedRealloc((void**)val, sizeof(int) * (MAXVAL * (++multiMaxVal))
                , "assignOneLineToInts") == NULL)
                return 0;
        }
        if (numVal != 0 && numInts >= numVal) {
            fprintf(stderr, "Too many ints in one line (assignOneLineToInts)\n");
            break;
        }
        sscanf(lineData + ((c[0] != '-') ? i : (i - 1))
            , "%d", &((*val)[numInts]));    /*get int from lineData*/
        while (isdigit(lineData[i + 1]))
            ++i;
        ++numInts;
    }
    if (numVal == 0 && numInts < MAXVAL) {
        if (numInts == 0) {
            free(*val);
            *val = NULL;
        }
        else if (revisedRealloc((void**)val, sizeof(int) * (numInts)
            , "assignOneLineToInts") == NULL)
            return 0;
    }
    if (numVal != 0 && numInts != 0 && numVal != numInts)
        fprintf(stderr, "The # of int is different (assignOneLineToInts)\n");
    return numInts;
}

Bool getOneLineFromFile(FILE *f, char **lineData)
{
    /*lineData: One line of data*/
    /*out: Isn't it read to EOF*/
    int c;                      /*char read from f*/
    size_t i = 0;               /*# of char have been read*/
    size_t multiMaxChar = 1;    /*multiple of MAXCHAR*/
    
    while ((c = getc(f)) != '\0' && c != '\n' && c != EOF) {
        (*lineData)[i++] = c;   /*Assign char to lineData*/
        if (i >= MAXCHAR * multiMaxChar) {
            if (revisedRealloc((void**)lineData, sizeof(char) * (MAXCHAR * (++multiMaxChar))
                , "getOneLineFromFile") == NULL)
                return FALSE;
        }
    }
    (*lineData)[i] = '\0';
    return (c != EOF || i != 0) ? TRUE : FALSE;
}

void closeTrainingData(int **data, size_t numData)
{
    size_t i = 0;
    
    if (data == NULL)
        return;
    for (i = 0; i < numData; ++i) {
        if (data[i] != NULL);
            free(data[i]);
    }
    free(data);
}

Bool allocEachData(int **iLineData, size_t numData, size_t numVal, Bool *isNeedReuse)
{
    /*out: Is it no error occur*/
    if (numData != 0 && !*isNeedReuse) {
        *iLineData = (int*)revisedMalloc(sizeof(int) * numVal, "allocEachData");
        if (*iLineData == NULL)
            return FALSE;
    }
    else if (*isNeedReuse)
        *isNeedReuse = FALSE;
    else
        *iLineData = NULL;
    return TRUE;
}

Bool
checkNumVal(int **data, char *lineData, size_t *numData
    , size_t *numVal, size_t nowNumVal, Bool *isNeedReuse)
{
    /*out: Is it no error occur*/
    if (*numVal == 0 && nowNumVal != 0)
        *numVal = nowNumVal; /*First time to assign *numVal*/
    else if (nowNumVal == 0 && *numData != 0) {  /*No int in current line*/
        *isNeedReuse = TRUE;
        --*numData;
    }
    else if (*numVal != nowNumVal){
        fprintf(stderr, "The data is wrong in data %u\n", (unsigned int)*numData + 1);
        closeTrainingData(data, *numData + 1);
        free(lineData);
        *numData = *numVal = 0;
        return FALSE;
    }
    return TRUE;
}

int** assignDataToVar(FILE *f, size_t *numData, size_t *numVal)
{
    /*out: The int data in f*/
    int **data = (int**)revisedMalloc(sizeof(int*) * MAXDATA, "assignDataToVar");
    
    if (data == NULL)
        return NULL;
    
    char *lineData = (char*)revisedMalloc(sizeof(char) * MAXCHAR, "assignDataToVar");
    
    if (lineData == NULL) {
        free(data);
        return NULL;
    }
    
    Bool isNeedReuse = FALSE;   /*reuse the data[*numData] when prev # of int is 0*/
    size_t multiMaxData = 1;    /*multiple of MAXDATA*/
    
    *numData = *numVal = 0;
    while (getOneLineFromFile(f, &lineData)) {
        if (*numData >= MAXDATA * multiMaxData) {
            if(revisedRealloc((void**)&data, sizeof(int*) * (MAXDATA * (++multiMaxData))
                , "assignDataToVar") == NULL)
                return NULL;
        }
        if (!allocEachData(&(data[*numData]), *numData, *numVal, &isNeedReuse)
            && *numData != 0 && !isNeedReuse)
            return NULL;
        
        size_t nowNumVal = assignOneLineToInts(lineData, &(data[*numData]), *numVal);
        
        if (!checkNumVal(data, lineData, numData, numVal, nowNumVal, &isNeedReuse))
            return (data = NULL);
        if (*numVal == 0 && *numData == 0)
            continue;
        ++*numData;
    }
    if (*numData < MAXDATA) {
        if (*numData == 0) {
            fprintf(stderr, "No data in file\n");
            closeTrainingData(data, *numData);
            data = NULL;
        }
        else if (revisedRealloc((void**)&data, sizeof(int*) * (*numData)
            , "assignDataToVar") == NULL)
            return 0;
    }
    free(lineData);
    return data;
}

int** ReadTrainingData(char *fileName, size_t *numData, size_t *numVal)
{
    /*out: int data in file*/
    FILE *f = fopen(fileName, "r");
    
    if (f == NULL) {
        fprintf(stderr, "Failed to read data\n");
        return NULL;
    }
    
    int **data = assignDataToVar(f, numData, numVal);
    
    fclose(f);
    return data;
}

/*
int main()
{*/
    /*char s[100];
    fgets(s, 100, stdin);
    int *a = NULL;
    size_t n = assignOneLineToInts(s, &a, 0);
    puts(s);
    printf("%u\n", (unsigned int)n);
    size_t i = 0;
    for (i = 0; i < n; ++i)
        printf("%d , ", a[i]);
    free(a);*/
    /*char fileName[MAXCHAR], *lineData;
    lineData = (char*)malloc(sizeof(char)*MAXCHAR);
    scanf("%s", fileName);
    FILE *f = fopen(fileName, "r");
    if (f == NULL) {puts("failed");free(lineData);return 0;}
    while (getOneLineFromFile(f, &lineData))
        puts(lineData);
    free(lineData);
    fclose(f);*/
    /*char fileName[MAXCHAR];
    scanf("%s", fileName);
    FILE *f = fopen(fileName, "r");
    size_t numData = 0, numVal = 0;
    int **data = assignDataToVar(f, &numData, &numVal);
    size_t i,j;
    for (i = 0; i < numData; ++i) {
        for (j = 0; j < numVal; ++j)
            printf("%d , ", data[i][j]);
        putchar('\n');
    }
    if (data != NULL)
        closeTrainingData(data, numData);
    fclose(f);*/
    /*char fileName[MAXCHAR];
    scanf("%s", fileName);
    size_t numData = 0, numVal = 0;
    int **data = ReadTrainingData(fileName, &numData, &numVal);
    size_t i,j;
    for (i = 0; i < numData; ++i) {
        for (j = 0; j < numVal; ++j)
            printf("%d , ", data[i][j]);
        putchar('\n');
    }
    if (data != NULL)
        closeTrainingData(data, numData);*/
/*    return 0;
}
*/
