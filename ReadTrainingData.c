/*Read Training Data*/
/*Author: tsinta*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "ReadTrainingData.h"

#define MAXDATA 100
#define MAXVAL 11
#define MAXCHAR 50

static void* revisedRealloc(void **ptr, size_t sz, char *funcName)
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

static void* revisedMalloc(size_t sz, char *funcName)
{
    void *bufptr = malloc(sz);
    if (bufptr == NULL)
        fprintf(stderr, "Failed to malloc in %s\n", funcName);
    return bufptr;
}

size_t assignOneLineToDTypes(char *lineData, DType **val, size_t numVal)
{
    /*lineData: One line of data*/
    /*val: Need to put DType from lineData, it would be malloc if it's NULL*/
    /*numVal: check the # of value. it's 0 means it's
        the first time to read, and *val must be NULL*/
    /*out: # of DType*/
    if (numVal == 0) {
        if (*val != NULL)
            free(*val);
        if ((*val = (DType*)revisedMalloc(sizeof(DType) * MAXVAL, "assignOneLineToDTypes")) == NULL)
            return 0;
    }
    
    size_t i = 0;           /*idx of lineData*/
    size_t numDTypes = 0;     /*count the # of DType in one line*/
    char c[2] = " ";        /*two continuous chars read from lineData*/
    size_t multiMaxVal = 1; /*multiple of MAXVAL*/
    
    for (i = 0; lineData[i] != '\n' && lineData[i] != '\0'; ++i) {
        if (!isdigit(c[1] = lineData[i])) {
            #ifdef USE_DOUBLE
            if (c[1] != '.') {
            #endif
            c[0] = c[1];
            continue;
            #ifdef USE_DOUBLE
            }
            #endif
        }
        if (numVal == 0 && numDTypes >= MAXVAL * multiMaxVal) {
            if (revisedRealloc((void**)val, sizeof(DType) * (MAXVAL * (++multiMaxVal))
                , "assignOneLineToDTypes") == NULL)
                return 0;
        }
        if (numVal != 0 && numDTypes >= numVal) {
            fprintf(stderr, "Too many DTypes in one line (assignOneLineToDTypes)\n");
            break;
        }
        #ifdef USE_DOUBLE
        sscanf(lineData + ((c[0] != '-') ? i : (i - 1))
            , "%lf", &((*val)[numDTypes]));    /*get DType(double) from lineData*/
            
        Bool meetDot = (c[1] != '.') ? FALSE : TRUE;
        
        c[0] = c[1] = ' ';
        #else
        sscanf(lineData + ((c[0] != '-') ? i : (i - 1))
            , "%d", &((*val)[numDTypes]));    /*get DType(int) from lineData*/
        #endif
        while (isdigit(lineData[i + 1])
            #ifdef USE_DOUBLE
            || lineData[i + 1] == '.'
            #endif
            ) {
            #ifdef USE_DOUBLE
            if (lineData[i + 1] == '.') {
                if (meetDot)
                    break;
                else
                    meetDot = TRUE;
            }
            #endif
            ++i;
        }
        ++numDTypes;
    }
    if (numVal == 0 && numDTypes < MAXVAL) {
        if (numDTypes == 0) {
            free(*val);
            *val = NULL;
        }
        else if (revisedRealloc((void**)val, sizeof(DType) * (numDTypes)
            , "assignOneLineToDTypes") == NULL)
            return 0;
    }
    if (numVal != 0 && numDTypes != 0 && numVal != numDTypes)
        fprintf(stderr, "The # of DType is different (assignOneLineToDTypes)\n");
    return numDTypes;
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

void closeTrainingData(DType **data, size_t numData)
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

static Bool allocEachData(DType **iLineData, size_t numData, size_t numVal, Bool *isNeedReuse)
{
    /*out: Is it no error occur*/
    if (numData != 0 && !*isNeedReuse) {
        *iLineData = (DType*)revisedMalloc(sizeof(DType) * numVal, "allocEachData");
        if (*iLineData == NULL)
            return FALSE;
    }
    else if (*isNeedReuse)
        *isNeedReuse = FALSE;
    else
        *iLineData = NULL;
    return TRUE;
}

static Bool
checkNumVal(DType **data, char *lineData, size_t *numData
    , size_t *numVal, size_t nowNumVal, Bool *isNeedReuse)
{
    /*out: Is it no error occur*/
    if (*numVal == 0 && nowNumVal != 0)
        *numVal = nowNumVal; /*First time to assign *numVal*/
    else if (nowNumVal == 0 && *numData != 0) {  /*No DType in current line*/
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

static DType** assignDataToVar(FILE *f, size_t *numData, size_t *numVal)
{
    /*out: The DType data in f*/
    DType **data = (DType**)revisedMalloc(sizeof(DType*) * MAXDATA, "assignDataToVar");
    
    if (data == NULL)
        return NULL;
    
    char *lineData = (char*)revisedMalloc(sizeof(char) * MAXCHAR, "assignDataToVar");
    
    if (lineData == NULL) {
        free(data);
        return NULL;
    }
    
    Bool isNeedReuse = FALSE;   /*reuse the data[*numData] when prev # of DType is 0*/
    size_t multiMaxData = 1;    /*multiple of MAXDATA*/
    
    *numData = *numVal = 0;
    while (getOneLineFromFile(f, &lineData)) {
        if (*numData >= MAXDATA * multiMaxData) {
            if(revisedRealloc((void**)&data, sizeof(DType*) * (MAXDATA * (++multiMaxData))
                , "assignDataToVar") == NULL)
                return NULL;
        }
        if (!allocEachData(&(data[*numData]), *numData, *numVal, &isNeedReuse)
            && *numData != 0 && !isNeedReuse)
            return NULL;
        
        size_t nowNumVal = assignOneLineToDTypes(lineData, &(data[*numData]), *numVal);
        
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
        else if (revisedRealloc((void**)&data, sizeof(DType*) * (*numData)
            , "assignDataToVar") == NULL)
            return 0;
    }
    free(lineData);
    return data;
}

DType** readTrainingData(char *fileName, size_t *numData, size_t *numVal)
{
    /*out: DType data in file*/
    FILE *f = fopen(fileName, "r");
    
    if (f == NULL) {
        fprintf(stderr, "Failed to read data\n");
        return NULL;
    }
    
    DType **data = assignDataToVar(f, numData, numVal);
    
    fclose(f);
    return data;
}

/*
int main()
{*/
    /*char s[100];
    fgets(s, 100, stdin);
    DType *a = NULL;
    size_t n = assignOneLineToDTypes(s, &a, 0);
    puts(s);
    printf("%u\n", (unsigned int)n);
    size_t i = 0;
    for (i = 0; i < n; ++i)
        #ifdef USE_DOUBLE
        printf("%lf , ", a[i]);
        #else
        printf("%d , ", a[i]);
        #endif
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
    DType **data = assignDataToVar(f, &numData, &numVal);
    size_t i,j;
    for (i = 0; i < numData; ++i) {
        for (j = 0; j < numVal; ++j) {
            #ifdef USE_DOUBLE
            printf("%lf , ", data[i][j]);
            #else
            printf("%d , ", data[i][j]);
            #endif
        }
        putchar('\n');
    }
    if (data != NULL)
        closeTrainingData(data, numData);
    fclose(f);*/
    /*char fileName[MAXCHAR];
    scanf("%s", fileName);
    size_t numData = 0, numVal = 0;
    DType **data = readTrainingData(fileName, &numData, &numVal);
    size_t i,j;
    for (i = 0; i < numData; ++i) {
        for (j = 0; j < numVal; ++j) {
            #ifdef USE_DOUBLE
            printf("%lf , ", data[i][j]);
            #else
            printf("%d , ", data[i][j]);
            #endif
        }
        putchar('\n');
    }
    if (data != NULL)
        closeTrainingData(data, numData);*/
/*    return 0;
}
*/
