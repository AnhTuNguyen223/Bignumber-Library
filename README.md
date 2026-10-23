### Thiết kế
N có tối đa 20000 chữ số 
Đầu vào và đầu ra phải là kiểu chuỗi 
Từ chuỗi tách thành các đoạn limbs nhỏ
Limbs có 9 chữ số lưu trong 1 kiểu dữ liệu bn_t có mảng Limbs[10000] và số lượng limbs(len). Mỗi Limbs có 9 chữ số  vậy mảng 10000 limbs sẽ được 9x10000 = 90000 chữ số, đủ để lưu được trường hợp tính toán 20000 chữ số tính tổng lũy thừa.
Limb[0] là phần tử có trọng số cao nhất rồi giảm dần từ trái sang phải
- Cơ số limb : 10^9
- Kiểu limb: uint32_t 
- Thứ tự limb: từ trái sang phải giảm dần
## Thiết kế thư viện
# Hàm bn_from_string
- Thuật toán: đầu tiên là tính chiều dài chuỗi sau đó tính ra được số lượng limbs cần thiết cho toàn bộ chuỗi
    +) Tính toán số lượng phần tử ở đầu tiên ở mảng limbs, phần tử này có thể có số lượng ít hơn 9 chữ số. Nên phải tính xem có bao nhiêu chữ số dư để cho vào phần tử đầu. Các phần tử tiếp theo sẽ có 9 chữ số tương ứng với chuỗi cắt nhỏ
    +) Kết quả là sẽ được 1 big_number có các limbs tương ứng với chuỗi dưới dạng uint32_t, mỗi limbs sẽ chứa 9 chữ số.
- Độ phức tạp: O(n2)
# Hàm bn_to_string
- Chuyển Limb đầu tiên thành chuỗi do limb đầu có thể là từ 1 đến 9 chữ số nên phải tách ra, sau đó chuyển các limb sau về chuỗi với mặc định là 9 chữ số.
- Độ phức tạp: O(n)
# Hàm bn_compare
- Quy ước:: 1 là a>b , 0 là a<b , 2 là a =b
- Thuật toán: đầu tiên xét số lượng limb nếu số nào có nhiều limb hơn thì số đó lớn hơn
+) Nếu bằng nhau thì tiếp tục so sánh các limb với nhau. So sánh thì so sánh bắt đầu từ limb[0] do có trọng số lớn nhất. Nếu tất cả limb bằng nhau thì 2 số bằng nhau.
- Độ phức tạp: O(n)
# Hàm bn_add
- Thuật toán: Đầu tiên tìm số có số lượng limb lớn nhất sau đó tiến hành cộng từ phần tử cuối lên, nếu tràn thì lấy carry cộng cho limb tiếp theo. Trường hợp tràn limb có nghĩa là phải cho thêm 1 limbs mới với trường hợp 2 limbs đầu bị tràn.
- Độ phức tạp: O(n)
# Hàm bn_sub 
- Chỉ thực hiện khi a > b
- Thuật toán: Trừ các limb ở dưới lên. Trường hợp limb a nhỏ hơn limb b thì cộng với base, base ở đây là cơ số 10^9 và borrow sẽ được trừ vào limb a tiếp theo.
- Độ phức tạp: O(n)
# Hàm bn_mul
- Thuật toán: thực hiện phép nhân từng cặp limb. Mỗi tích được cộng voiws giá trị đã cos tại vị trí tương ứng và phần nhớ. Sau đó, lấy phần dư theo cơ số 10^9 để lưu vào limb hiện tại và lấy thương để chuyển sang limb tiếp theo.
- Độ phức tạp: O(n2);
# Hàm bn_divmod_small 
- Thuật toán: Thực hiện phép chia Big Number cho một số nguyên nhỏ (divisor) bằng cách duyệt từng limb từ trái sang phải. Ở mỗi bước, giá trị tạm thời được tính bằng số dư của bước trước nhân với cơ số (10^9), sau đó cộng với limb hiện tại. Thương sẽ lấy phần nguyên của phép chia.
- Số dư sẽ bằng giá trị temp chia lấy dư cho số chia divisor. 
- Độ phức tạp: O(n)

### Phương pháp kiểm thử
# Nội dung kiểm thử
- Kiểm thử thư viện big_number.h và chương trình tính oddsum. 
# Nguồn đáp án chuẩn
- Bản V1 chỉ kiểm thử các giá trị nhỏ có thể tính toán nên có thể tin cậy vào kết quả này.
Ví dụ: 
./test_bignum
123 + 456 = 579
456 - 123 = 333
123 * 456 = 56088
456 / 3 = 152
All Big Number tests passed!
./test_odd_sum
odd_sum(3, 1) = 4
odd_sum(3, 2) = 10
odd_sum(3, 3) = 28
odd_sum(9, 1) = 25
odd_sum(9, 2) = 165
odd_sum(9, 3) = 1225
odd_sum(10, 1) = 25
odd_sum(0, 3) = 0
### Test Bench 
- Dùng để đo thời gian thực thi của 3 hàm bn_mul(), bn_divmod_small() và odd_sum() khi kích thước đầu vào từ 100 - 20000 số. sau đó lưu vào file csv để phân tích hiệu năng và độ phức tạp thuật toán.
- Cách đo: Sử dụng clock_gettime(CLOCK_MONOTONIC) để đo thời gian thực thi của bn_mul(), bn_divmod_small() và odd_sum().
- Số lần lặp: bn_mul() và odd_sum() lặp 20, 5 hoặc 2 lần tùy kích thước đầu vào; bn_di

- Lưu kết quả: Ghi thời gian đo được (ns) vào benchmark.csv để phân tích và vẽ biểu đồ log-log.

## Kết quả đo
-Xu hướng: Thời gian thực thi tăng khi kích thước đầu vào tăng.

- Độ phức tạp lý thuyết: Phép chia Big Number cho một số nguyên nhỏ có độ phức tạp (O(n)), với (n) là số limb của số lớn. Do mỗi limb chứa tối đa 9 chữ số thập phân, số limb tăng tỷ lệ với số chữ số đầu vào.
uat
### HẠN CHẾ
- Hiện tại hệ thống chưa kiểm thử được đầy đủ và bao quát được thư viện big_num và chương trình odd_sum do em chưa tìm thấy được nguồn dữ liệu chuẩn để so sánh vậy nên hiện tại em mới kiểm thử bằng các số nhỏ trong tầm tính toán thì thấy hệ thống chính xác.

### Hướng cải thiện
- Tìm được nguồn dữ liệu chuẩn cho big number sau đó viết chương trình kiểm thử đầy đủ các trường hợp biên.  
