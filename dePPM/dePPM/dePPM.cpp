#include "cryptlib.h"
#include "rijndael.h"
#include "modes.h"
#include "files.h"
#include "osrng.h"
#include "hex.h"
#include "sha.h"
#include "hex.h"
#include "dirent.h"

#include <iostream>
#include <string>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <vector>
#include <sstream>
#include <filesystem>

using namespace CryptoPP;
namespace fs = std::filesystem;


int main(int argc, char* argv[])
{
    const char* folderPath = "data";

    const std::vector<std::string> HashCode = { "E557","EC9C","7AFC","2206","A4FD" };

    byte iv[32] = "0123456789ABCDEF0123456789ABCDE";
    byte key[32] = "0123456789ABCDEF0123456789ABC0A";


    std::vector<std::string> fileNames;
    std::string output_exe = "merge.exe";
    std::ostringstream contentBuffer;
    std::string contentBuffer0;
    std::string contentBuffer1;
    std::string contentBuffer2;
    std::string contentBuffer3;
    std::string contentBuffer4;

    HexEncoder encoder(new FileSink(std::cout));
    std::string digest;
    SHA256 hash;



    std::vector<fs::path> OrderedFile;


    int ord[5];
    int order = 0;

    std::string filepath;

    for (const auto& entry : fs::recursive_directory_iterator("data")) {
        
        filepath = "";
        filepath = entry.path().string();

        std::ifstream file(filepath, std::ios::binary);

        if (!file.is_open()) {
            std::cerr << "Error opening file: " << filepath << std::endl;
        }

        std::ostringstream Buffertemp;

        Buffertemp.clear();
        digest = "";

        Buffertemp << file.rdbuf();

        std::string rawfile = Buffertemp.str();

        hash.Update((const byte*)rawfile.data(), rawfile.size());
        digest.resize(hash.DigestSize());
        hash.Final((byte*)&digest[0]);

        std::string encodedDigest;
        StringSource(digest, true, new HexEncoder(new StringSink(encodedDigest)));
        encodedDigest.resize(4);


        for (int i = 0; i < 5; i++) {
            if (HashCode[i] == encodedDigest && i == 0) {
                contentBuffer0 = rawfile;
            }
            if (HashCode[i] == encodedDigest && i == 1) {
                contentBuffer1 = rawfile;
            }
            if (HashCode[i] == encodedDigest && i == 2) {
                contentBuffer2 = rawfile;
            }
            if (HashCode[i] == encodedDigest && i == 3) {
                contentBuffer3 = rawfile;
            }
            if (HashCode[i] == encodedDigest && i == 4) {
                contentBuffer4 = rawfile;
            }
        }
    }

    contentBuffer << contentBuffer0 << contentBuffer1 << contentBuffer2 << contentBuffer3 << contentBuffer4;

    std::string mergedContent = contentBuffer.str();

    std::string recovered;

    try
    {

        CBC_Mode< AES >::Decryption d;
        d.SetKeyWithIV(key, 32, iv);
        StringSource s(mergedContent, true,
            new StreamTransformationFilter(d,
                new StringSink(recovered)
            ) // StreamTransformationFilter
        ); // StringSource


        std::vector<char> endata(recovered.begin(), recovered.end());
        std::ofstream outputFile("decryptedPPM.exe", std::ios::binary);
        if (!outputFile) {
            std::cerr << "無法創建修改後的檔案\n";
            return 1;
        }
        outputFile.write(endata.data(), endata.size());

        std::cout << "recovered success " << std::endl;

    }
    catch (const Exception& e)
    {
        std::cerr << e.what() << std::endl;
        exit(1);
    }


    return 0;
}