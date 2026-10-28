
#!/usr/bin/env python3

import ctypes
import random
import sys
import time
from pathlib import Path
import random

# ============================================================
# CONFIGURATION
# ============================================================

MAX_LIMBS = 10000
MAX_DIGITS = 20000
STEP = 10

MAX_FAILURES_SHOWN = 20

LIB_PATH = (
    Path(__file__).resolve().parent.parent
    / "libbig_number.so"
)

if STEP <= 0:
    raise ValueError("STEP phải lớn hơn 0")

if MAX_DIGITS <= 0:
    raise ValueError("MAX_DIGITS phải lớn hơn 0")

if hasattr(sys, "set_int_max_str_digits"):
    sys.set_int_max_str_digits(0)


# ============================================================
# C STRUCTURE
# ============================================================

class Bn(ctypes.Structure):
    _fields_ = [
        ("limbs", ctypes.c_uint32 * MAX_LIMBS),
        ("len", ctypes.c_size_t),
    ]


# ============================================================
# LOAD C SHARED LIBRARY
# ============================================================

def load_library():
    if not LIB_PATH.is_file():
        raise FileNotFoundError(
            f"Không tìm thấy thư viện: {LIB_PATH}\n"
            "Hãy biên dịch libbig_number.so trước khi kiểm thử."
        )

    lib = ctypes.CDLL(str(LIB_PATH))

    lib.bn_from_string.argtypes = [ctypes.c_char_p]
    lib.bn_from_string.restype = Bn

    lib.bn_to_string.argtypes = [
        ctypes.POINTER(Bn),
        ctypes.c_char_p,
    ]
    lib.bn_to_string.restype = None

    lib.bn_compare.argtypes = [Bn, Bn]
    lib.bn_compare.restype = ctypes.c_int

    lib.bn_add.argtypes = [Bn, Bn]
    lib.bn_add.restype = Bn

    lib.bn_sub.argtypes = [Bn, Bn]
    lib.bn_sub.restype = Bn

    lib.bn_mul.argtypes = [Bn, Bn]
    lib.bn_mul.restype = Bn

    lib.bn_divmod_small.argtypes = [
        Bn,
        ctypes.c_uint32,
        ctypes.POINTER(Bn),
        ctypes.POINTER(ctypes.c_uint32),
    ]
    lib.bn_divmod_small.restype = None

    lib.bn_normalize.argtypes = [ctypes.POINTER(Bn)]
    lib.bn_normalize.restype = None

    lib.odd_sum.argtypes = [Bn, ctypes.c_uint8]
    lib.odd_sum.restype = Bn

    return lib


# ============================================================
# TESTER
# ============================================================

class Tester:

    def __init__(self, lib):
        self.lib = lib
        self.total = 0
        self.failed = 0
        self.shown = 0

    # --------------------------------------------------------
    # General assertion
    # --------------------------------------------------------

    def check(self, name, actual, expected, digits):
        self.total += 1

        if actual != expected:
            self.failed += 1

            if self.shown < MAX_FAILURES_SHOWN:
                self.shown += 1

                print(f"\n\n[FAIL] {name}")
                print(f"Digits  : {digits}")
                print(f"Actual  : {str(actual)[:200]}")
                print(f"Expected: {str(expected)[:200]}")

    # --------------------------------------------------------
    # Conversion helpers
    # --------------------------------------------------------

    def from_string(self, s):
        return self.lib.bn_from_string(s.encode("ascii"))

    def to_string(self, bn):
        buf = ctypes.create_string_buffer(MAX_LIMBS * 9 + 2)

        self.lib.bn_to_string(ctypes.byref(bn), buf)

        return buf.value.decode("ascii")

    def check_bn(self, name, bn, expected, digits):
        self.check(
            name,
            self.to_string(bn),
            str(expected),
            digits,
        )

    # --------------------------------------------------------
    # Conversion test
    # --------------------------------------------------------

    def test_conversion(self, a_text, digits):
        a = self.from_string(a_text)

        self.check_bn(
            "from_string/to_string",
            a,
            int(a_text),
            digits,
        )

    # --------------------------------------------------------
    # Comparison test
    # --------------------------------------------------------

    def test_compare(self, a_text, b_text, digits):
        a = self.from_string(a_text)
        b = self.from_string(b_text)

        av = int(a_text)
        bv = int(b_text)

        expected = (
            1 if av > bv
            else 0 if av < bv
            else 2
        )

        actual = self.lib.bn_compare(a, b)

        self.check(
            "compare",
            actual,
            expected,
            digits,
        )

    # --------------------------------------------------------
    # Addition test
    # --------------------------------------------------------

    def test_add(self, a_text, b_text, digits):
        a = self.from_string(a_text)
        b = self.from_string(b_text)

        result = self.lib.bn_add(a, b)

        expected = int(a_text) + int(b_text)

        self.check_bn(
            "add",
            result,
            expected,
            digits,
        )

    # --------------------------------------------------------
    # Subtraction test
    # --------------------------------------------------------

    def test_sub(self, a_text, b_text, digits):
        a = self.from_string(a_text)
        b = self.from_string(b_text)

        av = int(a_text)
        bv = int(b_text)

        # Theo quy ước của thư viện:
        # a <= b thì bn_sub trả về 0.
        expected = av - bv if av > bv else 0

        result = self.lib.bn_sub(a, b)

        self.check_bn(
            "sub",
            result,
            expected,
            digits,
        )

    # --------------------------------------------------------
    # Multiplication test
    # --------------------------------------------------------

    def test_mul(self, a_text, b_text, digits):
        a = self.from_string(a_text)
        b = self.from_string(b_text)

        result = self.lib.bn_mul(a, b)

        expected = int(a_text) * int(b_text)

        self.check_bn(
            "mul",
            result,
            expected,
            digits,
        )

    # --------------------------------------------------------
    # Division test
    # --------------------------------------------------------

    def test_divmod(self, a_text, digits):
        a = self.from_string(a_text)
        av = int(a_text)

        divisors = [
            1,
            2,
            3,
            7,
            10,
            999999937,
            4294967295,
        ]

        divisor = divisors[(digits - 1) % len(divisors)]

        quotient = Bn()
        remainder = ctypes.c_uint32(0)

        self.lib.bn_divmod_small(
            a,
            divisor,
            ctypes.byref(quotient),
            ctypes.byref(remainder),
        )

        self.check_bn(
            "div quotient",
            quotient,
            av // divisor,
            digits,
        )

        self.check(
            "div remainder",
            remainder.value,
            av % divisor,
            digits,
        )

    # --------------------------------------------------------
    # Normalization test
    # --------------------------------------------------------

    def test_normalize(self, a_text, digits):
        a = self.from_string(a_text)

        if a.len == 0 or a.len >= MAX_LIMBS:
            self.check(
                "normalize input length",
                False,
                True,
                digits,
            )
            return

        # Chèn một limb 0 ở đầu.
        for i in range(a.len, 0, -1):
            a.limbs[i] = a.limbs[i - 1]

        a.limbs[0] = 0
        a.len += 1

        self.lib.bn_normalize(ctypes.byref(a))

        self.check_bn(
            "normalize",
            a,
            int(a_text),
            digits,
        )

    # --------------------------------------------------------
    # Odd sum test
    # --------------------------------------------------------

    def test_odd_sum(self, a_text, digits):
        a = self.from_string(a_text)

        n = int(a_text)
        k = (n + 1) // 2

        expected = {
            1: k ** 2,
            2: k * (4 * k ** 2 - 1) // 3,
            3: k ** 2 * (2 * k ** 2 - 1),
        }

        for power in (1, 2, 3):
            result = self.lib.odd_sum(a, power)

            self.check_bn(
                f"odd_sum(power={power})",
                result,
                expected[power],
                digits,
            )


# ============================================================
# TEST DATA GENERATOR
# ============================================================

def make_number(rng, digits, pattern=None):
    """
    Tạo số nguyên không âm có đúng số chữ số yêu cầu.

    - digits == 1: Tạo số ngẫu nhiên từ 0 đến 9.
    - digits > 1: Chữ số đầu tiên từ 1 đến 9,
      các chữ số còn lại từ 0 đến 9.
    """

    if digits <= 0:
        raise ValueError("digits phải lớn hơn 0")

    if digits == 1:
        return str(rng.randint(0, 9))

    first = str(rng.randint(1, 9))

    tail = "".join(
        str(rng.randint(0, 9))
        for _ in range(digits - 1)
    )

    return first + tail

# ============================================================
# MAIN
# ============================================================

def main():
    try:
        lib = load_library()
    except (OSError, FileNotFoundError) as exc:
        print(f"Lỗi nạp thư viện: {exc}")
        return 1

    test = Tester(lib)

    rng = random.Random(20261027)

    start = time.time()

    test_lengths = range(1, MAX_DIGITS + 1, STEP)
    total_lengths = len(test_lengths)

    print("========== BIG NUMBER TEST ==========")
    print(f"Library    : {LIB_PATH}")
    print(f"Max digits : {MAX_DIGITS:,}")
    print(f"Step       : {STEP}")
    print(f"Test levels: {total_lengths:,}")
    print("=====================================")

    for index, digits in enumerate(test_lengths, start=1):

        a_text = make_number(
            rng,
            digits,
            digits % 4,
        )

        b_text = make_number(
            rng,
            digits,
            (digits + 1) % 4,
        )

        test.test_conversion(a_text, digits)
        test.test_compare(a_text, b_text, digits)
        test.test_add(a_text, b_text, digits)
        test.test_sub(a_text, b_text, digits)
        test.test_mul(a_text, b_text, digits)
        test.test_divmod(a_text, digits)
        test.test_normalize(a_text, digits)
        test.test_odd_sum(a_text, digits)

        if index == 1 or index % 10 == 0 or index == total_lengths:
            elapsed = time.time() - start

            print(
                f"\rProgress: {index:5}/{total_lengths} | "
                f"Digits: {digits:5}/{MAX_DIGITS} | "
                f"Tests: {test.total:,} | "
                f"Failed: {test.failed:,} | "
                f"Time: {elapsed:.1f}s",
                end="",
                flush=True,
            )

    elapsed = time.time() - start

    print("\n\n========== TEST RESULT ==========")
    print(f"Total tests : {test.total:,}")
    print(f"Passed      : {test.total - test.failed:,}")
    print(f"Failed      : {test.failed:,}")
    print(f"Elapsed     : {elapsed:.2f} seconds")

    if test.failed:
        print("RESULT: FAIL")
        return 1

    print("RESULT: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
