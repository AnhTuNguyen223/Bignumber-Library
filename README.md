# Thiết kế
N có tối đa 20000 chữ số .
Đầu vào và đầu ra phải là kiểu chuỗi, và là số nguyên không âm
Từ chuỗi tách thành các đoạn limbs nhỏ.
Limbs có 9 chữ số lưu trong 1 kiểu dữ liệu bn_t có mảng Limbs[10000] và số lượng limbs(len). Mỗi Limbs có 9 chữ số  vậy mảng 10000 limbs sẽ được 9x10000 = 90000 chữ số, đủ để lưu được trường hợp tính toán 20000 chữ số tính tổng lũy thừa.
Limb[0] là phần tử có trọng số cao nhất rồi giảm dần từ trái sang phải
- Cơ số limb : 10^9
- Kiểu limb: uint32_t 
- Thứ tự limb: từ trái sang phải giảm dần
Power nhận các giá trị 1, 2 hoặc 3
## Thiết kế thư viện
### Hàm bn_from_string
- Thuật toán: đầu tiên là tính chiều dài chuỗi sau đó tính ra được số lượng limbs cần thiết cho toàn bộ chuỗi
    +) Tính toán số lượng phần tử ở đầu tiên ở mảng limbs, phần tử này có thể có số lượng ít hơn 9 chữ số. Nên phải tính xem có bao nhiêu chữ số dư để cho vào phần tử đầu. Các phần tử tiếp theo sẽ có 9 chữ số tương ứng với chuỗi cắt nhỏ
    +) Kết quả là sẽ được 1 big_number có các limbs tương ứng với chuỗi dưới dạng uint32_t, mỗi limbs sẽ chứa 9 chữ số.
- Độ phức tạp: O(n)
### Hàm bn_to_string
- Chuyển Limb đầu tiên thành chuỗi do limb đầu có thể là từ 1 đến 9 chữ số nên phải tách ra, sau đó chuyển các limb sau về chuỗi với mặc định là 9 chữ số.
- Độ phức tạp: O(n)
### Hàm bn_compare
- Quy ước:: 1 là a>b , 0 là a<b , 2 là a =b
- Thuật toán: đầu tiên xét số lượng limb nếu số nào có nhiều limb hơn thì số đó lớn hơn
+) Nếu bằng nhau thì tiếp tục so sánh các limb với nhau. So sánh thì so sánh bắt đầu từ limb[0] do có trọng số lớn nhất. Nếu tất cả limb bằng nhau thì 2 số bằng nhau.
- Độ phức tạp: O(n)
### Hàm bn_add
- Thuật toán: Đầu tiên tìm số có số lượng limb lớn nhất sau đó tiến hành cộng từ phần tử cuối lên, nếu tràn thì lấy carry cộng cho limb tiếp theo. Trường hợp tràn limb có nghĩa là phải cho thêm 1 limbs mới với trường hợp 2 limbs đầu bị tràn.
- Độ phức tạp: O(n)
### Hàm bn_sub 
- Chỉ thực hiện khi a > b
- Thuật toán: Trừ các limb ở dưới lên. Trường hợp limb a nhỏ hơn limb b thì cộng với base, base ở đây là cơ số 10^9 và borrow sẽ được trừ vào limb a tiếp theo.
- Độ phức tạp: O(n)
### Hàm bn_mul
- Thuật toán: thực hiện phép nhân từng cặp limb. Mỗi tích được cộng voiws giá trị đã cos tại vị trí tương ứng và phần nhớ. Sau đó, lấy phần dư theo cơ số 10^9 để lưu vào limb hiện tại và lấy thương để chuyển sang limb tiếp theo.
- Độ phức tạp: O(n2);
### Hàm bn_divmod_small 
- Thuật toán: Thực hiện phép chia Big Number cho một số nguyên nhỏ (divisor) bằng cách duyệt từng limb từ trái sang phải. Ở mỗi bước, giá trị tạm thời được tính bằng số dư của bước trước nhân với cơ số (10^9), sau đó cộng với limb hiện tại. Thương sẽ lấy phần nguyên của phép chia.
- Số dư sẽ bằng giá trị temp chia lấy dư cho số chia divisor. 
- Độ phức tạp: O(n)
## Thiết kế chương trình odd_sum
- Đầu vào của chương trình: 1 số nguyên N có tối đa 20000 chữ số được nhập từ 1 file input.
- Chương trình có power từ 1 đến 3 tương ứng với lũy thừa số lẻ cho tổng của chúng.
- Kết quả được xuất ra bằng bn_t và dưới dạng chuỗi.
### Công thức tính toán
- Với power = 1 thì công thức tính tổng số lẻ là: K^2. K là số lượng số lẻ được tính bằng (N+1)/2.
- Với Power = 2 thì công thức là: K(4K^2 -1)/3
- Với Power = 3 thì công thức là: K^2(2K^2 -1).

# Cách Build
## Cách chạy file
- Vào thư mục src: make 
    +) make để build chương trình ra file odd_sum
- Vào thư mục src: ./odd_sum input.txt [--power p]
    +) input.txt là nơi nhập số big number để test
    +) p nhận giá trị 1 2 3 tương ứng với lũy thừa các bậc 1 2 3. ví dụ: ./odd_sum input.txt --power 2
## Cách chạy file Kiểm thử
- Có 2 thư mục kiểm thử: test và test_bench. test sẽ gồm test thư viện và test chương trình odd_sum
- Cách chạy test biên: tại thư mục src chạy: make test_edge
- Cách chạy test chương trình odd_sum: make test_odd_sum
- Cách chạy test thư viện thông qua c : make test_library
- Cách chạy test thư viện thông qua python : make test_lib
- Cách chạy test tính chất toán học của phép toàn big number: make test_properties.
- Cách chạy test khả năng xử lý số lớn, phép cộng và phép nhân: make test_capacity.
- Kiểm tra các lỗi bộ nhớ và hành vi không xác định bằng ASan/UBSan: make test_capacity_sanitize. 
- Cách chạy full test: make test
## Cách chạy file test bench
- cd vào thư mục src: make bench
- Sau khi chạy thì sẽ kết hợp chạy bài test và vẽ biểu đồ, hiện thị độ phức tạp.
# Phương pháp kiểm thử
## Nội dung kiểm thử
- Kiểm thử thư viện big_number.h và chương trình tính oddsum. 
## Nguồn đáp án chuẩn
- Sử dụng thư viện số tự nhiên của python có độ chính xác cao, độ dài tùy ý.
- Bản V2 đã kiểm tra được toàn bộ chương trình odd_sum bằng chương trình test python theo từng bậc 20 chữ số. Kết quả pass toàn bộ từ power 1 đến power 3.
## Cách kiểm thử
### Kiểm thử thư viện
Mục tiêu: Kiểm tra tính chính xác của các hàm xử lý số nguyên lớn trong thư viện big_number.h.
Các hàm cần kiểm tra bao gồm:
bn_from_string(): Chuyển đổi chuỗi thập phân thành cấu trúc Big Number.
bn_to_string(): Chuyển đổi Big Number thành chuỗi thập phân.
bn_compare(): So sánh hai số nguyên lớn.

bn_add(): Cộng hai số nguyên lớn.

bn_sub(): Trừ hai số nguyên lớn.

bn_mul(): Nhân hai số nguyên lớn.

bn_divmod_small(): Chia số nguyên lớn cho một số nguyên nhỏ, trả về thương và số dư.

Kết quả của từng phép toán được đối chiếu với kết quả tính toán độc lập để phát hiện sai lệch. Qua đó, kiểm tra tính chính xác của các phép chuyển đổi và phép toán số nguyên lớn.
### Kiểm thử odd_sum
Phương pháp thực hiện:

Sử dụng Python để tạo dữ liệu đầu vào có độ dài tăng dần từ 20 đến 20.000 chữ số, với bước tăng 20 chữ số.

Với mỗi kích thước đầu vào, lần lượt kiểm tra các giá trị power từ 1 đến 3.

Sử dụng kiểu số nguyên có độ chính xác tùy ý của Python để tính kết quả chuẩn độc lập với chương trình C.

Chạy chương trình odd_sum với cùng dữ liệu đầu vào, lấy kết quả trả về và so sánh với kết quả chuẩn của Python.

Ghi nhận trạng thái thành công hoặc thất bại của từng lượt kiểm thử.

Với 1.000 kích thước đầu vào và 3 giá trị power cho mỗi kích thước, tổng cộng có 3.000 lượt kiểm thử.


## Kết quả
- Đã kiểm tra được toàn bộ chương trình odd_sum bằng chương trình test python theo từng bậc 20 chữ số. Kết quả pass toàn bộ từ power 1 đến power 3.
- Test biên cho kết quả pass
- Test các tính chất toán học như giao hoán... pass
- Test dung lượng cho kết quả pass, chạy cùng với AddressSanitizer và UndefinedBehaviorSanitizer
- Test thư viện dùng python: pass.
- Test khả năng xử lý số lớn: pass. 
# Test Bench 
- Dùng để đo thời gian thực thi của 3 hàm bn_mul(), bn_divmod_small() và odd_sum() khi kích thước đầu vào từ 100 - 20000 số. sau đó lưu vào file csv để phân tích hiệu năng và độ phức tạp thuật toán.
- Cách đo: Sử dụng clock_gettime(CLOCK_MONOTONIC) để đo thời gian thực thi của bn_mul(), bn_divmod_small() và odd_sum().
- Số lần lặp: bn_mul() và odd_sum() lặp 20, 5 hoặc 2 lần tùy kích thước đầu vào; bn_di
- Lưu kết quả: Ghi thời gian đo được (ns) vào benchmark.csv để phân tích và vẽ biểu đồ log-log.
## Kết quả đo
- Độ phức tạp lý thuyết của
    bn_mul() : O(n^2) Số mũ lý thuyết là 2 Số mũ thực tế là 1.776.
- Độ phức tạp lý thuyết của
    bn_divmod_small() : O(n) Số mũ lý thuyết là 1 Số mũ thực tế là 0.755.
- Độ phức tạp lý thuyết của
    odd_sum() : O(n^2) Số mũ lý thuyết là 2 Số mũ thực tế là 1.730.
    +) Do odd_sum() sử dụng hàm bn_mul() để nhân các số nguyên lớn mà hàm này có 2 vòng lặp lồng nhau nên độ phức tạp là O(n^2).
## Nhận xét
### bn_mul()
Số mũ thực nghiệm là 1.776, thấp hơn số mũ lý thuyết 2. Giá trị 1.776 là số mũ ước lượng từ dữ liệu benchmark. Giá trị 2 là số mũ lý thuyết của thuật toán nhân thông thường. Sự khác biệt có thể do kích thước dữ liệu đo, chi phí cố định, tối ưu hóa compiler và nhiễu đo. Kết quả thực nghiệm phù hợp với xu hướng tăng trưởng gần bậc hai, nhưng chưa chứng minh chính xác độ phức tạp tiệm cận. 

### bn_divmod_small()
Số mũ thực nghiệm là 0.755, thấp hơn giá trị lý thuyết 1. Về lý thuyết, hàm duyệt qua từng limb đúng một lần nên có độ phức tạp tuyến tính. Do thời gian chạy của phép chia có thể khá ngắn so với các chi phí cố định và nhiễu đo, kết quả hồi quy có thể thấp hơn 1. 

### odd_sum()
Số mũ thực nghiệm là 1.730, khá gần số mũ lý thuyết 2. Điều này phù hợp với phân tích code: hàm gọi bn_mul() để thực hiện các phép nhân số nguyên lớn, vốn có độ phức tạp O(n^2). Tuy nhiên, số mũ thực nghiệm chỉ phản ánh dữ liệu đo được trong phạm vi benchmark, không chứng minh chính xác độ phức tạp tiệm cận.

# CẢI THIỆN SO VỚI BẢN TRƯỚC
- Hiện tại hệ thống đã bao phủ hết các trường hợp biên và các trường hợp bình thường. Kết quả test thành công, không có case nào lỗi.
- Kết quả testbench cho thấy kết quả có xu hướng đúng với lý thuyết.
- Phạm vi kiểm thử tăng lên bao trọn chương trình.

# HẠN CHẾ
- Sử dụng bộ nhớ chưa tối ưu
- Hiệu năng thực nghiệm còn phụ thuộc vào môi trường chạy. Kết quả có thể bị ảnh hưởng bởi compiler, mức tối ưu hóa, phần cứng, số lần lặp và nhiễu đo. Vì vậy, cần thực hiện thêm các phép đo trên nhiều kích thước đầu vào và nhiều môi trường khác nhau để đánh giá hiệu năng chính xác hơn.

# HƯỚNG PHÁT TRIỂN, TỐI ƯU

- Kiểm thử hiệu năng trên nhiều môi trường khác để so sánh sự khác biệt
- Thêm các trường hợp kiểm tra đầu vào có hợp lệ hay không có lẫn chữ trong số hay không.
- Cấp phát bộ nhớ phù hợp theo kích thước số
- Hiện tại bn_t đang sử dụng 1 mảng cố định đủ lớn cho mọi trường hợp vì vậy mỗi số dù lớn hay nhỏ đều chiếm dung lượng tương đương
- Dùng thuật toán Karatsuba
- Em có tìm hiểu được thuật toán này có độ phực tạp chỉ là O(n^1.585) thay vì O(n^2) như nhân thông thường. Vậy đây có thể là hướng phát triển tốt trong tương lai.

