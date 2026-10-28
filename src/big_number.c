#include "big_number.h"

#include <stdio.h>
#include <string.h>
#include <limits.h>

bn_t bn_from_string(char *p){
    bn_t big_number;
    size_t lenght_of_string = strlen(p); 
    size_t len_limbs = (lenght_of_string + 8) /9 ;
    big_number.len = len_limbs ;

    uint8_t number=0;
    uint32_t value_9_digit=0;
    uint32_t pos = 0 ;
    size_t first_group = strlen(p) % 9;

    if (first_group == 0) {
        first_group = 9;
    }
    for (uint32_t i=0; i < big_number.len ; i++){ // moi lan lap lai tinh group len 1 lan de tinh so trong phan tu dau limbs
        size_t group_len=0;
        if(i == 0){
            group_len = first_group;
        }
        else
            group_len = 9 ;

        value_9_digit = 0;

        for (uint32_t j=0; j < group_len ; j++){
            char c = p[pos++];

            number = (uint8_t) (c - '0');

            value_9_digit = value_9_digit*10 +number;
        }
        big_number.limbs[i] = value_9_digit;

    }
    bn_normalize(&big_number);
    return big_number;
    
}
void bn_to_string(bn_t *bn, char *str_out ){
    
    int pos = snprintf(str_out, 32, "%u", bn->limbs[0]);

    for(uint32_t i = 1; i < bn->len ; i ++){

        pos += snprintf(str_out+pos, 10, "%09u" , bn->limbs[i]);

    }
}
// 1: a>b ; 0 a<b ; 2: a=b
int bn_compare(bn_t a, bn_t b){
    if (a.len > b.len)  
        return 1;
    else if( a.len < b.len )
        return 0;
    else {
        for ( uint32_t i=0 ; i < a.len ; i++){
            if ( a.limbs[i] > b.limbs[i])
                return 1;
            else if ( a.limbs[i] < b.limbs[i])
                return 0;
        }
        return 2; 
    }
}
bn_t bn_add(bn_t a, bn_t b){
    
    bn_t result = {0};
    uint32_t base_9_number = 1000000000;
    size_t max_len;
    if (a.len > b.len)
        max_len = a.len;
    else    
        max_len = b.len;

    uint32_t sum;
    uint32_t carry = 0;
    result.len = max_len;

    for(uint32_t i = 0 ; i<max_len; i++){
        sum = carry;

        if ( i < a.len)
            sum+= a.limbs[a.len - 1 -i];
        if (i < b.len)
            sum += b.limbs[b.len - 1 - i];

        result.limbs[max_len - 1 -i] = sum % base_9_number;
        carry = sum /base_9_number ;
    }
    if (carry > 0) // tran sang limbs moi' thi dich toan bo limb tu trai sang phai, de lai limbs[0] = carry
    {
        for (size_t i = result.len ; i> 0 ; i--)
        {
            result.limbs[i] = result.limbs[i-1];
        }
        result.limbs[0] = carry;
        result.len ++;
    }
    bn_normalize(&result);
    return result;
}

bn_t bn_sub(bn_t a, bn_t b)
{
    bn_t result;
    int borrow = 0;
    const int64_t base = 1000000000LL;

    if (bn_compare(a, b) != 1) {
        result.len = 1;
        result.limbs[0] = 0;
        return result;
    }

    size_t i = a.len;
    size_t j = b.len;

    result.len = a.len;

    while (i > 0U) {
        i--;

        int64_t diff = (int64_t)a.limbs[i] - borrow;

        if (j > 0U) {
            j--;
            diff -= (int64_t)b.limbs[j];
        }

        if (diff < 0) {
            diff += base;
            borrow = 1;
        } else {
            borrow = 0;
        }

        result.limbs[i] = (uint32_t)diff;
    }

    // xoa cac limb 0 o dau ket qua
    size_t start = 0U;

    while (start + 1U < result.len &&
           result.limbs[start] == 0U) {
        start++;
    }
    if (start > 0U) {
        size_t new_len = result.len - start;

        for (size_t k = 0U; k < new_len; k++) {
            result.limbs[k] = result.limbs[k + start];
        }
        result.len = new_len;
    }
    bn_normalize(&result);
    return result;
}

bn_t bn_mul(bn_t a, bn_t b)
{
    bn_t result = {0};
    const uint64_t BASE = 1000000000ULL;

    if (a.len == 0 || b.len == 0) {
        result.len = 1;
        result.limbs[0] = 0;
        return result;
    }

    result.len = a.len + b.len;

    for (int i = (int)a.len - 1; i >= 0; i--) {
        uint64_t carry = 0;

        for (int j = (int)b.len - 1; j >= 0; j--) {
            uint64_t temp =
                (uint64_t)a.limbs[i] * b.limbs[j]
                + result.limbs[i + j + 1]
                + carry;

            result.limbs[i + j + 1] = (uint32_t)(temp % BASE);
            carry = temp / BASE;
        }

        int pos = i;

        while (carry > 0 && pos >= 0) {
            uint64_t temp = (uint64_t)result.limbs[pos] + carry;

            result.limbs[pos] = (uint32_t)(temp % BASE);
            carry = temp / BASE;
            pos--;
        }
    }

    bn_normalize(&result);
    return result;
}
void bn_divmod_small(bn_t a, uint32_t divisor, bn_t *quotient, uint32_t *remainder){
    uint64_t remain = 0;
    uint32_t base = 1000000000;
    quotient ->len = a.len;

    for (uint32_t i = 0; i < a.len ; i++){
        uint64_t temp = remain * base + a.limbs[i];
        quotient ->limbs[i] = (uint32_t)(temp / divisor);
        remain = temp % divisor;
    }
    *remainder = (uint32_t)remain ;
    bn_normalize(quotient);
}
bn_t odd_sum(bn_t bn, uint8_t power){
    // dung cong thuc K=(N+1)/2
    bn_t one = {0};
    one.len = 1;
    one.limbs[0] = 1 ;
    bn_t four = {0};
    four.len = 1;
    four.limbs[0]= 4;
    bn_t bn_cong_1 = bn_add(bn, one); 
    bn_t two = {0};
    two.len = 1;
    two.limbs[0] = 2;
    bn_t k;
    uint32_t remainer;
    bn_divmod_small(bn_cong_1, 2, &k, &remainer);
    bn_t kk, sum_1, sub_1, mul_1, div;
    kk = bn_mul(k,k);
    // luy thua bac 1 thi tong so le la: k^2
    if( power == 1){
        bn_normalize(&kk);
        return kk;
    }
    else if (power == 2)
    {
        sum_1 = bn_mul(four,kk);        // 4k^2
        sub_1 = bn_sub(sum_1, one);     // 4k^2 +1
        mul_1 = bn_mul(k, sub_1);       // k(4k^2 +1)
        bn_divmod_small(mul_1,3,&div, &remainer);  // k(4k^2 +1)/3
        bn_normalize(&div);
        return div;
    }
    else if (power == 3)
    {
        sum_1 = bn_mul(two, kk);       // 2(k^2)
        sub_1 = bn_sub(sum_1, one);    // 2(k^2 - 1)
        mul_1 = bn_mul(kk, sub_1);     // k^2(2k^2 - 1)
        bn_normalize(&mul_1);
        return mul_1;
    }
    return one;
}

void bn_normalize(bn_t *bn)
{
    if (bn == NULL) {
        return;
    }

    if (bn->len == 0) {
        bn->len = 1;
        bn->limbs[0] = 0;
        return;
    }

    size_t i = 0;

    while (i + 1 < bn->len && bn->limbs[i] == 0) {
        i++;
    }

    if (i > 0) {
        size_t new_len = bn->len - i;

        for (size_t j = 0; j < new_len; j++) {
            bn->limbs[j] = bn->limbs[j + i];
        }

        bn->len = new_len;
    }
}
