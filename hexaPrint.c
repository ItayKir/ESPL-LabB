#include <stdio.h>
#include <stdlib.h>


void printHex(unsigned char buffer[], size_t length){
    for(size_t i =0; i< length; i++){
        printf("%02X ", buffer[i]);
    }
    printf("\n");
}

int main(int argc,char **argv){
    FILE* file_ptr= fopen(argv[1], "rb");
    unsigned char buffer[16];
    size_t total_bytes = fread(buffer, sizeof(unsigned char), 16, file_ptr);
    printHex(buffer, total_bytes);
    return 0;
}