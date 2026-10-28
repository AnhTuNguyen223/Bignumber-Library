
import ctypes
import random
import sys
from pathlib import Path

# ============================================================
# CONFIGURATION
# ============================================================

MAX_LIMBS = 10000
BN_BASE = 1000000000

NUM_TESTS = 1000
MAX_DIGITS = 1000

SEED = 20261027

LIB_PATH = (
    Path(__file__).resolve().parent.parent
    / "libbig_number.so"
)

rng = random.Random(SEED)


# ============================================================
# BIG NUMBER STRUCTURE
# Phải khớp chính xác với big_number.h
# ============================================================

class Bn(ctypes.Structure):
    _fields_ = [
        ("limbs", ctypes.c_uint32 * MAX_LIMBS),
        ("len", ctypes.c_size_t),
    ]


# ============================================================
# LOAD C LIBRARY
# ============================================================

if not LIB_PATH.exists():
    raise FileNotFoundError(
        "Không tìm thấy thư viện: {}".format(LIB_PATH)
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


# ============================================================
# HELPER FUNCTIONS
# ============================================================

def from_string(number):
    """Chuyển chuỗi thập phân thành bn_t."""
    return lib.bn_from_string(number.encode("ascii"))


def to_string(number):
    """Chuyển bn_t thành chuỗi thập phân."""
    lib.bn_normalize(ctypes.byref(number))

    # MAX_LIMBS * 9 + 1 là dung lượng lý thuyết đủ
    # cho một số có MAX_LIMBS limb.
    buffer = ctypes.create_string_buffer(MAX_LIMBS * 9 + 1)

    lib.bn_to_string(ctypes.byref(number), buffer)

    return buffer.value.decode("ascii")


def to_int(number):
    """Chuyển bn_t thành Python int để kiểm tra."""
    return int(to_string(number))


def make_number(digits):
    """Tạo số ngẫu nhiên không âm có đúng digits chữ số."""
    if digits == 1:
        return str(rng.randint(0, 9))

    first = str(rng.randint(1, 9))

    tail = "".join(
        str(rng.randint(0, 9))
        for _ in range(digits - 1)
    )

    return first + tail


def random_bn():
    """Tạo một Big Number ngẫu nhiên."""
    digits = rng.randint(1, MAX_DIGITS)
    return from_string(make_number(digits))


def assert_equal(actual, expected, property_name):
    """Báo lỗi khi hai kết quả không bằng nhau."""
    if actual != expected:
        raise AssertionError(
            "{}\nActual:   {}\nExpected: {}".format(
                property_name,
                actual,
                expected,
            )
        )


def check_compare(a, b):
    """Kiểm tra bn_compare theo quy ước của thư viện."""
    result = lib.bn_compare(a, b)

    a_value = to_int(a)
    b_value = to_int(b)

    if a_value > b_value:
        expected = 1
    elif a_value < b_value:
        expected = 0
    else:
        expected = 2

    assert_equal(
        result,
        expected,
        "bn_compare",
    )


# ============================================================
# PROPERTY TESTS
# ============================================================

def test_add_commutative():
    """a + b = b + a."""
    a = random_bn()
    b = random_bn()

    left = to_int(lib.bn_add(a, b))
    right = to_int(lib.bn_add(b, a))

    assert_equal(left, right, "Addition commutativity")


def test_add_identity():
    """a + 0 = a."""
    a = random_bn()
    zero = from_string("0")

    result = to_int(lib.bn_add(a, zero))

    assert_equal(
        result,
        to_int(a),
        "Addition identity",
    )


def test_add_associative():
    """(a + b) + c = a + (b + c)."""
    a = random_bn()
    b = random_bn()
    c = random_bn()

    ab = lib.bn_add(a, b)
    left = to_int(lib.bn_add(ab, c))

    bc = lib.bn_add(b, c)
    right = to_int(lib.bn_add(a, bc))

    assert_equal(left, right, "Addition associativity")


def test_subtract_inverse():
    """Nếu a >= b thì (a - b) + b = a."""
    a = random_bn()
    b = random_bn()

    if lib.bn_compare(a, b) == 0:
        a, b = b, a

    # Quy ước bn_compare:
    # 1 nếu a > b, 0 nếu a < b, 2 nếu a == b.
    if lib.bn_compare(a, b) == 0:
        a, b = b, a

    if lib.bn_compare(a, b) == 1:
        result = lib.bn_sub(a, b)
        restored = lib.bn_add(result, b)

        assert_equal(
            to_int(restored),
            to_int(a),
            "Subtraction inverse",
        )
    else:
        # Khi a < b, thư viện quy định bn_sub trả về 0.
        result = lib.bn_sub(a, b)

        assert_equal(
            to_int(result),
            0,
            "Subtraction when a < b",
        )


def test_subtract_equal():
    """a - a = 0 theo quy ước của thư viện."""
    a = random_bn()

    result = to_int(lib.bn_sub(a, a))

    assert_equal(result, 0, "Subtraction equal operands")


def test_mul_commutative():
    """a * b = b * a."""
    a = random_bn()
    b = random_bn()

    left = to_int(lib.bn_mul(a, b))
    right = to_int(lib.bn_mul(b, a))

    assert_equal(left, right, "Multiplication commutativity")


def test_mul_identity():
    """a * 1 = a."""
    a = random_bn()
    one = from_string("1")

    result = to_int(lib.bn_mul(a, one))

    assert_equal(
        result,
        to_int(a),
        "Multiplication identity",
    )


def test_mul_zero():
    """a * 0 = 0."""
    a = random_bn()
    zero = from_string("0")

    result = to_int(lib.bn_mul(a, zero))

    assert_equal(result, 0, "Multiplication by zero")


def test_distributive():
    """a * (b + c) = a*b + a*c."""
    a = random_bn()
    b = random_bn()
    c = random_bn()

    bc = lib.bn_add(b, c)

    left = to_int(lib.bn_mul(a, bc))

    ab = lib.bn_mul(a, b)
    ac = lib.bn_mul(a, c)

    right = to_int(lib.bn_add(ab, ac))

    assert_equal(left, right, "Multiplication distributivity")


def test_compare_consistency():
    """Kiểm tra tính nhất quán của bn_compare."""
    a = random_bn()
    b = random_bn()

    check_compare(a, b)
    check_compare(b, a)

    # Mọi số phải bằng chính nó.
    assert_equal(
        lib.bn_compare(a, a),
        2,
        "Compare a with itself",
    )


def test_divmod_identity():
    """a = q*d + r và 0 <= r < d."""
    a = random_bn()

    # Chọn số chia dương, tránh chia cho 0.
    divisor = rng.randint(1, 0xFFFFFFFF)

    quotient = Bn()
    remainder = ctypes.c_uint32(0)

    lib.bn_divmod_small(
        a,
        divisor,
        ctypes.byref(quotient),
        ctypes.byref(remainder),
    )

    q = to_int(quotient)
    r = remainder.value
    original = to_int(a)

    assert_equal(
        q * divisor + r,
        original,
        "Division identity: a = q*d + r",
    )

    if not (0 <= r < divisor):
        raise AssertionError(
            "Division remainder out of range: "
            "r={}, divisor={}".format(r, divisor)
        )


# ============================================================
# TEST RUNNER
# ============================================================

TESTS = [
    ("Addition commutativity", test_add_commutative),
    ("Addition identity", test_add_identity),
    ("Addition associativity", test_add_associative),
    ("Subtraction inverse", test_subtract_inverse),
    ("Subtraction equal operands", test_subtract_equal),
    ("Multiplication commutativity", test_mul_commutative),
    ("Multiplication identity", test_mul_identity),
    ("Multiplication by zero", test_mul_zero),
    ("Multiplication distributivity", test_distributive),
    ("Comparison consistency", test_compare_consistency),
    ("Division identity", test_divmod_identity),
]


def main():
    total = 0
    failed = 0

    print("========== PROPERTY-BASED TEST ==========")
    print("Library   :", LIB_PATH)
    print("Random seed:", SEED)
    print("Iterations:", NUM_TESTS)
    print("Max digits:", MAX_DIGITS)
    print("Properties:", len(TESTS))
    print("=========================================")

    for name, test_function in TESTS:
        print("\n[RUN] {}".format(name))

        for iteration in range(NUM_TESTS):
            total += 1

            try:
                test_function()

            except Exception as exc:
                failed += 1

                print(
                    "[FAIL] {} | iteration {}".format(
                        name,
                        iteration + 1,
                    )
                )
                print("Reason:", exc)

                # Dừng ngay để dễ tái hiện lỗi.
                print("Input seed:", SEED)
                print("Stopping after first failure.")
                break

        if failed:
            break

        print(
            "[PASS] {} ({} iterations)".format(
                name,
                NUM_TESTS,
            )
        )

    print("\n========== SUMMARY ==========")
    print("Checks executed:", total)
    print("Failed checks :", failed)

    if failed:
        print("PROPERTY TEST FAILED")
        return 1

    print("ALL PROPERTY TESTS PASSED")
    return 0


if __name__ == "__main__":
    sys.exit(main())
