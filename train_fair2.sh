make
time ./fair2vec2 -train ./dataset/text8 -output fair_vectors2.bin -cbow 1 -size 200 -window 8 -negative 25 -hs 0 -sample 1e-4 -threads 20 -binary 1 -iter 5
