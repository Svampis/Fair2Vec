#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#if defined(__MACH__)
#include <stdlib.h>
#else
#include <malloc.h>
#endif

const long long max_w = 50;

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Usage: %s <word-vector-file> <word>\n", argv[0]);
        return 1;
    }

    char *file_name = argv[1];
    char *word = argv[2];

    FILE *f = fopen(file_name, "rb");
    if (f == NULL) {
        printf("Input file not found\n");
        return 1;
    }

    long long words, size;
    fscanf(f, "%lld", &words);
    fscanf(f, "%lld", &size);

    char *vocab = (char *)malloc(words * max_w * sizeof(char));
    float *M = (float *)malloc(words * size * sizeof(float));
    if (!vocab || !M) {
        printf("Cannot allocate memory\n");
        return 1;
    }

    // Load vocabulary and normalize vectors
    for (long long b = 0; b < words; b++) {
        int a = 0;
        while (1) {
            vocab[b * max_w + a] = fgetc(f);
            if (feof(f) || vocab[b * max_w + a] == ' ') break;
            if (a < max_w && vocab[b * max_w + a] != '\n') a++;
        }
        vocab[b * max_w + a] = 0;
        for (a = 0; a < size; a++) fread(&M[a + b * size], sizeof(float), 1, f);

        // normalize vector
        float len = 0;
        for (a = 0; a < size; a++) len += M[a + b * size] * M[a + b * size];
        len = sqrt(len);
        for (a = 0; a < size; a++) M[a + b * size] /= len;
    }
    fclose(f);

    // Find indices of the input word, "man", and "woman"
    long long idx_word = -1, idx_man = -1, idx_woman = -1;
    for (long long b = 0; b < words; b++) {
        if (strcmp(&vocab[b * max_w], word) == 0) idx_word = b;
        if (strcmp(&vocab[b * max_w], "man") == 0) idx_man = b;
        if (strcmp(&vocab[b * max_w], "woman") == 0) idx_woman = b;
    }

    if (idx_word == -1 || idx_man == -1 || idx_woman == -1) {
        printf("One or more words ('%s', 'man', 'woman') not found in vocabulary.\n", word);
        free(vocab);
        free(M);
        return 1;
    }

    // Compute gender direction vector: man - woman
    float *g = (float *)malloc(size * sizeof(float));
    for (long long i = 0; i < size; i++) {
        g[i] = M[i + idx_man * size] - M[i + idx_woman * size];
    }

    // Normalize gender direction
    float g_len = 0;
    for (long long i = 0; i < size; i++) g_len += g[i] * g[i];
    g_len = sqrt(g_len);
    for (long long i = 0; i < size; i++) g[i] /= g_len;

    // Compute cosine similarity between the input word and gender direction
    float cosine = 0;
    for (long long i = 0; i < size; i++) {
        cosine += M[i + idx_word * size] * g[i];
    }

    printf("%f\n", fabs(cosine));

    free(vocab);
    free(M);
    free(g);
    return 0;
}

