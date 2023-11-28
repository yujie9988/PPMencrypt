#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <cryptopp890\sha.cpp>



using namespace CryptoPP;

std::string calculateSHA256(const std::string &input) {
    SHA256 hash;
    byte digest[SHA256::DIGESTSIZE];

    hash.CalculateDigest(digest, reinterpret_cast<const byte*>(input.c_str()), input.length());

    std::stringstream ss;
    for (int i = 0; i < SHA256::DIGESTSIZE; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(digest[i]);
    }

    return ss.str();
}

void splitAndWrite(const std::string &sha256Hash) {
    // Assuming you have a file named "input.exe" in the same directory as the executable
    std::ifstream inputFile("PaperMan.exe", std::ios::binary);

    if (!inputFile.is_open()) {
        std::cerr << "Error opening input file." << std::endl;
        return;
    }

    const int chunkSize = sha256Hash.size() / 5;
    std::ofstream outputFiles[5];

    for (int i = 0; i < 5; i++) {
        std::string outputFileName = "output" + std::to_string(i + 1) + ".bin";
        outputFiles[i].open(outputFileName, std::ios::binary);

        if (!outputFiles[i].is_open()) {
            std::cerr << "Error opening output file " << outputFileName << std::endl;
            return;
        }

        outputFiles[i].write(sha256Hash.c_str() + (i * chunkSize), chunkSize);
    }

    char buffer[1024];
    while (!inputFile.eof()) {
        inputFile.read(buffer, sizeof(buffer));
        for (int i = 0; i < 5; i++) {
            outputFiles[i].write(buffer, inputFile.gcount());
        }
    }

    for (int i = 0; i < 5; i++) {
        outputFiles[i].close();
    }

    inputFile.close();
}

int main() {
    // Assuming you have a file named "input.exe" in the same directory as the executablew
    std::ifstream inputFile("PaperMan.exe", std::ios::binary);

    if (!inputFile.is_open()) {
        std::cerr << "Error opening input file." << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << inputFile.rdbuf();
    std::string fileContent = buffer.str();

    std::string sha256Hash = calculateSHA256(fileContent);

    std::cout << "SHA256 Hash: " << sha256Hash << std::endl;

    splitAndWrite(sha256Hash);

    std::cout << "Files created successfully." << std::endl;

    return 0;
}