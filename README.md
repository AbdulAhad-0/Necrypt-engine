# NeCrypt Engine (C++ File Encryption/Decryption Prototype)

[![Language](https://img.shields.io/badge/language-C%2B%2B17-blue.svg)](#tech-stack)
[![Status](https://img.shields.io/badge/status-research-orange.svg)](#security-notice)
[![Platform](https://img.shields.io/badge/platform-linux%20%7C%20macOS-lightgrey.svg)](#requirements)

A command-line C++ prototype that explores a custom, multi-stage file encryption/decryption pipeline using XOR transforms, Huffman encoding, and 3D data permutation logic.

---

## Repository Naming Recommendation

To make this project more professional and searchable on GitHub, rename the repository from:

- `private-necryption-and-decryption-model`

to one of the following:

- `necrypt-engine-cpp` (recommended)
- `necryption-cpp`
- `file-crypto-prototype-cpp`

> You can rename it from **GitHub Repository → Settings → General → Repository name**.

---

## Table of Contents

- [Project Overview](#project-overview)
- [Tech Stack](#tech-stack)
- [How It Works](#how-it-works)
- [Features](#features)
- [Project Structure](#project-structure)
- [Requirements](#requirements)
- [Build](#build)
- [Usage](#usage)
- [Output & File Format](#output--file-format)
- [Security Notice](#security-notice)
- [Current Limitations](#current-limitations)
- [Roadmap](#roadmap)
- [Contributing](#contributing)
- [License](#license)

---

## Project Overview

This project implements a reversible encryption/decryption workflow for files and automatically selects operation mode based on input extension:

- **Encrypt mode**: regular file input
- **Decrypt mode**: files ending with `.khn`

Core algorithm stages include:

1. Key normalization to fixed length (1024 bytes)
2. XOR byte transformation
3. Huffman tree generation and bitstream encoding
4. Mapping to a 256 × 256 × 256 byte cube
5. Multi-axis cube shifts
6. Prime-based index shuffling and reverse mapping

---

## Tech Stack

### Language & Build
- **C++17**
- **g++** (recommended compiler)

### Standard Library Components Used
- `vector`, `string`, `unordered_map`, `queue`, `bitset`
- file I/O (`fstream`, iterators)
- utility modules (`algorithm`, `chrono`, `stdexcept`, `functional`)

### Algorithmic Building Blocks
- XOR transformation with normalized key
- Huffman compression/decompression
- 3D cube-based data permutation
- Prime-driven deterministic shuffling

---

## How It Works

### Encryption Flow
1. Read input bytes
2. Pad data to internal working size
3. Normalize key
4. Apply XOR transform
5. Build Huffman tree and encode to bitstream
6. Load data into 3D cube representation
7. Apply axis shifts and permutation
8. Apply prime-based shuffle
9. Write encrypted output with metadata

### Decryption Flow
1. Read encrypted file metadata + payload
2. Normalize key
3. Reverse prime-based shuffle
4. Reverse cube permutations
5. Huffman decode
6. Reverse XOR transform
7. Remove internal padding and restore original content
8. Write decrypted output

---

## Features

- Binary-safe file processing
- Encryption and decryption with one executable
- Extension-based mode detection (`.khn`)
- Automatic output naming:
  - encryption: `<input>.khn`
  - decryption: `decrypted_output.<original_extension>`
- Guardrails for unsupported/oversized inputs
- CLI status and error reporting

---

## Project Structure

```text
.
├── README.md      # Project documentation
├── code.cpp       # Main C++ implementation
├── code           # Compiled executable (if present)
├── assets/        # Project assets
├── input.txt      # Example plaintext input
├── input.txt.khn  # Example encrypted-like file
└── output.bin     # Generated output artifact
```

---

## Requirements

- Linux/macOS (or WSL on Windows)
- C++17-compatible compiler (`g++` preferred)

---

## Build

```bash
g++ -std=c++17 -O2 code.cpp -o code
```

---

## Usage

```bash
./code <input_file> <key>
```

### Encrypt a File

```bash
./code input.txt my-secret-key
```

Output:
```text
input.txt.khn
```

### Decrypt a File

```bash
./code input.txt.khn my-secret-key
```

Output:
```text
decrypted_output.<ext>
```

---

## Output & File Format

Encrypted files contain metadata headers required for successful decryption (including Huffman-related information and internal state needed by reverse transformations).  
If serialization format changes, backward compatibility for `.khn` files can break.

---

## Security Notice

This repository is a **research/learning prototype**, not audited production cryptography.

- Do **not** use this system for sensitive or regulated data
- Do **not** assume resistance to modern cryptographic attacks
- Treat this project as an educational implementation

---

## Current Limitations

- Implementation is concentrated in a single large source file (`code.cpp`)
- Limited modular architecture and test coverage
- High memory/runtime cost due to 3D permutation model
- No formal cryptographic audit

---

## Roadmap

- Split into modules (`io`, `crypto`, `huffman`, `permutation`, `cli`)
- Add deterministic round-trip test suite
- Version and document `.khn` format explicitly
- Improve performance and memory safety checks

---

## Contributing

Contributions are welcome, especially for:

- modular refactoring
- correctness testing
- performance optimization
- documentation quality

Please keep changes scoped, explain behavior impact clearly, and include verification notes.

---

## License

No license file is currently included.  
If you plan to publish broadly, add a license such as MIT or Apache-2.0.
