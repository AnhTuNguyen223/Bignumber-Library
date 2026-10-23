
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Đọc dữ liệu benchmark
data = pd.read_csv("benchmark.csv")

functions = [
    ("bn_mul_ns", "bn_mul()"),
    ("bn_divmod_small_ns", "bn_divmod_small()"),
    ("odd_sum_ns", "odd_sum()")
]

for column, name in functions:
    x = data["digits"].values
    y = data[column].values

    # Hồi quy tuyến tính trên dữ liệu log-log
    slope, intercept = np.polyfit(np.log(x), np.log(y), 1)

    # Vẽ biểu đồ
    plt.figure()
    plt.loglog(x, y, "o", markersize=3, label="Measured")
    plt.loglog(
        x,
        np.exp(intercept) * x**slope,
        label="Fit: O(n^{:.2f})".format(slope)
    )

    plt.xlabel("Input size (digits)")
    plt.ylabel("Execution time (ns)")
    plt.title(name + " - Log-Log Benchmark")
    plt.grid(True, which="both", linestyle="--", alpha=0.5)
    plt.legend()
    plt.tight_layout()

    # Lưu biểu đồ
    filename = column + ".png"
    plt.savefig(filename, dpi=300)
    print("{}: estimated complexity exponent = {:.3f}".format(
        name, slope
    ))

    plt.close()

print("Da luu cac bieu do.")
