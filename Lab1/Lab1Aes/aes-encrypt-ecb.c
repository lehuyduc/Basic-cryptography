#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aes.h"

typedef unsigned char byte;

extern void aes_encrypt(byte* cipher, byte* message, byte* key);
extern void aes_decrypt(byte* message, byte* cipher, byte* key);

void display(byte* state, char *label);

int main(int argc, char* argv[])
{
    byte ciphertext_block[16];
    byte key[16] = {0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C};
    byte plaintext_block[16];

    char name_ciphertext[100];
    char name_plaintext[100];

    FILE* pfd_ciphertext = NULL;
    FILE* pfd_plaintext = NULL;

    unsigned int nb_byte;

    /*
    if (argc != 3)
    {
    printf("Usage: %s plaintext_file ciphertext_file\n", argv[0]);
    exit(-1);
    }

    sprintf(name_plaintext, "%s", argv[1]);
    sprintf(name_ciphertext, "%s", argv[2]);
    */
    strcpy(name_plaintext, "logo.png");
    strcpy(name_ciphertext, "logo_myencrypt.txt");
    //strcpy(name_plaintext, "message.txt");
    //strcpy(name_ciphertext, "message_myencrypt.txt");

    printf("name_plaintext : %s\n", name_plaintext);
    printf("name_ciphertext : %s\n", name_ciphertext);

    if ((pfd_plaintext = fopen(name_plaintext, "rb")) == NULL)
    {
    printf("ERROR: can not open file \"%s\" for reading\n", name_plaintext);
    exit(-1);
    }

    if ((pfd_ciphertext = fopen(name_ciphertext, "wb")) == NULL)
    {
    printf("ERROR: can not open file \"%s\" for writing\n", name_ciphertext);
    exit(-1);
    }

  // complete here
    while ((nb_byte = fread(plaintext_block, sizeof(char), 16, pfd_plaintext)) == 16)
    {
        aes_encrypt(ciphertext_block, plaintext_block, key);
        fwrite(ciphertext_block, sizeof(char), 16, pfd_ciphertext);
    }

    memset(plaintext_block + nb_byte, 16 - nb_byte, 16 - nb_byte);
    aes_encrypt(ciphertext_block, plaintext_block, key);
    fwrite(ciphertext_block, sizeof(char), 16, pfd_ciphertext);

    fclose(pfd_plaintext);
    fclose(pfd_ciphertext);

  return 0;
}
