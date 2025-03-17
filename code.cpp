// =================== Khan-Rubix Private Key Encryption =================== //
// This implementation follows the 12-step encryption process with:
// 1. Padding to 16MB
// 2. XOR Encryption with Key
// 3. Huffman Encoding (with Tree embedded in header)
// 4. Loading into 256x256x256 Rubik's Cube Array
// 5. Placeholder for Row, Column, and Drawer Shifting
// 6. Placeholder for Prime-based Shuffling
#include <queue>
#include <bitset>
#include <sstream>
#include <random>
#include <functional>
#include <stdexcept>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include<iterator>
#include <algorithm>
#include <random>
#include <functional>
#include <ctime>
#include <chrono>
#include <iomanip>

using namespace std;

// =================== CONSTANTS =================== //
const size_t MAX_SIZE = 12 * 1024 * 1024; // 12MB
const size_t key_size = 1024; // 1KB
const size_t file_size = 16 * 1024 * 1024; // 16MB

// Node structure for Huffman Tree
struct Node {
    unsigned char data;
    unsigned freq;
    Node *left, *right;
    Node(unsigned char data, unsigned freq) : data(data), freq(freq), left(nullptr), right(nullptr) {}
};

vector<unsigned char> readfile(const string& filename);
void writefile(const string& filename, const vector<unsigned char>& data, size_t bitLength, Node* root);

// Comparison for Min-Heap
struct compare {
    bool operator()(Node* l, Node* r) { return (l->freq > r->freq); }
};

// ==================== Huffman Encoding ==================== //
void buildHuffmanTree(const vector<unsigned char>& data, unordered_map<unsigned char, string>& huffmanCodes, Node*& root) {
    unordered_map<unsigned char, unsigned> freq;

    // Initialize frequencies for all possible characters (0-255)
    for (int i = 0; i < 256; i++) {
        freq[static_cast<unsigned char>(i)] = 0;
    }

    // Count actual character frequencies
    for (unsigned char ch : data) freq[ch]++;

    // Build Huffman Tree only for characters with frequency > 0
    priority_queue<Node*, vector<Node*>, compare> minHeap;
    for (auto& pair : freq) {
        if (pair.second > 0) {
            minHeap.push(new Node(pair.first, pair.second));
        }
    }

    // Build Huffman Tree
    while (minHeap.size() > 1) {
        Node* left = minHeap.top(); minHeap.pop();
        Node* right = minHeap.top(); minHeap.pop();
        Node* parent = new Node('\0', left->freq + right->freq);
        parent->left = left;
        parent->right = right;
        minHeap.push(parent);
    }

    root = minHeap.top();

    // Generate Huffman Codes
    function<void(Node*, string)> encode = [&](Node* node, string str) {
        if (!node) return;
        if (node->data != '\0') huffmanCodes[node->data] = str;
        encode(node->left, str + "0");
        encode(node->right, str + "1");
    };
    encode(root, "");
}

// ==================== Huffman Encoding (Bitwise) ==================== //
vector<unsigned char> huffmanEncode(const vector<unsigned char>& data, const unordered_map<unsigned char, string>& huffmanCodes, size_t& bitLength) {
    string encodedStr;
    int i=0;
    for (unsigned char ch : data) {
        auto it = huffmanCodes.find(ch);
        if (it != huffmanCodes.end()) {  // Check if character exists
            encodedStr += it->second;
        } else {
            throw runtime_error("Error: Character '" + to_string(ch) + "' missing in Huffman Codes!");
        }
    }

    bitLength = encodedStr.size(); // Assign bit length
    cout << "Bit Length: " << bitLength << endl;
    cout << "Encoded String{1st 50 bits only}: " << endl;
    while(i<50){
    cout << " " << encodedStr[i] ;
    i++;}
    vector<unsigned char> encodedData((bitLength + 7) / 8, 0);
    for (size_t i = 0; i < bitLength; ++i) {
        if (encodedStr[i] == '1') {
            encodedData[i / 8] |= (1 << (7 - (i % 8)));
        }
    }

    return encodedData;
}

// ==================== Huffman Decoding (Bitwise) ==================== //
vector<unsigned char> huffmanDecode(const vector<unsigned char>& encodedData, Node* root, size_t bitLength) {
    vector<unsigned char> decodedData;
    Node* current = root;
    
    for (size_t i = 0; i < bitLength; ++i) {
        bool bit = (encodedData[i / 8] >> (7 - (i % 8))) & 1;
        current = bit ? current->right : current->left;

        if (!current->left && !current->right) {
            decodedData.push_back(current->data);
            current = root;
        }
    }
    return decodedData;
}

// ==================== Serialize Huffman Tree ==================== //
void serializeTree(Node* root, ofstream& out) {
    if (!root) return;
    if (root->data != '\0') {
        out.put('L');
        out.put(root->data);
    } else {
        out.put('I');
    }
    serializeTree(root->left, out);
    serializeTree(root->right, out);
}

// ==================== Deserialize Huffman Tree ==================== //
Node* deserializeTree(ifstream& in) {
    char type;
    if (!in.get(type)) return nullptr;
    
    if (type == 'L') {
        unsigned char data;
        in.get(reinterpret_cast<char&>(data));
        return new Node(data, 0);
    }
    
    Node* node = new Node('\0', 0);
    node->left = deserializeTree(in);
    node->right = deserializeTree(in);
    return node;
}

// ==================== File Handling ==================== //
void writefile(const string& filename, const vector<unsigned char>& data, size_t bitLength, Node* root) {
    ofstream out(filename, ios::binary);
    if (!out) throw runtime_error("Error writing file!");
    out.write(reinterpret_cast<const char*>(&bitLength), sizeof(bitLength));
    serializeTree(root, out);
    out.write(reinterpret_cast<const char*>(data.data()), data.size());
    cout << "File written successfully: " << filename << endl;
    out.close();
}

vector<unsigned char> readfile(const string& filename) {
    ifstream in(filename, ios::binary | ios::ate); // Open in binary mode and seek to end
    if (!in) throw runtime_error("Error opening file!");
    
    streamsize filesize = in.tellg(); // Get file size in bytes
    in.seekg(0, ios::beg); // Move pointer back to beginning

    if (filesize > MAX_SIZE) {
        throw runtime_error("File size exceeds 12 MB limit!");
    }

    return vector<unsigned char>((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
}

// =================== KEY NORMALIZATION (1KB) =================== //
vector<unsigned char> normalizeKey(const string& key) {
    vector<unsigned char> keyVec(key.begin(), key.end());
    if (keyVec.size() < key_size) {
        // Pad with zeros if smaller
        keyVec.resize(key_size, 0);
    } else if (keyVec.size() > key_size) {
        // Truncate if larger
        keyVec.resize(key_size);
    }
    cout << "Key normalized to size: " << keyVec.size() << " bytes" << endl;
    return keyVec;
}

// =================== Padding to 16MB =================== //
void padTo16MB(vector<unsigned char>& data) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(0, data.size() - 1);
    while (data.size() < file_size) {
        data.push_back(data[dist(gen)] ^ dist(gen));
    }
    cout << "Data padded to size: " << data.size() << " bytes" << endl;
    }

// =================== XOR Encryption =================== //
void xorEncrypt(vector<unsigned char>& data, const vector<unsigned char>& key) {
    if (data.empty() || key.empty()) {
        throw runtime_error("Data or key cannot be empty for encryption.");
    }
    for (size_t i = 0; i < data.size(); i++) {
        data[i] ^= key[i % key_size];
    }
}

// =================== Load into Rubik's Array =================== //
unsigned char rubikArray[256][256][256];

// =================== Rubik's Cube Shifting =================== //
void primeBasedShuffling(const vector<unsigned char>& key) {
    vector<int> primes = {1009, 1013, 1019, 1021, 1031, 1033, 1039, 1049, 1051, 1061, 1063, 1069, 1087, 1091, 1093, 1097};
    int prime = primes[key[59] % primes.size()];
    
    vector<int> shuffleMap(16777216, -1); // Initialize all to -1
    cout << "Starting shuffle map creation..." << endl;

    for (int i = 0; i < 16777216; i++) {
        // Prevent integer overflow
        long long tempIndex = static_cast<long long>(i) * prime;
        int index = tempIndex % 16777216;

        // Linear probing with safety check
        int probeCount = 0;
        while (shuffleMap[index] != -1) {
            index = (index + 1) % 16777216;  // Linear probing
            probeCount++;
            if (probeCount > 16777216) {  // Safety check
                throw runtime_error("Error: Linear probing exceeded maximum iterations!");
            }
        }

        shuffleMap[index] = i;

        if (i % 1000000 == 0) { // Log progress every 1 million iterations
            cout << "Progress: " << i << " entries added to shuffle map." << endl;
        }
    }

    cout << "Shuffle map creation completed." << endl;

    vector<unsigned char> shuffledData(16777216);
    int idx = 0;
    for (int x = 0; x < 256; x++) {
        for (int y = 0; y < 256; y++) {
            for (int z = 0; z < 256; z++) {
                shuffledData[shuffleMap[idx++]] = rubikArray[x][y][z];
            }
        }
    }
    cout << "Prime-based shuffling completed successfully." << endl;
}

void shiftRubikArray(const vector<unsigned char>& key) {
    // Row Shifting (X-Dimension)
    for (int x = 0; x < 256; x++) {
        int shiftAmount = key[x] & 0b11;  // Last 2 bits
        for (int y = 0; y < 256; y++) {
            unsigned char temp[256];
            for (int z = 0; z < 256; z++) {
                temp[(z + shiftAmount) % 256] = rubikArray[x][y][z];
            }
            for (int z = 0; z < 256; z++) {
                rubikArray[x][y][z] = temp[z];
            }
        }
    }
    cout << "Row Shifting Completed!" << endl;

    // Column Shifting (Y-Dimension)
    for (int y = 0; y < 256; y++) {
        int shiftAmount = key[y] & 0b11;  // Last 2 bits
        for (int x = 0; x < 256; x++) {
            unsigned char temp[256];
            for (int z = 0; z < 256; z++) {
                temp[(z + shiftAmount) % 256] = rubikArray[x][y][z];
            }
            for (int z = 0; z < 256; z++) {
                rubikArray[x][y][z] = temp[z];
            }
        }
    }
    cout << "Column Shifting Completed!" << endl;

    // Drawer Shifting (Z-Dimension)
    for (int z = 0; z < 256; z++) {
        int shiftAmount = key[z] & 0b11;  // Last 2 bits
        for (int x = 0; x < 256; x++) {
            unsigned char temp[256];
            for (int y = 0; y < 256; y++) {
                temp[(y + shiftAmount) % 256] = rubikArray[x][y][z];
            }
            for (int y = 0; y < 256; y++) {
                rubikArray[x][y][z] = temp[y];
            }
        }
    }
    cout << "Drawer Shifting Completed!" << endl;
    cout << "Displaying a 4x4x4 section of the 3D array:\n";
    for (int i = 0; i < 4; i++) {
        cout << "\nLayer " << i << ":\n";
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 4; k++) {
                cout << rubikArray[i][j][k] << " ";
            }
            cout << endl;
        }
    }
    

    cout << "Shifting Completed!" << endl;
}

void loadIntoRubikArray(const vector<unsigned char>& data) {
    if (data.size() != file_size) {
        throw runtime_error("Data size mismatch: Expected 16MB after padding.");
    }
    
    size_t index = 0;
    for (int x = 0; x < 256; x++) {
        
        for (int y = 0; y < 256; y++) {
            for (int z = 0; z < 256; z++) {
                rubikArray[x][y][z] = data[index++];
                
            }
        }       
    }
    cout << "Rubik's Cube array loaded successfully." << endl;
}
vector<unsigned char> readEncryptedFile(const string& filename, size_t& bitLength, Node*& root) {
    ifstream in(filename, ios::binary);
    if (!in) throw runtime_error("Error opening encrypted file!");

    in.read(reinterpret_cast<char*>(&bitLength), sizeof(bitLength));
    root = deserializeTree(in);

    vector<unsigned char> data((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
    in.close();
    return data;
}

// ========== Reverse Shuffle ==========
void reversePrimeShuffling(vector<unsigned char>& shuffledData, const vector<unsigned char>& key) {
    vector<int> primes = {1009, 1013, 1019, 1021, 1031, 1033, 1039, 1049, 1051, 1061, 1063, 1069, 1087, 1091, 1093, 1097};
    int prime = primes[key[59] % primes.size()];
    
    vector<int> shuffleMap(16777216, -1);
    
    for (int i = 0; i < 16777216; i++) {
        long long tempIndex = static_cast<long long>(i) * prime;
        int index = tempIndex % 16777216;

        while (shuffleMap[index] != -1) {
            index = (index + 1) % 16777216;
        }
        shuffleMap[index] = i;
    }

    int idx = 0;
    for (int x = 0; x < 256; x++) {
        for (int y = 0; y < 256; y++) {
            for (int z = 0; z < 256; z++) {
                rubikArray[x][y][z] = shuffledData[shuffleMap[idx++]];
            }
        }
    }
    cout << "Reverse shuffling completed!" << endl;
}

// ========== Reverse Rubik's Cube Shifting ==========
void reverseShiftRubikArray(const vector<unsigned char>& key) {
    // Reverse Z-Dimension (Drawer Shift)
    for (int z = 0; z < 256; z++) {
        int shiftAmount = key[z] & 0b11;
        for (int x = 0; x < 256; x++) {
            unsigned char temp[256];
            for (int y = 0; y < 256; y++) temp[y] = rubikArray[x][(y - shiftAmount + 256) % 256][z];
            for (int y = 0; y < 256; y++) rubikArray[x][y][z] = temp[y];
        }
    }

    // Reverse Y-Dimension (Column Shift)
    for (int y = 0; y < 256; y++) {
        int shiftAmount = key[y] & 0b11;
        for (int x = 0; x < 256; x++) {
            unsigned char temp[256];
            for (int z = 0; z < 256; z++) temp[z] = rubikArray[x][y][(z - shiftAmount + 256) % 256];
            for (int z = 0; z < 256; z++) rubikArray[x][y][z] = temp[z];
        }
    }

    // Reverse X-Dimension (Row Shift)
    for (int x = 0; x < 256; x++) {
        int shiftAmount = key[x] & 0b11;
        for (int y = 0; y < 256; y++) {
            unsigned char temp[256];
            for (int z = 0; z < 256; z++) temp[z] = rubikArray[(x - shiftAmount + 256) % 256][y][z];
            for (int z = 0; z < 256; z++) rubikArray[x][y][z] = temp[z];
        }
    }
    cout << "Reverse shifting completed!" << endl;
}
// ========== XOR Decryption ==========
void xorDecrypt(vector<unsigned char>& data, const vector<unsigned char>& key) {
    for (size_t i = 0; i < data.size(); i++) {
        data[i] ^= key[i % key_size];
    }
}

// ========== Extract File Info ==========
void extractFileInfo(vector<unsigned char>& data, string& fileSuffix, size_t& originalSize) {
    originalSize = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];

    fileSuffix.clear();
    size_t maxSuffixLength = 255;
    for (size_t i = 4; i < data.size() && data[i] != '\0' && i < 4 + maxSuffixLength; i++) {
        fileSuffix += data[i];
    }

    if (fileSuffix.empty()) {
        throw runtime_error("Error: File suffix is missing or invalid!");
    }

    cout << "Extracted file suffix: " << fileSuffix << endl;

    size_t eraseLength = fileSuffix.size() + 5;
    if (eraseLength > data.size()) {
        throw runtime_error("Error: Invalid file structure, cannot erase beyond data size!");
    }

    data.erase(data.begin(), data.begin() + eraseLength);
}

// ========== Pad to 16MB ==========
void removePadding(vector<unsigned char>& data, size_t originalSize) {
    if (data.size() > originalSize) {
        data.resize(originalSize);  // Extra bytes remove karo
    }
}


// ========== Write Decrypted File ==========
void writeDecryptedFile(const string& filename, const vector<unsigned char>& data) {
    ofstream out(filename, ios::binary);
    if (!out) throw runtime_error("Error writing decrypted file!");
    out.write(reinterpret_cast<const char*>(data.data()), data.size());
    out.close();
    cout << "Decryption completed! File saved as: " << filename << endl;
}

void appendFileInfo(vector<unsigned char>& data, const string& fileSuffix, size_t originalSize) {
    // Add the original size (4 bytes)
    data.insert(data.begin(), (originalSize >> 24) & 0xFF);
    data.insert(data.begin() + 1, (originalSize >> 16) & 0xFF);
    data.insert(data.begin() + 2, (originalSize >> 8) & 0xFF);
    data.insert(data.begin() + 3, originalSize & 0xFF);

    // Add the file suffix (null-terminated)
    size_t maxSuffixLength = 255; // Limit the suffix length
    string truncatedSuffix = fileSuffix.substr(0, maxSuffixLength); // Truncate if necessary
    data.insert(data.begin() + 4, truncatedSuffix.begin(), truncatedSuffix.end());
    data.insert(data.begin() + 4 + truncatedSuffix.size(), '\0'); // Null-terminate
}

void encryption_file(const string& inputFilename, const string& outputFilename, const string& keyStr) {
    vector<unsigned char> fileData = readfile(inputFilename);
    cout << "Step 1: File read successfully. Size: " << fileData.size() << " bytes" << endl;

    // Step 1: Padding
    padTo16MB(fileData);
    cout << "Step 2: Padding completed. Size: " << fileData.size() << " bytes" << endl;

    // Step 2: XOR Encryption
    vector<unsigned char> key = normalizeKey(keyStr);
    xorEncrypt(fileData, key);
    cout << "Step 3: XOR encryption completed." << endl;

    // Step 3: Huffman Encoding
    unordered_map<unsigned char, string> huffmanCodes;
    Node* root = nullptr;
    buildHuffmanTree(fileData, huffmanCodes, root);
    cout << "Step 4: Huffman tree built successfully." << endl;

    size_t bitLength;
    vector<unsigned char> encodedData = huffmanEncode(fileData, huffmanCodes, bitLength);
    cout << "Step 5: Huffman encoding completed. Bit length: " << bitLength << endl;

    // Serialize Huffman Tree
    ofstream outFile(outputFilename, ios::binary);
    if (!outFile) throw runtime_error("Error opening output file!");
    cout << "Step 6: Output file opened successfully." << endl;

    // Step 4: Load into Rubik's Cube Array
    loadIntoRubikArray(fileData);
    cout << "Step 7: Rubik's Cube array loaded successfully." << endl;

    // Step 5: Perform Shifting
    shiftRubikArray(key);
    cout << "Step 8: Rubik's Cube shifting completed." << endl;

    // Step 6: Perform Prime-Based Shuffling
    primeBasedShuffling(key);
    cout << "Step 9: Prime-based shuffling completed." << endl;

    // Write to output file
    outFile.write(reinterpret_cast<const char*>(&bitLength), sizeof(bitLength));
    serializeTree(root, outFile);
    outFile.write(reinterpret_cast<const char*>(fileData.data()), fileData.size());
    outFile.close();
    cout << "Step 10: Data written to output file successfully." << endl;

    cout << "Encryption completed!" << endl;
}
void decryptFile(const string& inputFilename, const string& keyStr) {
    size_t bitLength;
    Node* root = nullptr;

    vector<unsigned char> encryptedData = readEncryptedFile(inputFilename, bitLength, root);
    vector<unsigned char> key = normalizeKey(keyStr);

    reversePrimeShuffling(encryptedData, key);
    reverseShiftRubikArray(key);
    //print
    cout<<"done 1"<<endl;


    vector<unsigned char> decodedData = huffmanDecode(encryptedData, root, bitLength);
    xorDecrypt(decodedData, key);
    string fileSuffix;
    size_t originalSize;
    extractFileInfo(decodedData, fileSuffix, originalSize);
    cout<<"file suffix: "<<fileSuffix<<endl;
    cout<<"done 3"<<endl;
    cout<<"before padding decoded data: "<<decodedData.size()<<endl;
    removePadding(decodedData, originalSize);  // 🛑 Remove extra padding
    cout<<"after padding decoded data: "<<decodedData.size()<<endl;
    cout<<"done 4"<<endl;
    string outputFilename = "decrypted_output." + fileSuffix;
    //cout << "Decrypted output file saved as: " << outputFilename << endl;

    // Write decrypted file to disk
    writeDecryptedFile(outputFilename, decodedData);
}


int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "❌ Usage: " << argv[0] << " <input_file> <key>" << endl;
        return 1;
    }

    string inputFilename = argv[1];
    string keyStr = argv[2];

    try {
        if (inputFilename.size() >= 4 && inputFilename.substr(inputFilename.size() - 4) == ".khn") {
            // 🛑 **DECRYPTION MODE**
            cout << "🔓 Decryption Mode Detected!\n";
            decryptFile(inputFilename, keyStr);
        } else {
            // 🔒 **ENCRYPTION MODE**
            cout << "🔒 Encryption Mode Detected!\n";
            string outputFilename = inputFilename + ".khn";  // Auto-generate encrypted filename
            encryption_file(inputFilename, outputFilename, keyStr);
        }
    } catch (const exception& ex) {
        cerr << "❌ Error: " << ex.what() << endl;
        return 1;
    }
    return 0;
}
