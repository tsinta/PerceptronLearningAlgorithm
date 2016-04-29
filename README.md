##Using perceptron learning algorithm to train any linear separable input file

####Build Command:
#####Integer data:

C:\...\> gcc -Wall -ansi -static -o runPLA.exe *.c

#####Floating point data:

C:\...\> gcc -Wall -ansi -DUSE_DOUBLE -static -o runPLA.exe *.c

####Run:
C:\...\>  runPLA.exe data.txt

data.txt <-file name

#####Some keywords can be added behind the file name:

random: training by random sequence

strict: strict training (If isStrict == TRUE, wt will be adjusted even if wt*val == threshold)

show: show the data which use to adjust weight

adjust: one iteration is equal to one adjust time, not one data

pocket: keeping the best weight in pocket(the weight will be reset if it can't find better weight)

#####For example:
C:\...\>  runPLA.exe data.txt random strict show adjust

C:\...\>  runPLA.exe data2.txt random show adjust

C:\...\>  runPLA.exe data3.txt strict show adjust

#####Code hierarchy:
1. PerceptronLearningAlgorithm <- ReadTrainingData, AnalyzeTrainingData

2. runPLA <- PerceptronLearningAlgorithm