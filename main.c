#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define MAX_LIMBS 10000
typedef struct {
    uint32_t limbs[MAX_LIMBS];
    uint32_t len;
}bn_t;


bn_t bn_from_string(char *p){
    bn_t big_number;
    uint16_t lenght_of_string = strlen(p); 
    uint16_t len_limbs = (lenght_of_string + 8) /9 ;
    big_number.len = len_limbs ;

    uint8_t number=0;
    uint32_t value_9_digit=0;
    uint32_t pos = 0 ;
    uint32_t first_group = strlen(p) % 9;

    for (int i=0; i < big_number.len ; i++){ // moi lan lap lai tinh group len 1 lan de tinh so trong phan tu dau limbs
        uint32_t group_len=0;
        if(i == 0){
            group_len = first_group;
        }
        else
            group_len = 9 ;

        value_9_digit = 0;

        for (int j=0; j < group_len ; j++){
            char c = p[pos++];

            number = (uint8_t) (c - '0');

            value_9_digit = value_9_digit*10 +number;
        }
        big_number.limbs[i] = value_9_digit;

    }
    return big_number;
    
}
void bn_to_string(bn_t *bn, char *str_out ){
    
    int pos = snprintf(str_out, 32, "%u", bn->limbs[0]);

    for(int i = 1; i < bn->len ; i ++){

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
        for ( int i=0 ; i < a.len ; i++){
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
    uint32_t max_len;
    if (a.len > b.len)
        max_len = a.len;
    else    
        max_len = b.len;

    uint64_t sum;
    uint64_t carry = 0;
    result.len = max_len;

    for(int i = 0 ; i<max_len; i++){
        sum = carry;

        if ( i < a.len)
            sum+= a.limbs[max_len - 1 -i];
        if (i < b.len)
            sum += b.limbs[max_len - 1 - i];

        result.limbs[max_len - 1 -i] = sum % base_9_number;
        carry = sum /base_9_number ;
    }
    if (carry > 0) // tran sang limbs moi' thi dich toan bo limb tu trai sang phai, de lai limbs[0] = carry
    {
        for (int i = result.len ; i> 0 ; i--)
        {
            result.limbs[i] = result.limbs[i-1];
        }
        result.limbs[0] = carry;
        result.len ++;
    }
    return result;
}
bn_t bn_sub(bn_t a, bn_t b){
    bn_t result;
    int borrow =0;
    uint32_t base = 1000000000;
    if (bn_compare(a, b) != 1) {  // neu a < b thi return
        result.len = 1;
        result.limbs[0] = 0;
        return result;
    }
    int i = a.len -1;
    int j = b.len -1;
    result.len = a.len;
    while( i >= 0){
        int64_t diff = a.limbs[i] - borrow;

        if ( j >= 0)
            diff -= b.limbs[j];

        if (diff < 0 ){
            diff += base;
            borrow = 1;
        }
        else{
            borrow = 0;
        }
        result.limbs[i] = (uint32_t)diff;
        i--;
        j--;
    }
    return result;
}
bn_t bn_mul(bn_t a, bn_t b){
    bn_t result = {0};
    uint32_t base = 1000000000;
    result.len = a.len + b.len;

    for(int i= a.len - 1; i >= 0; i--){ 
        uint64_t carry = 0;
        for (int j = b.len -1 ; j >= 0 ; j--){
            uint64_t temp = (uint64_t)a.limbs[i] * (uint64_t)b.limbs[j] + result.limbs[i+j+1] + carry;

            result.limbs[i + j +1] = temp % base;
            carry = temp/base;
        }
        result.limbs[i] += carry;
    }
    return result ;
}
void bn_divmod_small(bn_t a, uint32_t divisor, bn_t *quotient, uint32_t *remainder){
    uint64_t remain = 0;
    uint32_t base = 1000000000;
    quotient ->len = a.len;

    for (int i = 0; i < a.len ; i++){
        uint64_t temp = remain * base + a.limbs[i];
        quotient ->limbs[i] = temp / divisor;
        remain = temp % divisor;
    }
    *remainder = remain ;

}
int main(){
    char bn[] = "1234567890123" ;
    char bn1[] = "341234567890123" ;
    char string_out[20001];
    uint32_t remainder=0;
    bn_t big_number_1;
    bn_t big_number_2;
    bn_t big_number_3;
    big_number_1=bn_from_string(&bn[0]);
    big_number_2=bn_from_string(&bn1[0]);
    big_number_3 = bn_add(big_number_1,big_number_2);
    bn_to_string(&big_number_3, string_out);
    printf("BIGNUMBER ADD:%s\n", string_out);

    int cp = bn_compare(big_number_1,big_number_2);
    if (cp == 2)
        printf("BN1 == BN 2 \n");
    else if (cp == 1)
        printf("BN1 > BN 2 \n");
    else
        printf("BN1 < BN \n");

    big_number_3 = bn_sub(big_number_2,big_number_1);
    bn_to_string(&big_number_3, string_out);
    printf("BIGNUMBER SUB:%s\n", string_out);

    big_number_3 = bn_mul(big_number_1, big_number_2);
    bn_to_string(&big_number_3, string_out);
    printf("BIGNUMBER MUL:%s\n", string_out);

    bn_divmod_small(big_number_1, 3, &big_number_3, &remainder);
    bn_to_string(&big_number_3, string_out);
    printf("BIGNUMBER DIVMOD:%s\n", string_out);

    printf("%d",cp);
    return 0;
}
