gcc -Wall -O3 -g -o knn_jowessendorf knn_jowessendorf.c -pthread -lpthread -lm
./knn_jowessendorf hsl_codebook.txt 100000 10 5 12
