gcc -Wall -O3 -g -o knn_jowessendorf knn_jowessendorf.c -pthread -lpthread -lm
./knn_jowessendorf hsl_codebook.txt 10000 10 5 1
