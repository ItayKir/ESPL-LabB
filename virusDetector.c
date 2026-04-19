#include <stdio.h>
#include <stdlib.h>

typedef struct virus {
    unsigned short SigSize;
    unsigned char* VirusName;
    unsigned char* Sig;
} virus;

void printHex(unsigned char buffer[], size_t length){
    for(size_t i =0; i< length; i++){
        printf("%02X ", buffer[i]);
    }
    printf("\n");
}

virus* readVirus(FILE* file_ptr){
    virus* vir = malloc(sizeof(virus));

    // for size of sig
    fread(&(v->SigSize), 1, 2, file_ptr);

    // for name
    v -> VirusName = malloc(16);
    fread(v->VirusName, 1, 16, file_ptr);

    // for sig
    v -> Sig = malloc(v->SigSize);
    fread(v->Sig, 1, v->SigSize, file_ptr);

    return v;
}

void printVirus(virus* virus, FILE* output){
    fprintf(output, "Virus name: %s\n" ,virus->VirusName);
    fprintf(output, "Virus size: %d\n" ,virus ->SigSize);
    fprintf(output, "signature:\n");
    for (int i = 0; i < virus->SigSize; i++){
        fprintf(output, "%02X ", virus->Sig[i]);
    }
}


int main(int argc, char** argv){

}