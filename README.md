##Using perceptron learning algorithm to train any linear classfied input file

####Build Command:
C:\...\> gcc -Wall -ansi -static -o runPLA.exe *.c

####Run:
C:\...\>  runPLA.exe data.txt

data.txt <-file name

#####Some key words can be added behind the file name:

random: training by random sequence

strict: strict training (If isStrict == TRUE, wt will be adjusted even if wt*val == threshold)

show: show the data which use to adjust weight

adjust: one iteration is equal to one adjust time, not one data