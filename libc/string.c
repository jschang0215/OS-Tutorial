#include "string.h"
#include <stdint.h>

// 정수를 문자로 변환 (stdlib의 itoa)
void int_to_ascii(int n, char str[]) {
    int i, sign = n;
    if(sign<0) {
        n = -n;
    }
    i = 0;
    while(n>0) {
        str[i++] = n%10+'0';
        n /= 10;
    }

    if(sign<0) {
        str[i++] = '-';
    }
    str[i] = '\0';
    
    reverse(str);
}

// Hex를 문자로 변환
void hex_to_ascii(int n, char str[]) {
    append(str, '0');
    append(str, 'x');
    char zeros = 0;

    int32_t tmp;
    int i;
    for(i=28; i>0; i-=4) {
        tmp = (n>>i) & 0xF;
        if(tmp==0 && zeros==0) continue;
        zeros = 1;
        if(tmp>0xA) append(str, tmp-0xA+'a');
        else append(str, tmp+'0');
    }

    tmp = n&0xF;
    if(tmp>0xA) {
        append(str, tmp-0xA+'a');
    } else {
        append(str, tmp+'0');
    }
}

void reverse(char str[]) {
    int i, str_len = strlen(str);
    for(i=0; i<str_len/2; i++) {
        char tmp = str[i];
        str[i] = str[str_len-1-i];
        str[str_len-1-i] = tmp;
    }
}

int strlen(char str[]) {
    int i = 0;
    while(str[i]!='\0') {
        i++;
    }
    return i;
}

void backspace(char s[]) {
    int len = strlen(s);
    s[len-1] = '\0';
}

void append(char s[], char n) {
    int len = strlen(s);
    s[len] = n;
    s[len+1] = '\0';
}

// <0: s1<s2, 0: s1==s2, >0: s1>s2
int strcmp(char s1[], char s2[]) {
    int i;
    for(i=0; s1[i]==s2[i]; i++) {
        if(s1[i]=='\0') {
            return 0;
        }
    }
    return s1[i]-s2[i];
}