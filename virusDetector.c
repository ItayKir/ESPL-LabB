#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

FILE* outfile = NULL;

typedef struct virus {
    unsigned short SigSize;
    unsigned char* VirusName;
    unsigned char* Sig;
} virus;

void freeVirus(virus* vir){
    free(vir -> VirusName);
    free(vir -> Sig);
    free(vir);
}

virus* readVirus(FILE* file_ptr){
    virus* vir = malloc(sizeof(virus));
    
    vir->VirusName = NULL;
    vir->Sig = NULL;
    
    // for size of sig
    size_t ret;
    ret = fread(&(vir->SigSize), 1, 2, file_ptr);
    if(ret < 2){
        freeVirus(vir);
        return NULL;
    }

    // for name
    vir -> VirusName = malloc(16);
    ret = fread(vir->VirusName, 1, 16, file_ptr);
    if(ret < 16){
        freeVirus(vir);
        return NULL;
    }
    // for sig
    vir -> Sig = malloc(vir->SigSize);
    ret = fread(vir->Sig, 1, vir->SigSize, file_ptr);
    if(ret < vir->SigSize){
        freeVirus(vir);
        return NULL;
    }
    return vir;
}

void printVirus(virus* virus, FILE* output){
    fprintf(output, "Virus name: %s\n" ,virus->VirusName);
    fprintf(output, "Virus size: %d\n" ,virus ->SigSize);
    fprintf(output, "signature:\n");
    for (int i = 0; i < virus->SigSize; i++){
        fprintf(output, "%02X ", virus->Sig[i]);
    }
    fprintf(output, "\n\n");
}

// returns 0 if Magic Number is valid, else 1
int checkMagicNumber(FILE* file_ptr, unsigned char* buffer){
    size_t magicLength = fread(buffer, 1, 4, file_ptr);
    if(magicLength < 4){
        fprintf(outfile, "ERROR: file shorter than 4 bytes.");
        return 1;
    }
    if(memcmp(buffer, "VIRL", 4)==0 || memcmp(buffer, "VIRB", 4)==0){
        return 0;
    }
    fprintf(outfile, "ERROR: magic number is not VIRL or VIRB");
    return 1;

}

int main(int argc, char** argv){
    outfile = stdout;
    if (argc < 2){
        printf("Provided argument for the file!\n"); 
        return 1;
    }
    FILE* file_ptr= fopen(argv[1], "rb");
    unsigned char magicNumber[4];
    if (!file_ptr) return 1;
    if(checkMagicNumber(file_ptr, magicNumber) == 1){
        fclose(file_ptr);
        return 1;
    }

    while(true){
        virus* vir = readVirus(file_ptr);
        if(vir == NULL){
            break;
        }
        printVirus(vir, outfile);
        freeVirus(vir);
    }
    fclose(file_ptr);
    return 0;
}