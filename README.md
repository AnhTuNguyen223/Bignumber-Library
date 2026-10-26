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

1. Xác định số có số lượng `limb`