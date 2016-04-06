/*Read Training Data*/
/*Author: tsinta*/
#ifndef READ_TRAINING_DATA
#define READ_TRAINING_DATA

#ifndef ENUM_BOOL
#define ENUM_BOOL
typedef enum { FALSE, TRUE } Bool;
#endif

size_t assignOneLineToInts(char *lineData, int **val, size_t numVal);
/*example: lineData = "1 2 3" -> *val = {1, 2, 3}, return 3*/
/*numVal: Use to check if it's equal to return value. it's 0 when it's first time to use*/

Bool getOneLineFromFile(FILE *f, char **lineData);
/*example: f = fopen("data,txt", "r"), data.txt:1 2 3\n4 5 6 -> *lineData = "1 2 3"*/
/*next call -> *lineData = "4 5 6"*/
/*return TRUE: no error, FALSE: error occurred*/

int** readTrainingData(char *fileName, size_t *numData, size_t *numVal);
/*example: fileName = "data.txt, data.txt:1 2 3\n4 5 6 -> *numData = 2, *numVal = 3"*/
/*return data[0] = {1, 2, 3}, data[1] = {4, 5, 6}*/

void closeTrainingData(int **data, size_t numData);

#endif