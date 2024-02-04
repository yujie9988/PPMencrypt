#include "cryptlib.h"
#include "rijndael.h"
#include "modes.h"
#include "files.h"
#include "osrng.h"
#include "hex.h"

#include <iostream>
#include <string>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <vector>
#include <sstream>


int main(int argc, char* argv[])
{
    using namespace CryptoPP;
    byte iv[32] = "0123456789ABCDEF0123456789ABCDE";
    byte key[32] = "0123456789ABCDEF0123456789ABC0A";

    const std::vector<std::string> fileParts = { "PPMen_0.exe", "PPMen_1.exe",
                                                    "PPMen_2.exe", "PPMen_3.exe",
                                                    "PPMen_4.exe" };

    // 打開輸出檔案
    std::ofstream outputFile("merged_ppm.exe", std::ios::binary);
    if (!outputFile) {
        std::cerr << "無法打開輸出檔案\n";
        return 1;
    }

    std::ostringstream contentBuffer;

    // 合併檔案
    for (const auto& filePart : fileParts) {
        // 打開每個部分的檔案
        std::ifstream inputFile(filePart, std::ios::binary);
        if (!inputFile) {
            std::cerr << "無法打開檔案: " << filePart << "\n";
            return 1;
        }

        // 讀取檔案內容
        contentBuffer << inputFile.rdbuf();

    }


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