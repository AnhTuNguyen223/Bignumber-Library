
import os
import re
import subprocess
import random
PROGRAM = "./odd_sum"
INPUT_FILE = "input.txt"

START_DIGITS = 20
MAX_DIGITS = 20000
STEP = 20


def generate_number(digits):
    """Tạo số nguyên ngẫu nhiên có đúng số chữ số yêu cầu."""
    first_digit = random.randint(1, 9)
    remaining_digits = ''.join(
        random.choices('0123456789', k=digits - 1)
    )
    return str(first_digit) + remaining_digits


def expected_result(n, power):
    """Tính kết quả tham chiếu bằng công thức Python."""
    k = (n + 1) // 2

    if power == 1:
        return k * k
    elif power == 2:
        return k * (4 * k * k - 1) // 3
    elif power == 3:
        return k * k * (2 * k * k - 1)

    raise ValueError("Power không hợp lệ")


def run_c_program(number, power):
    """Ghi input, chạy chương trình C và lấy kết quả."""
    with open(INPUT_FILE, "w") as f:
        f.write(number)

    command = [PROGRAM, INPUT_FILE, "--power", str(power)]

    result = subprocess.run(
        command,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        universal_newlines=True,
        timeout=60
    )

    if result.returncode != 0:
        raise RuntimeError(
            "Chương trình C bị lỗi, return code = {}\n{}".format(
                result.returncode, result.stderr
            )
        )

    matches = re.findall(
        r"BIGNUMBER odd sum:\s*(\d+)",
        result.stdout
    )

    if not matches:
        raise RuntimeError(
            "Không tìm thấy kết quả trong output:\n{}".format(
                result.stdout
            )
        )

    return int(matches[-1])


def main():
    total = 0
    passed = 0
    failed = 0

    # Kiểm thử cả ba chế độ lũy thừa
    for power in (1, 2, 3):
        print("\n===== POWER {} =====".format(power))

        for digits in range(START_DIGITS, MAX_DIGITS + 1, STEP):
            number = generate_number(digits)
            n = int(number)

            try:
                actual = run_c_program(number, power)
                expected = expected_result(n, power)

                if actual == expected:
                    passed += 1
                    status = "PASS"
                else:
                    failed += 1
                    status = "FAIL"

                print("{} | digits={} | power={}".format(
                    status, digits, power
                ))

                if actual != expected:
                    print("Expected:", expected)
                    print("Actual:  ", actual)
                    print("Dừng kiểm thử vì kết quả sai.")
                    return

            except Exception as e:
                failed += 1
                print("ERROR | digits={} | power={}".format(
                    digits, power
                ))
                print(e)
                print("Dừng kiểm thử.")
                return

            total += 1

    print("\n===== TEST SUMMARY =====")
    print("Total:", total)
    print("Passed:", passed)
    print("Failed:", failed)


if __name__ == "__main__":
    main()

