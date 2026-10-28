

import re
import subprocess
import sys
from pathlib import Path

# Thư mục chứa file test_edge.py
TEST_DIR = Path(__file__).resolve().parent

# Thư mục src chứa chương trình odd_sum
SRC_DIR = TEST_DIR.parent / "src"

PROGRAM_PATH = SRC_DIR / "odd_sum"
INPUT_FILE = SRC_DIR / "input_edge.txt"

TIMEOUT = 60


# Đường dẫn thư mục chứa chương trình odd_sum.
# Mặc định chạy script từ thư mục src.
PROGRAM_PATH = SRC_DIR / "odd_sum_sanitize"


def expected_result(n, power):
    """
    Tính kết quả kỳ vọng bằng số nguyên lớn của Python.

    n: số nguyên không âm.
    power: 1, 2 hoặc 3.
    """
    k = (n + 1) // 2

    if power == 1:
        return k * k
    elif power == 2:
        return k * (4 * k * k - 1) // 3
    elif power == 3:
        return k * k * (2 * k * k - 1)

    raise ValueError("Power không hợp lệ: {}".format(power))


def run_c_program(number, power):
    """
    Ghi số đầu vào ra file, chạy odd_sum và lấy kết quả.

    Phát sinh exception nếu chương trình lỗi, timeout,
    hoặc kết quả đầu ra không đúng định dạng.
    """
    with open(INPUT_FILE, "w", encoding="ascii") as f:
        f.write(number + "\n")

    try:
            result = subprocess.run(
        [
            str(PROGRAM_PATH),
            str(INPUT_FILE),
            "--power",
            str(power),
        ],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        universal_newlines=True,
        timeout=TIMEOUT,
        check=False,
        cwd=str(SRC_DIR),
        )
    except subprocess.TimeoutExpired:
        raise RuntimeError(
            "Chương trình vượt quá thời gian {} giây".format(TIMEOUT)
        )

    if result.returncode != 0:
        raise RuntimeError(
            "Return code: {}\nSTDOUT:\n{}\nSTDERR:\n{}".format(
                result.returncode,
                result.stdout,
                result.stderr,
            )
        )

    matches = re.findall(
        r"BIGNUMBER odd sum:\s*(\d+)",
        result.stdout,
    )

    # Phải có chính xác một kết quả.
    if len(matches) != 1:
        raise RuntimeError(
            "Mong đợi đúng 1 kết quả, nhưng tìm thấy {}.\n"
            "STDOUT:\n{}".format(len(matches), result.stdout)
        )

    return int(matches[0])


def build_test_cases():
    """
    Tạo danh sách các trường hợp kiểm thử biên.
    Mỗi phần tử là một chuỗi biểu diễn số nguyên không âm.
    """
    test_cases = [
        # -------------------------------------------------
        # 1. Các giá trị nhỏ
        # -------------------------------------------------
        "0",
        "1",
        "2",
        "3",
        "4",
        "5",
        "6",
        "7",
        "8",
        "9",
        "10",
        "11",
        "12",
        "99",
        "100",
        "101",
        "999",
        "1000",
        "1001",

        # -------------------------------------------------
        # 2. Ranh giới limb với cơ số 10^9
        # -------------------------------------------------
        "999999998",
        "999999999",
        "1000000000",
        "1000000001",
        "1000000002",

        # Ranh giới 10^18
        "999999999999999998",
        "999999999999999999",
        "1000000000000000000",
        "1000000000000000001",
        "1000000000000000002",

        # Ranh giới 10^27
        "999999999999999999999999998",
        "999999999999999999999999999",
        "1000000000000000000000000000",
        "1000000000000000000000000001",
        "1000000000000000000000000002",

        # -------------------------------------------------
        # 3. Số có nhiều chữ số 0
        # -------------------------------------------------
        "1000000000001",
        "1000000000000000000000000000",
        "9000000000000000000000000000",
        "1000000001000000001",
        "1000000000000000001000000001",
        "9000000000000000009000000009",
        "1010101010101010101010101010",
        "1000000000000000000000000000000000000001",

        # -------------------------------------------------
        # 4. Số toàn chữ số 9
        # -------------------------------------------------
        "99",
        "999999999",
        "999999999999",
        "999999999999999999",
        "999999999999999999999999999999",

        # -------------------------------------------------
        # 5. Số có nhiều nhóm limb khác nhau
        # -------------------------------------------------
        "123456789012345678901234567890",
        "987654321098765432109876543210",
        "123456789000000000123456789",
        "1000000010000000100000001",
        "987654321000000000000000123456789",

        # -------------------------------------------------
        # 6. Các độ dài lớn
        # -------------------------------------------------
        "1" + "0" * 99,          # 100 chữ số
        "9" * 100,               # 100 chữ số toàn 9

        "1" + "0" * 999,         # 1.000 chữ số
        "9" * 1000,              # 1.000 chữ số toàn 9

        "1" + "0" * 4999,        # 5.000 chữ số
        "9" * 5000,              # 5.000 chữ số toàn 9

        "1" + "0" * 9999,        # 10.000 chữ số
        "9" * 10000,              # 10.000 chữ số toàn 9

        "1" + "0" * 19999,       # 20.000 chữ số
        "9" * 20000,             # 20.000 chữ số toàn 9
    ]

    # Kiểm tra tính hợp lệ của các dữ liệu trước khi chạy.
    for number in test_cases:
        if not number.isdigit():
            raise ValueError("Dữ liệu kiểm thử không phải số: {!r}".format(number))

        if len(number) > 20000:
            raise ValueError(
                "Đầu vào vượt quá 20.000 chữ số: {} chữ số".format(
                    len(number)
                )
            )

        if len(number) > 1 and number[0] == "0":
            raise ValueError(
                "Đầu vào không được có số 0 thừa ở đầu: {!r}".format(number)
            )

    return test_cases


def main():
    if not PROGRAM_PATH.is_file():
        print("ERROR: Không tìm thấy chương trình:", PROGRAM_PATH)
        print("Hãy biên dịch odd_sum trước khi chạy test.")
        return 1

    test_cases = build_test_cases()

    total = 0
    passed = 0
    failed = 0
    errors = []

    print("========== EDGE TEST ==========")
    print("Program       :", PROGRAM_PATH.resolve())
    print("Input file    :", Path(INPUT_FILE).resolve())
    print("Number of N   :", len(test_cases))
    print("Powers        : 1, 2, 3")
    print("Max input     : 20000 digits")
    print("Total tests   :", len(test_cases) * 3)
    print("===============================")

    for power in (1, 2, 3):
        print("\n===== POWER {} =====".format(power))

        for number in test_cases:
            total += 1
            label = "N={} digits, power={}".format(len(number), power)

            try:
                actual = run_c_program(number, power)
                expected = expected_result(int(number), power)

                if actual == expected:
                    passed += 1
                    print(
                        "PASS | digits={} | power={}".format(
                            len(number), power
                        )
                    )
                else:
                    failed += 1
                    message = (
                        "FAIL | digits={} | power={}\n"
                        "Expected: {}\n"
                        "Actual:   {}"
                    ).format(
                        len(number),
                        power,
                        expected,
                        actual,
                    )
                    print(message)
                    errors.append(message)

            except Exception as e:
                failed += 1
                message = "ERROR | {} \n{}".format(label, e)
                print(message)
                errors.append(message)

    print("\n========== SUMMARY ==========")
    print("Total  :", total)
    print("Passed :", passed)
    print("Failed :", failed)

    if failed > 0:
        print("\n========== FAILURE DETAILS ==========")
        for error in errors:
            print(error)
            print("-" * 60)

        print("\nEDGE TESTS FAILED!")
        return 1

    print("\nALL EDGE TESTS PASSED!")
    return 0


if __name__ == "__main__":
    sys.exit(main())