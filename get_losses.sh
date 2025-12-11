#!/bin/bash

gcc fair2vec.c -lm -lpthread -O3 -o fair2vec
gcc fair2vec2.c -lm -lpthread -O3 -o fair2vec2

for i in `seq 1 5`
do
	echo "fair2vec debias rate=0."$i
	sed -i '26s/.*/#define DEBIASING_RATE 0.'"$i"'/' fair2vec.c
	./fair2vec -train ./dataset/text8 -output fair_vectors.bin -cbow 1 -size 200 -window 8 -negative 25 -hs 0 -sample 1e-4 -threads 20 -binary 1 -iter 1
	echo "Done"
	sum=0
	while read -r line
	do
		sum=$(echo "$sum + $(./cosine-sim fair_vectors.bin $line)" | bc)
	done < protected_words.txt
	echo -n "Average: "
	echo "scale=5; $sum / $(wc -l protected_words.txt | awk '{print $1}')" | bc
	echo -n "Average absolute cosine similarity between semantically adjacent pairs: "
	python3 assess_vectors.py fair_vectors.bin adjacent_word_pairs.txt
done



