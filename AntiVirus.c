#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

FILE* outfile = NULL;
char endian;
char suspected_file[256] = "";


typedef struct virus {
    unsigned short SigSize;
    unsigned char* VirusName;
    unsigned char* Sig;
} virus;

struct link{
    struct link *nextVirus;
    virus *vir;
};






// helper function to free virus data, makes code cleaner
void freeVirus(virus* vir){
    free(vir -> VirusName);
    free(vir -> Sig);
    free(vir);
}

virus* readVirus(FILE* file_ptr){
    virus* vir = malloc(sizeof(virus));
    
    // so freeVirus will not fail if they are not initalized yet
    vir->VirusName = NULL;
    vir->Sig = NULL;
    
    // for size of sig
    size_t ret;
    unsigned char tempSigSize[2];

    ret = fread(tempSigSize, 1, 2, file_ptr);
    if(ret < 2){
        freeVirus(vir);
        return NULL;
    }

    if(endian=='B'){ //Took the bitwise operation from Gemini
        vir->SigSize = (tempSigSize[0] << 8) | tempSigSize[1];
    } else {
        vir->SigSize = tempSigSize[0] | (tempSigSize[1] << 8);
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
    if(memcmp(buffer, "VIRL", 4)==0){
        endian = 'L';
        return 0;
    }
    if(memcmp(buffer, "VIRB", 4)==0){
        endian = 'B';
        return 0;
    }    
    fprintf(outfile, "ERROR: magic number is not VIRL or VIRB");
    return 1;

}


//1b - start
struct link* list_print(struct link* virus_list, FILE* output){
    while(virus_list != NULL){
        printVirus(virus_list->vir, output);
        virus_list = virus_list->nextVirus;
    }
    return NULL;
}


struct link* list_append(struct link* virus_list, virus* data){
    struct link* new_link = malloc(sizeof(struct link));
    new_link -> vir = data;
    new_link -> nextVirus = virus_list;
    return new_link;
}

void list_free(struct link *virus_list){
    if(virus_list == NULL){
        return;
    }
    freeVirus(virus_list -> vir);
    list_free(virus_list -> nextVirus);
    free(virus_list);
}

//1b test phase
//TO DO: check if return is NULL
struct link* loadSignatures(struct link* virus_list){
    if(virus_list != NULL){ // to clean previous list if loading twice
        list_free(virus_list);
        virus_list = NULL;
    }

    printf("Provide a signatures file name:\n");

    char buff[10000];
    char fileName[256];
    fgets(buff, sizeof(buff), stdin);
    sscanf(buff, "%s", fileName);

    if(feof(stdin) || stdin == NULL){
        return NULL;
    }

    FILE* file_ptr= fopen(fileName, "rb");
    unsigned char magicNumber[4];
    if (!file_ptr){
        printf("ERROR: Can't open file - not loading.\n");
        return NULL;
    }
    if(checkMagicNumber(file_ptr, magicNumber) == 1){
        fclose(file_ptr);
        return NULL;
    }

    struct link* viruses = virus_list;

    while(true){
        virus* vir = readVirus(file_ptr);
        if(vir == NULL){
            break;
        }
        viruses = list_append(viruses, vir);
    }
    fclose(file_ptr);
    return viruses;
}

struct link* printSignatures(struct link* virus_list){
    list_print(virus_list, outfile);
    return virus_list;
}

struct link* fileInspect(struct link* virus_list) {
    printf("Enter file name: \n");
    char buff[256];
    fgets(buff, sizeof(buff), stdin);
    sscanf(buff, "%s", suspected_file);
    return virus_list;
}


// helper function, function from main calls this function with size and a buffer
void detect_virus(char *buffer, unsigned int size, struct link *virus_list) {
    for (unsigned int i = 0; i < size; i++) {
        
        struct link* current = virus_list;
        
        while (current != NULL) {
            unsigned short sig_len = current->vir->SigSize;
            if (i + sig_len <= size) {
                if (memcmp(&buffer[i], current->vir->Sig, sig_len) == 0) {
                    printf("Starting byte location: %d\n", i);
                    printf("Virus name: %s\n", current->vir->VirusName);
                    printf("Virus signature size: %d\n\n", sig_len);
                }
            }
            current = current->nextVirus;
        }
    }
}


struct link* detectViruses(struct link* virus_list){
    if (strlen(suspected_file) == 0) {
        printf("ERROR: No file selected.\n");
        return virus_list;
    }

    FILE* file_ptr = fopen(suspected_file, "rb");
    if (!file_ptr) {
        printf("ERROR: Cannot open suspected file.\n");
        return virus_list;
    }

    char buffer[10000];
    
    unsigned int size_read = fread(buffer, 1, 10000, file_ptr);

    detect_virus(buffer, size_read, virus_list);

    fclose(file_ptr);
    return virus_list; 
}

void neutralize_virus(char *fileName, int signatureOffset){
    FILE* file_ptr = fopen(fileName, "r+b");
    fseek(file_ptr, signatureOffset ,SEEK_SET);
    unsigned char ret = 0xC3;
    fwrite(&ret, sizeof(unsigned char), 1, file_ptr);
    fclose(file_ptr);
}

struct link* fixFile(struct link* virus_list){
    char buffer[10000];
    FILE* file_ptr = fopen(suspected_file, "rb");
    unsigned int size_read = fread(buffer, 1, 10000, file_ptr);

    for (unsigned int i = 0; i < size_read; i++) {
        
        struct link* current = virus_list;
        
        while (current != NULL) {
            unsigned short sig_len = current->vir->SigSize;
            if (i + sig_len <= size_read) {
                if (memcmp(&buffer[i], current->vir->Sig, sig_len) == 0) {
                    neutralize_virus(suspected_file, i);
                }
            }
            current = current->nextVirus;
        }
    }
    fclose(file_ptr);
    return virus_list;
}

struct link* quit(struct link* virus_list){
    list_free(virus_list);
    exit(0);
    return NULL;
}




//from lab1
struct fun_desc {
    char *name;
    char index;
    struct link* (*fun)(struct link*);
};

struct fun_desc menu[] = {
    { "<L>oad signatures", 'L', loadSignatures },
    { "<P>rint signatures", 'P', printSignatures },
    { "<S>elect file to inspect", 'S', fileInspect },
    { "<D>etect viruses", 'D', detectViruses },
    { "<F>ix file", 'F', fixFile },
    { "<Q>uit", 'Q', quit },
    { NULL, 0, NULL } // The required null terminator
};

void print_menu(struct fun_desc menu[]){
    int i =0;
    while(menu[i].name != NULL){
        printf("%s\n",menu[i].name);
        i++;
    }

    fputs("Enter the function index to run:\n", outfile);
}

int main(int argc, char** argv){
    //outfile = fopen("lab3_out_compare.txt", "wr");
    outfile = stdout;
    struct link* virus_list = NULL;
    fputs("Select operation from the following menu:\n", outfile);
    while(true){
        print_menu(menu);
        char buff[10000];
        fgets(buff, sizeof(buff), stdin);
        if(feof(stdin) || stdin == NULL){
            break;
        }
        char user_index = buff[0];
        int i=0;

        

        while(menu[i].name != NULL){
            if(menu[i].index == user_index){
                virus_list = menu[i].fun (virus_list);
                break;
            }
            i++;
        }
        if(menu[i].name == NULL){
            fputs("ERROR: function not supported!\n", outfile);
        }
    }
}