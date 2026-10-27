
import re
import subprocess
import sys

PROGRAM = "./odd_sum"
INPUT_FILE = "input_edge.txt"


def expected_result(n, power):
    k = (n + 1) // 2

    if power == 1:
        return k * k
    elif power == 2:
        return k * (4 * k * k - 1) // 3
    elif power == 3:
        return k * k * (2 * k * k - 1)

    raise ValueError("Power không hợp lệ")


def run_c_program(number, power):
    with open(INPUT_FILE, "w") as f:
        f.write(number + "\n")

    result = subprocess.run(
        [PROGRAM, INPUT_FILE, "--power", str(power)],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        universal_newlines=True,
        timeout=60
    )

    if result.returncode != 0:
        raise RuntimeError(
            "Return code: {}\n{}".format(
                result.returncode, result.stderr
            )
        )

    matches = re.findall(
        r"BIGNUMBER odd sum:\s*(\d+)",
        result.stdout
    )

    if not matches:
        raise RuntimeError(
            "Không tìm thấy kết quả:\n{}".format(result.stdout)
        )

    return int(matches[-1])


def main():
    # Các giá trị nhỏ và trường hợp biên.
    test_cases = [
        "0",
        "1",
        "2",
        "3",
        "8",
        "9",
        "10",
        "11",
        "99",
        "100",
        "101",

        # Ranh giới limb với cơ số 10^9.
        "999999999",
        "1000000000",
        "1000000001",
        "999999999999999999",
        "1000000000000000000",
        "1000000000000000001",

        # Nhiều chữ số 0 ở giữa hoặc cuối.
        "1000000000001",
        "1000000000000000000000000000",
        "9000000000000000000000000000",

        # Số gồm toàn chữ số 9.
        "9999999999999999999999999999",

        # Số có nhiều chữ số, gồm các nhóm limb khác nhau.
        "123456789012345678901234567890",
        "987654321098765432109876543210",
    ]

    total = 0
    passed = 0
    failed = 0

    for power in (1, 2, 3):
        print("\n===== POWER {} =====".format(power))

        for number in test_cases:
            total += 1

            try:
                actual = run_c_program(number, power)
                expected = expected_result(int(number), power)

                if actual == expected:
                    passed += 1
                    print("PASS | N={} | power={}".format(
                        number, power
                    ))
                else:
                    failed += 1
                    print("FAIL | N={} | power={}".format(
                        number, power
                    ))
                    print("Expected:", expected)
                    print("Actual:  ", actual)

            except Exception as e:
                failed += 1
                print("ERROR | N={} | power={}".format(
                    number, power
                ))
                print(e)

    print("\n===== SUMMARY =====")
    print("Total:", total)
    print("Passed:", passed)
    print("Failed:", failed)

    # Trả mã thoát khác 0 nếu có bất kỳ test nào thất bại.
    if failed > 0:
        print("\nTESTS FAILED!")
        return 1

    print("\nALL TESTS PASSED!")
    return 0


if __name__ == "__main__":
    sys.exit(main())
