# Big Number Library & odd_sum

Thư viện xử lý số nguyên lớn bằng C và chương trình tính tổng lũy thừa của các số nguyên dương lẻ không vượt quá `N`. Dự án hỗ trợ số nguyên đầu vào có tối đa 20.000 chữ số thập phân, cung cấp các phép toán số nguyên lớn cơ bản, hệ thống kiểm thử bằng Python và chương trình benchmark để phân tích hiệu năng.

---

## Mục lục

- [1. Thiết kế](#1-thiết-kế)
  - [1.1. Thiết kế dữ liệu](#11-thiết-kế-dữ-liệu)
  - [1.2. Thiết kế thư viện](#12-thiết-kế-thư-viện)
    - [1.2.1. bn_from_string](#121-bn_from_string)
    - [1.2.2. bn_to_string](#122-bn_to_string)
    - [1.2.3. bn_compare](#123-bn_compare)
    - [1.2.4. bn_add](#124-bn_add)
    - [1.2.5. bn_sub](#125-bn_sub)
    - [1.2.6. bn_mul](#126-bn_mul)
    - [1.2.7. bn_divmod_small](#127-bn_divmod_small)
  - [1.3. Thiết kế chương trình odd_sum](#13-thiết-kế-chương-trình-odd_sum)
  - [1.4. Công thức tính toán](#14-công-thức-tính-toán)
- [2. Build và sử dụng](#2-build-và-sử-dụng)
  - [2.1. Build chương trình](#21-build-chương-trình)
  - [2.2. Chạy chương trình](#22-chạy-chương-trình)
  - [2.3. Chạy kiểm thử](#23-chạy-kiểm-thử)
  - [2.4. Chạy Test Bench](#24-chạy-test-bench)
- [3. Phương pháp kiểm thử](#3-phương-pháp-kiểm-thử)
  - [3.1. Nội dung kiểm thử](#31-nội-dung-kiểm-thử)
  - [3.2. Nguồn đáp án chuẩn](#32-nguồn-đáp-án-chuẩn)
  - [3.3. Kiểm thử thư viện](#33-kiểm-thử-thư-viện)
  - [3.4. Kiểm thử odd_sum](#34-kiểm-thử-odd_sum)
  - [3.5. Kết quả kiểm thử](#35-kết-quả-kiểm-thử)
- [4. Test Bench và phân tích độ phức tạp](#4-test-bench-và-phân-tích-độ-phức-tạp)
  - [4.1. Phương pháp đo](#41-phương-pháp-đo)
  - [4.2. Kết quả đo](#42-kết-quả-đo)
  - [4.3. Nhận xét](#43-nhận-xét)
- [5. Cải thiện so với phiên bản trước](#5-cải-thiện-so-với-phiên-bản-trước)
- [6. Hạn chế](#6-hạn-chế)
- [7. Hướng phát triển và tối ưu](#7-hướng-phát-triển-và-tối-ưu)

---

# 1. Thiết kế

## 1.1. Thiết kế dữ liệu

### Đặc điểm

- Số nguyên đầu vào `N` có tối đa **20.000 chữ số thập phân**.
- Đầu vào và đầu ra được biểu diễn dưới dạng chuỗi ký tự tương ứng với số nguyên không âm.
- Chuỗi đầu vào được chia thành các đoạn nhỏ gọi là `limb`.
- Mỗi `limb` chứa tối đa 9 chữ số thập phân và được lưu bằng kiểu dữ liệu `uint32_t`.
- Cấu trúc `bn_t` sử dụng mảng `limbs[10000]` để lưu các `limb` và biến `len` để lưu số lượng `limb` đang sử dụng.
- Mỗi `limb` sử dụng cơ số \(10^9\).
- `limbs[0]` là phần tử có trọng số cao nhất. Trọng số giảm dần từ trái sang phải.
- Tham số `power` nhận một trong ba giá trị: `1`, `2` hoặc `3`.

### Thông số biểu diễn

| Thuộc tính | Giá trị |
|---|---|
| Số chữ số tối đa của đầu vào | 20.000 |
| Kiểu dữ liệu biểu diễn số nguyên lớn | `bn_t` |
| Mảng lưu trữ | `limbs[10000]` |
| Số lượng `limb` đang sử dụng | `len` |
| Kiểu dữ liệu của mỗi `limb` | `uint32_t` |
| Cơ số | \(10^9\) |
| Số chữ số tối đa mỗi `limb` | 9 |
| Thứ tự lưu trữ | Từ trọng số cao nhất đến thấp nhất |
| Các giá trị `power` được hỗ trợ | 1, 2, 3 |

Với 10.000 `limb`, mảng có khả năng biểu diễn tối đa 90.000 chữ số thập phân về mặt dung lượng lưu trữ. Dung lượng này được lựa chọn để đáp ứng yêu cầu tính toán với đầu vào có tối đa 20.000 chữ số.

## 1.2. Thiết kế thư viện

Thư viện `big_number.h` cung cấp các hàm chuyển đổi và xử lý số nguyên lớn, bao gồm chuyển đổi chuỗi, so sánh, cộng, trừ, nhân và chia cho số nguyên nhỏ.

### 1.2.1. `bn_from_string`

**Chức năng:** Chuyển đổi chuỗi thập phân thành cấu trúc `bn_t`.

**Thuật toán:**

1. Tính độ dài chuỗi đầu vào.
2. Xác định số lượng `limb` cần thiết để lưu trữ toàn bộ chuỗi.
3. Tính số chữ số của `limb` đầu tiên. Phần tử này có thể chứa ít hơn 9 chữ số.
4. Chia chuỗi thành các đoạn, mỗi đoạn tối đa 9 chữ số.
5. Chuyển từng đoạn thành giá trị `uint32_t` và lưu vào mảng `limbs`.

Kết quả là một cấu trúc số nguyên lớn có các `limb` tương ứng với chuỗi đầu vào.

**Độ phức tạp thời gian:** \(O(n)\).

### 1.2.2. `bn_to_string`

**Chức năng:** Chuyển đổi cấu trúc `bn_t` thành chuỗi thập phân.

**Thuật toán:**

1. Chuyển `limb` đầu tiên thành chuỗi mà không bổ sung các chữ số `0` ở đầu, vì phần tử này có thể chứa từ 1 đến 9 chữ số.
2. Chuyển các `limb` còn lại thành chuỗi có độ rộng cố định 9 chữ số.
3. Nếu một `limb` có ít hơn 9 chữ số, bổ sung các chữ số `0` ở đầu.
4. Ghép các đoạn chuỗi để tạo thành số nguyên lớn hoàn chỉnh.

**Độ phức tạp thời gian:** \(O(n)\).

### 1.2.3. `bn_compare`

**Chức năng:** So sánh hai số nguyên lớn `a` và `b`.

**Quy ước giá trị trả về:**

| Giá trị trả về | Ý nghĩa |
|---|---|
| `1` | \(a > b\) |
| `0` | \(a < b\) |
| `2` | \(a = b\) |

**Thuật toán:**

1. So sánh số lượng `limb` của hai số. Số có nhiều `limb` hơn sẽ có giá trị lớn hơn.
2. Nếu hai số có cùng số lượng `limb`, so sánh từng phần tử bắt đầu từ `limbs[0]`, tức phần tử có trọng số cao nhất.
3. Khi tìm thấy hai `limb` khác nhau, xác định kết quả dựa trên giá trị của chúng.
4. Nếu tất cả các `limb` tương ứng đều bằng nhau, hai số bằng nhau.

**Độ phức tạp thời gian:** \(O(n)\).

### 1.2.4. `bn_add`

**Chức năng:** Cộng hai số nguyên lớn.

**Thuật toán:**

1. Xác định số có số lượng `limb` lớn hơn.
2. Thực hiện phép cộng từ phần tử cuối cùng của hai mảng, tức từ `limb` có trọng số thấp nhất.
3. Cộng các `limb` tương ứng cùng với giá trị nhớ `carry` từ bước trước.
4. Lấy phần dư theo cơ số \(10^9\) để lưu vào `limb` hiện tại.
5. Lấy phần nguyên của phép chia cho \(10^9\) để xác định giá trị nhớ chuyển sang vị trí tiếp theo.
6. Nếu vẫn còn `carry` sau khi cộng, bổ sung một `limb` mới ở đầu kết quả.

**Độ phức tạp thời gian:** \(O(n)\).

### 1.2.5. `bn_sub`

**Chức năng:** Trừ hai số nguyên lớn.

**Điều kiện:** Chỉ thực hiện phép trừ khi \(a > b\).

**Thuật toán:**

1. Thực hiện phép trừ từ `limb` cuối cùng về `limb` đầu tiên.
2. Nếu `limb` của số bị trừ nhỏ hơn `limb` tương ứng của số trừ sau khi tính `borrow`, cộng thêm cơ số \(10^9\) để thực hiện phép trừ.
3. Đánh dấu `borrow` để trừ thêm 1 vào `limb` tiếp theo.
4. Sau khi hoàn tất, loại bỏ các `limb` có giá trị `0` ở đầu kết quả nếu cần thiết.

**Độ phức tạp thời gian:** \(O(n)\).

### 1.2.6. `bn_mul`

**Chức năng:** Nhân hai số nguyên lớn.

**Thuật toán:**

1. Thực hiện phép nhân từng cặp `limb` của hai số.
2. Tại mỗi bước, nhân hai `limb`, cộng với giá trị đã có tại vị trí tương ứng và phần nhớ `carry`.
3. Lấy phần dư theo cơ số \(10^9\) để lưu vào `limb` hiện tại.
4. Lấy phần nguyên của phép chia cho \(10^9\) để chuyển phần nhớ sang vị trí tiếp theo.
5. Tiếp tục cho đến khi hoàn tất các cặp `limb` và xử lý phần nhớ còn lại.

Thuật toán sử dụng hai vòng lặp lồng nhau để thực hiện phép nhân các `limb`.

**Độ phức tạp thời gian:** \(O(n^2)\).

### 1.2.7. `bn_divmod_small`

**Chức năng:** Chia số nguyên lớn cho một số nguyên nhỏ `divisor`, trả về thương và số dư.

**Thuật toán:**

1. Duyệt các `limb` từ trái sang phải, bắt đầu từ phần tử có trọng số cao nhất.
2. Tại mỗi bước, tính giá trị tạm thời bằng số dư của bước trước nhân với cơ số \(10^9\), sau đó cộng với `limb` hiện tại.
3. Tính `limb` thương bằng phép chia nguyên giá trị tạm thời cho `divisor`.
4. Cập nhật số dư bằng phép chia lấy dư giá trị tạm thời cho `divisor`.
5. Tiếp tục cho đến khi duyệt hết các `limb`.

**Độ phức tạp thời gian:** \(O(n)\).

---

## 1.3. Thiết kế chương trình odd_sum

Chương trình `odd_sum` tính tổng lũy thừa của các số nguyên dương lẻ không vượt quá `N`.

### Đầu vào

- Một số nguyên không âm `N` có tối đa 20.000 chữ số.
- Dữ liệu được đọc từ một tệp đầu vào.
- Tham số `power` nhận giá trị từ `1` đến `3`.

### Đầu ra

- Kết quả được lưu dưới dạng cấu trúc `bn_t`.
- Kết quả cuối cùng được chuyển thành chuỗi thập phân để xuất ra.

### Ý nghĩa tham số `power`

| Giá trị | Phép tính |
|---|---|
| `1` | Tổng các số nguyên dương lẻ |
| `2` | Tổng bình phương các số nguyên dương lẻ |
| `3` | Tổng lập phương các số nguyên dương lẻ |

Ví dụ, với \(N = 7\):

- `power = 1`: \(1 + 3 + 5 + 7 = 16\)
- `power = 2`: \(1^2 + 3^2 + 5^2 + 7^2 = 84\)
- `power = 3`: \(1^3 + 3^3 + 5^3 + 7^3 = 256\)

## 1.4. Công thức tính toán

Đặt:

\[
K = \left\lfloor \frac{N+1}{2} \right\rfloor
\]

Trong đó, \(K\) là số lượng số nguyên dương lẻ không vượt quá \(N\).

### Trường hợp `power = 1`

Tổng các số nguyên dương lẻ:

\[
S_1 = K^2
\]

### Trường hợp `power = 2`

Tổng bình phương các số nguyên dương lẻ:

\[
S_2 = \frac{K(4K^2-1)}{3}
\]

### Trường hợp `power = 3`

Tổng lập phương các số nguyên dương lẻ:

\[
S_3 = K^2(2K^2-1)
\]

Các công thức trên cho phép tính kết quả trực tiếp thay vì duyệt từng số lẻ. Vì `N` có thể chứa đến 20.000 chữ số, chương trình sử dụng thư viện số nguyên lớn để thực hiện các phép toán cần thiết.

---

# 2. Build và sử dụng

## 2.1. Build chương trình

Mở Terminal và di chuyển đến thư mục `src`:

```bash
cd src
```

Biên dịch chương trình:

```bash
make
```

Sau khi build thành công, file thực thi `odd_sum` sẽ được tạo trong thư mục `src`.

## 2.2. Chạy chương trình

Cú pháp:

```bash
./odd_sum input.txt [--power p]
```

Trong đó:

- `input.txt`: Tệp chứa số nguyên `N` cần tính.
- `--power p`: Tham số tùy chọn xác định bậc lũy thừa.
- `p`: Nhận một trong ba giá trị `1`, `2` hoặc `3`.

Ví dụ, tính tổng bình phương các số nguyên dương lẻ không vượt quá số `N` được lưu trong `input.txt`:

```bash
./odd_sum input.txt --power 2
```

Nếu không chỉ định `--power`, chương trình sử dụng giá trị mặc định được quy định trong mã nguồn.

## 2.3. Chạy kiểm thử

Dự án có hai thư mục liên quan đến kiểm thử:

- `test`: Chứa các chương trình kiểm thử thư viện và chương trình `odd_sum`.
- `test_bench`: Chứa chương trình benchmark, dữ liệu đo hiệu năng và script vẽ biểu đồ.

Các lệnh dưới đây được thực hiện từ thư mục `src`.

**Kiểm thử biên:**

```bash
make edgetest
```

**Kiểm thử chương trình `odd_sum`:**

```bash
make test_odd_sum
```

**Kiểm thử thư viện số nguyên lớn:**

```bash
make test_library
```

**Chạy toàn bộ bài kiểm thử:**

```bash
make test
```

## 2.4. Chạy Test Bench

Từ thư mục `src`, chạy:

```bash
make bench
```

Sau khi thực thi, quy trình benchmark sẽ đo thời gian chạy của các hàm được lựa chọn, ghi kết quả vào file CSV và sử dụng script Python để phân tích, vẽ biểu đồ log-log.

---

# 3. Phương pháp kiểm thử

## 3.1. Nội dung kiểm thử

Quá trình kiểm thử tập trung vào hai thành phần:

- Thư viện số nguyên lớn `big_number.h`.
- Chương trình tính tổng lũy thừa các số nguyên dương lẻ `odd_sum`.

Mục tiêu là đánh giá tính chính xác của các phép toán số nguyên lớn và kết quả đầu ra của chương trình với nhiều kích thước dữ liệu khác nhau.

## 3.2. Nguồn đáp án chuẩn

Sử dụng kiểu số nguyên có độ chính xác tùy ý của Python để tạo nguồn đáp án chuẩn độc lập.

Trong phiên bản V2, chương trình `odd_sum` được kiểm thử bằng script Python với kích thước đầu vào tăng dần theo từng bước 20 chữ số, từ 20 đến 20.000 chữ số.

Cả ba giá trị `power = 1`, `power = 2` và `power = 3` đều được kiểm tra.

## 3.3. Kiểm thử thư viện

**Mục tiêu:** Kiểm tra tính chính xác của các hàm xử lý số nguyên lớn trong thư viện `big_number.h`.

Các hàm được kiểm thử bao gồm:

- `bn_from_string()`: Chuyển đổi chuỗi thập phân thành cấu trúc số nguyên lớn.
- `bn_to_string()`: Chuyển đổi cấu trúc số nguyên lớn thành chuỗi thập phân.
- `bn_compare()`: So sánh hai số nguyên lớn.
- `bn_add()`: Cộng hai số nguyên lớn.
- `bn_sub()`: Trừ hai số nguyên lớn.
- `bn_mul()`: Nhân hai số nguyên lớn.
- `bn_divmod_small()`: Chia số nguyên lớn cho một số nguyên nhỏ, trả về thương và số dư.

### Các trường hợp kiểm thử

- Các phép toán thông thường.
- Số `0`.
- Các số có nhiều chữ số `0`.
- Các giá trị nằm sát ranh giới cơ số \(10^9\).
- Các số có nhiều `limb`.
- Các trường hợp phát sinh `carry` khi cộng và nhân.
- Các trường hợp phát sinh `borrow` khi trừ.
- Các trường hợp hai số bằng nhau hoặc có số lượng `limb` khác nhau.

Kết quả của từng phép toán được đối chiếu với kết quả tính toán độc lập để phát hiện sai lệch. Qua đó, quá trình kiểm thử đánh giá tính chính xác của các phép chuyển đổi và phép toán số nguyên lớn.

## 3.4. Kiểm thử odd_sum

### Phương pháp thực hiện

1. Sử dụng Python để tạo dữ liệu đầu vào có độ dài tăng dần từ 20 đến 20.000 chữ số, với bước tăng 20 chữ số.
2. Với mỗi kích thước đầu vào, lần lượt kiểm tra các giá trị `power` từ `1` đến `3`.
3. Sử dụng kiểu số nguyên có độ chính xác tùy ý của Python để tính kết quả chuẩn độc lập với chương trình C.
4. Chạy chương trình `odd_sum` với cùng dữ liệu đầu vào.
5. Đọc kết quả do chương trình C trả về và so sánh với kết quả chuẩn của Python.
6. Ghi nhận trạng thái thành công hoặc thất bại của từng lượt kiểm thử.

Với 1.000 kích thước đầu vào và 3 giá trị `power` cho mỗi kích thước, tổng số lượt kiểm thử là:

\[
1000 \times 3 = 3000
\]

## 3.5. Kết quả kiểm thử

Trong phiên bản V2, chương trình `odd_sum` đã được kiểm thử bằng script Python với các kích thước đầu vào tăng dần theo từng bước 20 chữ số, từ 20 đến 20.000 chữ số.

Cả ba giá trị `power` từ `1` đến `3` đều được kiểm tra.

**Kết quả thực thi script kiểm thử:**

| Chỉ tiêu | Kết quả |
|---|---:|
| Tổng số lượt kiểm thử | 3.000 |
| Số lượt thành công | 3.000 |
| Số lượt thất bại | 0 |
| Tỷ lệ thành công | 100% |

Tất cả 3.000 lượt kiểm thử đã thực hiện đều cho kết quả khớp với đáp án chuẩn của Python.

Kết quả này phản ánh phạm vi dữ liệu và các trường hợp đã kiểm tra, không đồng nghĩa với việc chương trình chắc chắn không còn lỗi trong mọi tình huống.

---

# 4. Test Bench và phân tích độ phức tạp

## 4.1. Phương pháp đo

Test Bench được sử dụng để đo thời gian thực thi của ba hàm:

- `bn_mul()`
- `bn_divmod_small()`
- `odd_sum()`

Kích thước đầu vào được khảo sát trong khoảng từ 100 đến 20.000 chữ số.

Mục tiêu là thu thập dữ liệu thực nghiệm để đánh giá hiệu năng và so sánh xu hướng tăng trưởng thời gian chạy với độ phức tạp lý thuyết.

### Cách đo

Sử dụng hàm `clock_gettime()` với `CLOCK_MONOTONIC` để đo thời gian thực thi.

### Số lần lặp

Các hàm `bn_mul()` và `odd_sum()` được lặp lại 20, 5 hoặc 2 lần tùy theo kích thước đầu vào, nhằm hạn chế thời gian benchmark quá dài khi xử lý số nguyên lớn.

### Lưu kết quả

Thời gian đo được, tính bằng nanosecond (ns), được ghi vào file:

```text
benchmark.csv
```

Dữ liệu được sử dụng để phân tích hiệu năng và vẽ biểu đồ log-log theo kích thước đầu vào.

## 4.2. Kết quả đo

Kết quả benchmark thu được như sau:

| Hàm | Độ phức tạp lý thuyết | Số mũ lý thuyết | Số mũ thực nghiệm |
|---|---|---:|---:|
| `bn_mul()` | \(O(n^2)\) | 2 | 1.776 |
| `bn_divmod_small()` | \(O(n)\) | 1 | 0.755 |
| `odd_sum()` | \(O(n^2)\) | 2 | 1.730 |

Trong bảng trên, `n` biểu thị kích thước đầu vào, được đo bằng số lượng `limb` hoặc đại lượng kích thước tương ứng được sử dụng nhất quán trong benchmark.

### Phân tích độ phức tạp lý thuyết

**`bn_mul()`**

Thuật toán nhân thông thường sử dụng hai vòng lặp lồng nhau để nhân từng cặp `limb`. Với hai số có kích thước xấp xỉ `n` `limb`, độ phức tạp thời gian là:

\[
T(n) = O(n^2)
\]

**`bn_divmod_small()`**

Hàm duyệt qua các `limb` của số nguyên lớn đúng một lượt. Mỗi bước thực hiện một số phép toán số học có chi phí hằng số khi `divisor` là số nguyên nhỏ phù hợp.

Do đó:

\[
T(n) = O(n)
\]

**`odd_sum()`**

Chương trình sử dụng các phép toán số nguyên lớn, trong đó có phép nhân được thực hiện bằng `bn_mul()`.

Khi phép nhân chi phối thời gian thực thi, độ phức tạp của chương trình có thể được ước lượng là:

\[
T(n) = O(n^2)
\]

Nhận định này áp dụng cho cách triển khai hiện tại và phụ thuộc vào các phép toán được thực hiện trong từng nhánh `power`.

## 4.3. Nhận xét

### `bn_mul()`

Số mũ thực nghiệm là `1.776`, thấp hơn số mũ lý thuyết `2`.

Giá trị `1.776` là số mũ ước lượng từ dữ liệu benchmark, trong khi `2` là số mũ lý thuyết của thuật toán nhân thông thường.

Sự khác biệt có thể xuất phát từ:

- Phạm vi kích thước dữ liệu đo còn hữu hạn.
- Chi phí cố định của chương trình.
- Tối ưu hóa của compiler.
- Số lần lặp và phương pháp tổng hợp thời gian đo.
- Nhiễu trong quá trình đo hiệu năng.

Kết quả thực nghiệm cho thấy thời gian thực thi có xu hướng tăng gần với bậc hai khi kích thước đầu vào tăng. Tuy nhiên, kết quả này không chứng minh chính xác độ phức tạp tiệm cận của thuật toán.

### `bn_divmod_small()`

Số mũ thực nghiệm là `0.755`, thấp hơn giá trị lý thuyết `1`.

Về lý thuyết, hàm duyệt qua từng `limb` đúng một lần nên có độ phức tạp tuyến tính:

\[
T(n) = O(n)
\]

Do thời gian thực thi của phép chia có thể ngắn so với chi phí cố định và nhiễu đo, kết quả hồi quy trên một phạm vi dữ liệu hữu hạn có thể cho số mũ thấp hơn `1`.

Vì vậy, số mũ thực nghiệm `0.755` không có nghĩa là thuật toán có độ phức tạp tiệm cận thấp hơn tuyến tính.

### `odd_sum()`

Số mũ thực nghiệm là `1.730`, khá gần số mũ lý thuyết `2`.

Điều này phù hợp với phân tích mã nguồn: chương trình thực hiện các phép toán số nguyên lớn, trong đó phép nhân thông thường có độ phức tạp \(O(n^2)\).

Tuy nhiên, số mũ thực nghiệm chỉ phản ánh dữ liệu đo được trong phạm vi benchmark, không chứng minh chính xác độ phức tạp tiệm cận của chương trình.

---

# 5. Cải thiện so với phiên bản trước

So với phiên bản trước, phiên bản V2 đã có những cải thiện sau:

- Bổ sung kiểm thử cho các trường hợp biên và các trường hợp thông thường.
- Kiểm tra chương trình `odd_sum` với nhiều kích thước đầu vào, từ 20 đến 20.000 chữ số.
- Đối chiếu kết quả với kiểu số nguyên có độ chính xác tùy ý của Python.
- Kiểm tra cả ba giá trị `power = 1`, `power = 2` và `power = 3`.
- Thực hiện tổng cộng 3.000 lượt kiểm thử, với 3.000 lượt thành công và 0 lượt thất bại.
- Bổ sung chương trình Test Bench để đo thời gian thực thi của các hàm xử lý số nguyên lớn.
- Thu thập dữ liệu benchmark và phân tích xu hướng tăng trưởng thời gian thực thi.
- So sánh số mũ thực nghiệm với độ phức tạp lý thuyết của các thuật toán.

Kết quả kiểm thử cho thấy chương trình hoạt động chính xác trên các trường hợp đã kiểm tra. Kết quả Test Bench cũng cho thấy xu hướng tăng trưởng thời gian thực thi tương đối phù hợp với phân tích độ phức tạp lý thuyết.

---

# 6. Hạn chế

Mặc dù thư viện số nguyên lớn và chương trình `odd_sum` đã vượt qua các bài kiểm thử hiện tại, hệ thống vẫn còn một số hạn chế cần cải thiện.

### 6.1. Sử dụng bộ nhớ chưa tối ưu

Cấu trúc `bn_t` sử dụng mảng `limbs[10000]` có kích thước cố định. Do đó, các số nguyên nhỏ cũng sử dụng cấu trúc có khả năng lưu trữ số lượng `limb` lớn, dẫn đến việc sử dụng bộ nhớ chưa tối ưu.

### 6.2. Phạm vi kiểm thử chưa bao phủ mọi tình huống

Mặc dù chương trình đã vượt qua 3.000 lượt kiểm thử `odd_sum`, kết quả này chỉ xác nhận tính chính xác trên những dữ liệu và trường hợp đã kiểm tra.

Các tình huống như dữ liệu đầu vào không hợp lệ, ký tự không phải chữ số, số vượt giới hạn dung lượng hoặc các trường hợp biên chưa được kiểm tra vẫn có thể phát sinh lỗi.

### 6.3. Hiệu năng thực nghiệm phụ thuộc vào môi trường chạy

Kết quả benchmark có thể bị ảnh hưởng bởi compiler, mức tối ưu hóa, phần cứng, số lần lặp và nhiễu đo.

Vì vậy, cần thực hiện thêm các phép đo trên nhiều kích thước đầu vào và nhiều môi trường khác nhau để đánh giá hiệu năng chính xác hơn.

---

# 7. Hướng phát triển và tối ưu

### 7.1. Kiểm thử hiệu năng trên nhiều môi trường

Tiếp tục thực hiện benchmark trên nhiều môi trường khác nhau để so sánh sự khác biệt về thời gian thực thi.

Việc kiểm thử trên các cấu hình compiler, mức tối ưu hóa và phần cứng khác nhau giúp đánh giá rõ hơn hiệu năng của thư viện.

### 7.2. Bổ sung kiểm tra dữ liệu đầu vào

Bổ sung các trường hợp kiểm thử và cơ chế kiểm tra đầu vào để phát hiện dữ liệu không hợp lệ, chẳng hạn như chuỗi chứa chữ cái hoặc ký tự không phải chữ số.

Điều này giúp chương trình xử lý đầu vào an toàn và rõ ràng hơn.

### 7.3. Cấp phát bộ nhớ theo kích thước số

Hiện tại, `bn_t` sử dụng mảng cố định đủ lớn cho các trường hợp tính toán dự kiến. Vì vậy, mỗi số nguyên, dù lớn hay nhỏ, đều có cấu trúc lưu trữ với cùng dung lượng tối đa.

Một hướng phát triển là chuyển sang cấp phát bộ nhớ động theo kích thước thực tế của số nguyên lớn. Cách tiếp cận này có thể giúp giảm lượng bộ nhớ sử dụng cho các số nhỏ và tăng tính linh hoạt khi xử lý dữ liệu.

### 7.4. Sử dụng thuật toán Karatsuba

Thuật toán nhân thông thường có độ phức tạp:

\[
O(n^2)
\]

Trong khi đó, thuật toán Karatsuba có độ phức tạp:

\[
O(n^{\log_2 3}) \approx O(n^{1.585})
\]

Do đó, Karatsuba là một hướng phát triển tiềm năng nhằm cải thiện hiệu năng phép nhân khi xử lý các số nguyên lớn.

Trong tương lai, có thể nghiên cứu và triển khai thuật toán này, sau đó so sánh với thuật toán nhân thông thường bằng Test Bench để xác định ngưỡng kích thước đầu vào phù hợp.