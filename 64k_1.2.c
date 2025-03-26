//64k OTP tool version 1.2 - using open SSL
// A secure One-Time Pad implementation with improved cryptographic security
// to compile:
// gcc -o 64k 64k.c -lssl -lcrypto

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/rand.h>  // For cryptographically secure random number generation
#include <openssl/evp.h>   // For cryptographic hash functions
#include <openssl/sha.h>   // For SHA-256 implementation

// Constants for security and performance
#define PAD_SIZE 65536     // 64 KB - Size of the one-time pad
#define BUFFER_SIZE 4096   // 4KB buffer for efficient file operations
#define MIN_PAD_SIZE 1024  // Minimum pad size to ensure security

// Secure memory wiping function to prevent sensitive data from persisting in memory
// Uses volatile pointer to prevent compiler optimization from removing the wiping
static void secure_memzero(void *v, size_t n) {
    volatile unsigned char *p = v;
    while (n--) {
        *p++ = 0;
    }
}

// File integrity verification using SHA-256
// This function can be used to verify that files haven't been tampered with
static int verify_file_integrity(const char *filename, unsigned char *expected_hash) {
    FILE *file = fopen(filename, "rb");  // Open in binary mode for consistent hashing
    if (!file) return 0;

    // Initialize OpenSSL's message digest context
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    const EVP_MD *md = EVP_sha256();
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_len;
    unsigned char buffer[BUFFER_SIZE];
    size_t bytes;

    // Process file in chunks to handle large files efficiently
    EVP_DigestInit_ex(mdctx, md, NULL);
    while ((bytes = fread(buffer, 1, BUFFER_SIZE, file)) != 0) {
        EVP_DigestUpdate(mdctx, buffer, bytes);
    }
    EVP_DigestFinal_ex(mdctx, hash, &hash_len);
    EVP_MD_CTX_free(mdctx);
    fclose(file);

    // Compare computed hash with expected hash
    return (memcmp(hash, expected_hash, hash_len) == 0);
}

// Generate a cryptographically secure random pad
void generate_random_pad(const char *pad_file, size_t length) {
    // Security check: ensure minimum pad size
    if (length < MIN_PAD_SIZE) {
        fprintf(stderr, "Error: Pad size must be at least %d bytes.\n", MIN_PAD_SIZE);
        exit(EXIT_FAILURE);
    }

    // Open pad file in binary write mode for consistent data handling
    FILE *pad = fopen(pad_file, "wb");
    if (!pad) {
        fprintf(stderr, "Error opening pad file.\n");
        exit(EXIT_FAILURE);
    }

    // Allocate buffer for efficient file operations
    unsigned char *buffer = malloc(BUFFER_SIZE);
    if (!buffer) {
        fprintf(stderr, "Error allocating memory.\n");
        fclose(pad);
        exit(EXIT_FAILURE);
    }

    // Generate pad in chunks to handle large sizes efficiently
    size_t remaining = length;
    while (remaining > 0) {
        size_t chunk = (remaining < BUFFER_SIZE) ? remaining : BUFFER_SIZE;
        
        // Generate cryptographically secure random bytes
        if (RAND_bytes(buffer, chunk) != 1) {
            fprintf(stderr, "Error generating secure random bytes.\n");
            secure_memzero(buffer, BUFFER_SIZE);  // Wipe sensitive data
            free(buffer);
            fclose(pad);
            exit(EXIT_FAILURE);
        }

        // Convert random bytes to printable ASCII (32-126)
        for (size_t i = 0; i < chunk; i++) {
            buffer[i] = (buffer[i] % 95) + 32;
        }

        // Write chunk to file with error checking
        if (fwrite(buffer, 1, chunk, pad) != chunk) {
            fprintf(stderr, "Error writing to pad file.\n");
            secure_memzero(buffer, BUFFER_SIZE);  // Wipe sensitive data
            free(buffer);
            fclose(pad);
            exit(EXIT_FAILURE);
        }

        remaining -= chunk;
    }

    // Clean up sensitive data and resources
    secure_memzero(buffer, BUFFER_SIZE);
    free(buffer);
    fclose(pad);
}

// Process file encryption/decryption using the one-time pad
void process_file(const char *input_file, const char *pad_file, const char *output_file, int encrypt) {
    // Open all files in binary mode for consistent data handling
    FILE *input = fopen(input_file, "rb");
    FILE *pad = fopen(pad_file, "rb");
    FILE *output = fopen(output_file, "wb");

    // Comprehensive error handling for file operations
    if (!input || !pad || !output) {
        fprintf(stderr, "Error opening files.\n");
        if (input) fclose(input);
        if (pad) fclose(pad);
        if (output) fclose(output);
        exit(EXIT_FAILURE);
    }

    // Allocate buffers for efficient file processing
    unsigned char *input_buffer = malloc(BUFFER_SIZE);
    unsigned char *pad_buffer = malloc(BUFFER_SIZE);
    if (!input_buffer || !pad_buffer) {
        fprintf(stderr, "Error allocating memory.\n");
        if (input_buffer) free(input_buffer);
        if (pad_buffer) free(pad_buffer);
        fclose(input);
        fclose(pad);
        fclose(output);
        exit(EXIT_FAILURE);
    }

    // Process file in chunks for efficiency and memory safety
    size_t input_size = 0;
    while ((input_size = fread(input_buffer, 1, BUFFER_SIZE, input)) > 0) {
        // Read matching amount of pad data
        size_t pad_size = fread(pad_buffer, 1, input_size, pad);
        if (pad_size != input_size) {
            fprintf(stderr, "Error: Pad file is too short.\n");
            secure_memzero(input_buffer, BUFFER_SIZE);  // Wipe sensitive data
            secure_memzero(pad_buffer, BUFFER_SIZE);    // Wipe sensitive data
            free(input_buffer);
            free(pad_buffer);
            fclose(input);
            fclose(pad);
            fclose(output);
            exit(EXIT_FAILURE);
        }

        // Perform XOR operation for encryption/decryption
        // Note: XOR is symmetric, so same operation works for both
        for (size_t i = 0; i < input_size; i++) {
            input_buffer[i] ^= pad_buffer[i];
        }

        // Write processed data with error checking
        if (fwrite(input_buffer, 1, input_size, output) != input_size) {
            fprintf(stderr, "Error writing to output file.\n");
            secure_memzero(input_buffer, BUFFER_SIZE);  // Wipe sensitive data
            secure_memzero(pad_buffer, BUFFER_SIZE);    // Wipe sensitive data
            free(input_buffer);
            free(pad_buffer);
            fclose(input);
            fclose(pad);
            fclose(output);
            exit(EXIT_FAILURE);
        }
    }

    // Clean up sensitive data and resources
    secure_memzero(input_buffer, BUFFER_SIZE);
    secure_memzero(pad_buffer, BUFFER_SIZE);
    free(input_buffer);
    free(pad_buffer);
    fclose(input);
    fclose(pad);
    fclose(output);
}

// Main function with command-line interface
int main(int argc, char *argv[]) {
    if (argc == 3 && strcmp(argv[1], "-g") == 0) {
        // Generate a new one-time pad
        generate_random_pad(argv[2], PAD_SIZE);
    } else if (argc == 5 && strcmp(argv[1], "-e") == 0) {
        // Encrypt a file using the pad
        process_file(argv[2], argv[3], argv[4], 1);
    } else if (argc == 5 && strcmp(argv[1], "-d") == 0) {
        // Decrypt a file using the pad
        process_file(argv[2], argv[3], argv[4], 0);
    } else {
        // Display usage information
        fprintf(stderr, "Usage:\n");
        fprintf(stderr, "  %s -g <pad_file>\n", argv[0]);
        fprintf(stderr, "  %s -e <input_file> <pad_file> <output_file>\n", argv[0]);
        fprintf(stderr, "  %s -d <input_file> <pad_file> <output_file>\n", argv[0]);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
