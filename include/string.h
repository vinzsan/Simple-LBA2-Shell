#ifndef STRING_H
#define STRING_H

static short charcmp(char *buffer,char *src){
    for(short i = 0;buffer[i] == src[i];i++){
        if(buffer[i] == '\0'){
            return 0;
        }
    }
    return 1;
}

static char *short2char_arr(short n){
    static char s2a_buffer[32];
    char *p = s2a_buffer + sizeof(s2a_buffer);
    unsigned short x;
    *--p = '\0';
    x = (n < 0) ? (unsigned short)(-n) : (unsigned short)n;
    
    do {
        *--p = '0' + (x % 10);
        x /= 10;
    } while(x);
    if(n < 0){
        *--p = '-';
    }
    return p;
}

#endif