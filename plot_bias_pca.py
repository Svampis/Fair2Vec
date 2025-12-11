import struct
import numpy as np
import matplotlib.pyplot as plt
from sklearn.decomposition import PCA

# ---------------------------------------------------------
# CONFIGURATION
# ---------------------------------------------------------
TARGET_WORDS = [
    "doctor", "engineer", "scientist", "teacher", "mathematician",
    "man", "woman", "he", "she", "male", "female",
    "boy", "girl", "father", "mother", "nurse",
    "cashier", "lawyer", "surgeon", "ceo", "librarian"
]

MAX_W = 50  # max word length in word2vec binaries


# ---------------------------------------------------------
# LOAD WORD2VEC BINARY FILE
# ---------------------------------------------------------
def load_word2vec_bin(filename):
    with open(filename, 'rb') as f:
        # Read header: number of words and vector dimension
        header = f.readline().decode("utf-8")
        vocab_size, vector_size = map(int, header.strip().split())
        print(f"Vocabulary size = {vocab_size}, vector size = {vector_size}")

        vocab = []
        vectors = np.zeros((vocab_size, vector_size), dtype=np.float32)

        for i in range(vocab_size):
            # Read word
            word_bytes = []
            while True:
                ch = f.read(1)
                if ch == b' ':
                    break
                if ch != b'\n':
                    word_bytes.append(ch)
            word = b''.join(word_bytes).decode("utf-8", errors="ignore")
            vocab.append(word)

            # Read vector
            vec = np.fromfile(f, dtype=np.float32, count=vector_size)
            vectors[i] = vec

        return vocab, vectors


# ---------------------------------------------------------
# MAIN: LOAD, FILTER, PCA, PLOT
# ---------------------------------------------------------
def main(model_path):
    vocab, vectors = load_word2vec_bin(model_path)

    # Normalize vectors
    norms = np.linalg.norm(vectors, axis=1, keepdims=True)
    vectors = vectors / norms

    # Look up vectors for target words
    selected_words = []
    selected_vectors = []

    for w in TARGET_WORDS:
        if w in vocab:
            idx = vocab.index(w)
            selected_words.append(w)
            selected_vectors.append(vectors[idx])
        else:
            print(f"Warning: '{w}' not found in vocabulary.")

    selected_vectors = np.vstack(selected_vectors)

    # PCA to 2D
    pca = PCA(n_components=2)
    projected = pca.fit_transform(selected_vectors)

    # Plotting
    plt.figure(figsize=(10, 8))
    x = projected[:, 0]
    y = projected[:, 1]

    plt.scatter(x, y)

    for i, word in enumerate(selected_words):
        plt.annotate(word, (x[i] + 0.005, y[i] + 0.005))

    plt.title("PCA Projection of Profession & Gender Words")
    plt.xlabel("PC 1")
    plt.ylabel("PC 2")
    plt.xlim(-1, 1)
    plt.ylim(-1, 1)
    plt.grid(True)
    plt.tight_layout()
    plt.savefig("out.png")


# ---------------------------------------------------------
# ENTRY POINT
# ---------------------------------------------------------
if __name__ == "__main__":
    import sys
    if len(sys.argv) != 2:
        print("Usage: python plot_bias_pca.py <vectors.bin>")
        exit(1)

    model_path = sys.argv[1]
    main(model_path)

