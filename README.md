# Private Necryption and Decryption Model

A C++ command-line prototype for file encryption/decryption that combines:

- key normalization (1 KB fixed key)
- XOR-based byte transformation
- Huffman tree construction and bitstream handling
- 3D 256×256×256 Rubik-style array loading/shifting
- prime-driven shuffle mapping logic

> ⚠️ **Project status:** experimental research prototype.

---

## Repository Layout

```text
.
├── code.cpp                  # Main implementation
├── code                      # Compiled binary (if already built)
├── input.txt                 # Sample input
├── input.txt.khn             # Sample encrypted-like file
├── output.bin                # Output artifact
└── assets/
    └── screenshots/
        ├── usage.png
        └── encryption-run.png
```

---

## Build

```bash
g++ -std=c++17 -O2 code.cpp -o code
```

### CLI Contract

```bash
./code <input_file> <key>
```

If `<input_file>` ends with `.khn`, the tool switches to decryption mode. Otherwise, it runs encryption mode.

---

## Screenshots

### Usage Output

![CLI usage screenshot](assets/screenshots/usage.png)

### Encryption Run (sample input)

![Encryption run screenshot](assets/screenshots/encryption-run.png)

---

## Processing Pipeline (High-Level)

### Encryption Path
1. Read file in binary mode (max 12 MB input check).
2. Pad data to fixed 16 MB.
3. Normalize key to 1024 bytes.
4. Apply XOR encryption.
5. Build Huffman tree and encode bitstream.
6. Load data into a 3D Rubik-style byte array.
7. Apply row/column/drawer shifts.
8. Build prime-based shuffle map.
9. Serialize metadata and write output (`<input>.khn`).

### Decryption Path
1. Read encrypted file metadata (bit length + serialized tree).
2. Normalize key to 1024 bytes.
3. Reverse shuffle operations.
4. Reverse Rubik-style shifts.
5. Huffman decode.
6. XOR decrypt.
7. Extract original file metadata and remove padding.
8. Write decrypted output as `decrypted_output.<ext>`.

---

## Example Commands

```bash
# Encrypt
./code input.txt my-secret-key

# Decrypt
./code input.txt.khn my-secret-key
```

---

## Current Constraints

- Single-file implementation (`code.cpp`) with no modular separation.
- Designed around fixed 16 MB internal processing buffer.
- Intended for experimentation and learning, not production cryptography.
- Error handling and format compatibility still require hardening.

---

## Notes for Contributors

- Keep changes small and verifiable.
- Prefer maintaining backward compatibility of the `.khn` format.
- Add/adjust tests before making algorithmic changes.

