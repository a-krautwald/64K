# 64K Encryption Tool Manual

## NAME
64k - One-time pad encryption and decryption tool

## SYNOPSIS
64k -g <pad_file> 
64k -e <input_file> <pad_file> <output_file> 
64k -d <input_file> <pad_file> <output_file>

## DESCRIPTION
The `64k` program provides secure encryption and decryption using a **one-time pad**. It ensures perfect secrecy when used correctly.

### COMMANDS
- `-g <pad_file>`
  - Generates a 64 KB one-time pad file containing random printable ASCII characters.
- `-e <input_file> <pad_file> <output_file>`
  - Encrypts `input_file` using `pad_file` and writes the encrypted output to `output_file`.
- `-d <input_file> <pad_file> <output_file>`
  - Decrypts `input_file` using `pad_file` and writes the decrypted output to `output_file`.

## USAGE EXAMPLES

### Generate a one-time pad
./64k -g pad.txt

This command creates a 64 KB file `pad.txt` containing random ASCII characters.

### Encrypt a message
./64k -e message.txt pad.txt encrypted.txt

This command encrypts `message.txt` using `pad.txt` and saves the result in `encrypted.txt`.

### Decrypt a message
./64k -d encrypted.txt pad.txt decrypted.txt

This command decrypts `encrypted.txt` using `pad.txt` and saves the result in `decrypted.txt`.

## NOTES
- The pad must be at least as long as the message.
- The same pad must never be reused for multiple encryptions.
- The pad and encrypted message should be securely stored and transmitted.

## EXIT STATUS
- `0` - Success
- `1` - Error (e.g., file issues, invalid arguments)

# SECURE DELETE PLAINTEXT

`shred -u <input_file>`

## AUTHOR
Written by A. Krautwald
