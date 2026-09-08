# Necryption Engine (C++ Research Prototype)

A command-line C++ prototype for **file encryption/decryption** using a custom multi-stage pipeline that combines byte-level transforms, Huffman coding, and 3D Rubik-style data permutation.

> **Important:** This repository is an experimental implementation for learning/research. It is **not** production-grade cryptography.

---

## Table of Contents

- [Overview](#overview)
- [Core Features](#core-features)
- [Project Structure](#project-structure)
- [Requirements](#requirements)
- [Build](#build)
- [Usage](#usage)
- [Encryption/Decryption Flow](#encryptiondecryption-flow)
- [File Format Notes](#file-format-notes)
- [Current Limitations](#current-limitations)
- [Roadmap](#roadmap)
- [Contributing](#contributing)
- [License](#license)

---

## Overview

This project implements a custom encryption workflow around these ideas:

1. fixed-size key normalization (1024 bytes)
2. XOR-based transformation
3. Huffman tree generation and bitstream handling
4. mapping data into a 256×256×256 byte cube
5. dimension-wise cube shifts
6. prime-index shuffle mapping and reverse operations for decryption

The executable automatically selects mode:

- **Encrypt mode** for normal input files
- **Decrypt mode** for files ending in `.khn`

---

## Core Features

- Binary file input/output support
- Automatic file size guard (input > 12 MB is rejected)
- Internal 16 MB processing target via padding
- Automatic output naming:
  - encryption: `<input>.khn`
  - decryption: `decrypted_output.<original_extension>`
- Built-in error handling with terminal messages

---

## Project Structure

```text
.
├── code.cpp       # Main source implementation
├── code           # Compiled executable (if present)
├── input.txt      # Sample plain input
├── input.txt.khn  # Sample encrypted-like file
└── output.bin     # Artifact / test output file
```

---

## Requirements

- Linux/macOS environment (or WSL on Windows)
- C++17 compatible compiler (`g++` recommended)

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

### Encrypt

```bash
./code input.txt my-secret-key
```

Generates:

```text
input.txt.khn
```

### Decrypt

```bash
./code input.txt.khn my-secret-key
```

Generates:

```text
decrypted_output.<ext>
```

---

## Encryption/Decryption Flow

### Encryption Path

1. Read input as bytes
2. Pad to 16 MB
3. Normalize key to 1024 bytes
4. XOR transform
5. Build Huffman tree and encode
6. Load into 3D cube array
7. Apply row/column/drawer shifts
8. Apply prime-based shuffle logic
9. Write encrypted output and metadata

### Decryption Path

1. Parse encrypted file metadata
2. Normalize key
3. Reverse shuffle
4. Reverse cube shifts
5. Huffman decode
6. XOR reverse
7. Recover original metadata + trim padding
8. Write decrypted file

---

## File Format Notes

Encrypted output stores internal metadata (including Huffman-related data) before payload bytes. Decryption relies on this metadata, so format consistency is critical when changing code.

---

## Current Limitations

- Single large source file (`code.cpp`) with limited modular separation
- Experimental algorithm design; not security-audited
- Memory/runtime characteristics are heavy due to 3D-array and mapping approach
- Reliability edge cases still need validation and test coverage

---

## Roadmap

- Refactor into clear modules (`io`, `crypto`, `huffman`, `permutation`, `cli`)
- Add deterministic tests for encrypt/decrypt round-trip
- Define and document stable `.khn` format versioning
- Improve performance and memory safety checks

---

## Contributing

Contributions are welcome for cleanup, testing, and architecture improvements.

Suggested rules:

- keep changes scoped and reviewable
- preserve backward compatibility where possible
- include test evidence for behavior changes

---

## License

No license file is currently included in this repository.
If you plan to open-source this project broadly, add a `LICENSE` file (e.g., MIT/Apache-2.0).
