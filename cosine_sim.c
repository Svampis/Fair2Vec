#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

const long long max_w = 50;

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Usage: %s <input.bin> <word>\n", argv[0]);
        return 1;
    }

    char *input_file = argv[1];
    char *word = argv[2];

    FILE *f = fopen(input_file, "rb");
    if (!f) {
        printf("Input file not found.\n");
        return 1;
    }

    long long words, size;
    fscanf(f, "%lld", &words);
    fscanf(f, "%lld", &size);

    char *vocab = (char *)malloc(words * max_w * sizeof(char));
    float *M = (float *)malloc(words * size * sizeof(float));
    if (!vocab || !M) {
        printf("Cannot allocate memory.\n");
        return 1;
    }

    // Load vocabulary + vectors
    for (long long b = 0; b < words; b++) {
        int a = 0;
        while (1) {
            vocab[b * max_w + a] = fgetc(f);
            if (feof(f) || vocab[b * max_w + a] == ' ') break;
            if (a < max_w && vocab[b * max_w + a] != '\n') a++;
        }
        vocab[b * max_w + a] = 0;
        fread(&M[b * size], sizeof(float), size, f);
    }
    fclose(f);

    // Find "man", "woman", and input word
    long long idx_man = -1, idx_woman = -1, idx_word = -1;
    for (long long i = 0; i < words; i++) {
        if (!strcmp(&vocab[i * max_w], "man")) idx_man = i;
        if (!strcmp(&vocab[i * max_w], "woman")) idx_woman = i;
        if (!strcmp(&vocab[i * max_w], word)) idx_word = i;
    }

    if (idx_man == -1 || idx_woman == -1 || idx_word == -1) {
        printf("Error: 'man', 'woman', or '%s' not found in vocabulary.\n", word);
        return 1;
    }

    // Compute gender direction = man - woman
    float *g = (float *)malloc(size * sizeof(float));
    for (long long i = 0; i < size; i++)
        g[i] = M[idx_man * size + i] - M[idx_woman * size + i];

    // Normalize gender direction
    float gnorm = 0;
    for (long long i = 0; i < size; i++) gnorm += g[i] * g[i];
    gnorm = sqrt(gnorm);
    for (long long i = 0; i < size; i++) g[i] /= gnorm;

    // Compute cosine similarity of input word with gender direction
    float *v = &M[idx_word * size];
    float dot = 0, vnorm = 0;
    for (long long i = 0; i < size; i++) {
        dot += v[i] * g[i];
        vnorm += v[i] * v[i];
    }
    vnorm = sqrt(vnorm);

    float cosine = dot / vnorm;
    printf("%f\n", cosine);

    free(vocab);
    free(M);
    free(g);
    return 0;
}

