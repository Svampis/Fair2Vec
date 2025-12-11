
for i in `seq 1 5`
do
	echo "hard debias rate=0."$i
	./hard-debias unfair_vectors.bin protected_words.txt fair_vectors2.bin 0.$i
	sum=0
	while read -r line
	do
		sum=$(echo "$sum + $(./cosine-sim fair_vectors2.bin $line)" | bc)
	done < protected_words.txt
	echo -n "Average absolute cosine similarity of protected words with gender direction vector (man-woman): "
	echo "scale=5; $sum / $(wc -l protected_words.txt | awk '{print $1}')" | bc

	echo -n "Average absolute cosine similarity between semantically adjacent pairs: "
	python3 assess_vectors.py fair_vectors2.bin adjacent_word_pairs.txt
done


