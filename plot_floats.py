#!/usr/bin/env python3

import sys
import matplotlib.pyplot as plt

def main():
    x_values = [0.01, 0.02, 0.03, 0.04, 0.05]
    y_values = [.02029, .01965, .01634, .01193, .01705]

    # Plotting
    plt.plot(x_values, y_values, marker='o', linestyle='-')
    plt.xlabel("Debiasing rate")
    plt.ylabel("abs cosine sim")
    plt.title(f"Absolute cosine similarity with (man-woman) over debiasing rate")
    plt.grid(True)
    plt.savefig("fair2vec2_loss.png")

if __name__ == "__main__":
    main()

