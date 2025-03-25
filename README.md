# 64K OTP Tool

A secure One-Time Pad (OTP) implementation in C using OpenSSL for cryptographic operations.

## Features

- Cryptographically secure random number generation using OpenSSL
- Efficient file processing with buffer-based operations
- Secure memory handling with explicit memory wiping
- File integrity verification using SHA-256
- Binary mode file operations for consistent data handling
- Comprehensive error handling and resource cleanup

## Security Features

- Minimum pad size requirement (1024 bytes)
- Secure memory wiping to prevent data persistence
- Binary mode file operations to prevent text mode issues
- Proper resource cleanup in all error cases
- Buffer overflow protection
- File integrity verification capability

## Requirements

- C compiler (gcc recommended)
- OpenSSL development libraries
- POSIX-compliant operating system

## Installation

1. Install OpenSSL development libraries:
   - On Ubuntu/Debian: `sudo apt-get install libssl-dev`
   - On macOS: `brew install openssl`
   - On Windows: Download from [OpenSSL website](https://www.openssl.org/source/)

2. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/64k-otp.git
   cd 64k-otp
   ```

3. Compile the program:
   ```bash
   gcc -o 64k 64k.c -lssl -lcrypto
   ```

## Usage

### Generate a new one-time pad
```bash
./64k -g <pad_file>
```

### Encrypt a file
```bash
./64k -e <input_file> <pad_file> <output_file>
```

### Decrypt a file
```bash
./64k -d <input_file> <pad_file> <output_file>
```

## Security Considerations

1. Never reuse a one-time pad
2. Store the pad file securely
3. Use a secure method to transfer the pad file to the recipient
4. Delete the pad file after use
5. Implement proper access controls on the pad file

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## Disclaimer

This tool is provided for educational and research purposes only. Users are responsible for ensuring proper security practices when using this tool.
