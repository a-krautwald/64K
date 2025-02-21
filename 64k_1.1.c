//64k OTP tool version 1.1 - using open SSL
// to compile:
// gcc -o 64k 64k.c -lssl -lcrypto


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/rand.h>

#define PAD_SIZE 65536  // 64 KB

void generate_random_pad(const char *pad_file, size_t length) {
    FILE *pad = fopen(pad_file, "w");
    if (!pad) {
        fprintf(stderr, "Error opening pad file.\n");
        exit(EXIT_FAILURE);
    }

    unsigned char buffer[length];
    if (RAND_bytes(buffer, length) != 1) {
        fprintf(stderr, "Error generating secure random bytes.\n");
        fclose(pad);
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < length; i++) {
        fputc((buffer[i] % 95) + 32, pad);  // Ensure printable ASCII (32-126)
    }

    fclose(pad);
}

void process_file(const char *input_file, const char *pad_file, const char *output_file, int encrypt) {
    FILE *input = fopen(input_file, "r");
    FILE *pad = fopen(pad_file, "r");
    FILE *output = fopen(output_file, "w");

    if (!input || !pad || !output) {
        fprintf(stderr, "Error opening files.\n");
        exit(EXIT_FAILURE);
    }

    int input_char, pad_char;
    while ((input_char = fgetc(input)) != EOF && (pad_char = fgetc(pad)) != EOF) {
        fputc(encrypt ? (input_char ^ pad_char) : (input_char ^ pad_char), output);
    }

    fclose(input);
    fclose(pad);
    fclose(output);
}

int main(int argc, char *argv[]) {
    if (argc == 3 && strcmp(argv[1], "-g") == 0) {
        generate_random_pad(argv[2], PAD_SIZE);
    } else if (argc == 5 && strcmp(argv[1], "-e") == 0) {
        process_file(argv[2], argv[3], argv[4], 1);
    } else if (argc == 5 && strcmp(argv[1], "-d") == 0) {
        process_file(argv[2], argv[3], argv[4], 0);
    } else {
        fprintf(stderr, "Usage:\n");
        fprintf(stderr, "  %s -g <pad_file>\n", argv[0]);
        fprintf(stderr, "  %s -e <input_file> <pad_file> <output_file>\n", argv[0]);
        fprintf(stderr, "  %s -d <input_file> <pad_file> <output_file>\n", argv[0]);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
