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
#include <cmath>



void splitAndWriteToFile(const std::string& inputString, const std::string& baseFileName) {
    // 計算每一份的大小
    const int numParts = 5;
    std::size_t partSize = std::ceil(static_cast<double>(inputString.size()) / numParts);

    for (int i = 0; i < numParts; ++i) {
        // 計算當前份的起始索引和大小
        std::size_t startIndex = i * partSize;
        std::size_t currentPartSize = std::min(partSize, inputString.size() - startIndex);

        // 提取當前份的內容
        std::string currentPart = inputString.substr(startIndex, currentPartSize);

        // 建立檔案名稱
        std::string fileName = baseFileName + "_" + std::to_string(i) + ".exe";

        // 將當前份的內容寫入檔案
        std::ofstream outputFile(fileName, std::ios::binary);
        if (outputFile) {
            outputFile.write(currentPart.c_str(), currentPart.size());
            std::cout << "檔案 " << fileName << " 寫入完成\n";
        }
        else {
            std::cerr << "無法打開檔案: " << fileName << "\n";
        }
    }
}








int main(int argc, char* argv[])
{
    using namespace CryptoPP;

    byte iv[32] = "0123456789ABCDEF0123456789ABCDE";
    byte key[32] = "0123456789ABCDEF0123456789ABC0A";

    std::ifstream inputFile("updater.exe", std::ios::binary);
    if (!inputFile) {
        std::cerr << "無法打開原始檔案\n";
        return 1;
    }

    // 取得原始檔案大小
    inputFile.seekg(0, std::ios::end);
    std::streampos fileSize = inputFile.tellg();
    inputFile.seekg(0, std::ios::beg);
    // 讀取整個檔案到一個 String
    std::string fileContent(static_cast<std::size_t>(fileSize), '\0');
    inputFile.read(&fileContent[0], fileSize);

    AutoSeededRandomPool prng;
    HexEncoder encoder(new FileSink(std::cout));



    std::string plain = "dtyjdhhhhhhhhhhhhhhhhhhh";
    std::string cipher, recovered;

    std::cout << "plain text: " << plain << std::endl;

    /*********************************\
    \*********************************/

    try
    {


        CBC_Mode< AES >::Encryption e;
        e.SetKeyWithIV(key, 32, iv);

        StringSource s(fileContent, true,
            new StreamTransformationFilter(e,
                new StringSink(cipher)
            ) // StreamTransformationFilter
        ); // StringSource

        splitAndWriteToFile(cipher, "PPMen");

        std::cout << cipher.data() << std::endl;

        double filesize = cipher.size();

        std::cout << "sizeeeeeeeeeee:" << filesize << std::endl;

        const int numParts = 5;

        std::streampos partSize = std::ceil(static_cast<double>(filesize) / numParts);

        std::cout << "partsizeeeeeeeeeee:" << partSize << std::endl;

        /*
        for (int i = 0; i < numParts; ++i) {
            // 計算當前份的大小
            std::streampos currentPartSize;

            if (i == numParts - 1) {
                currentPartSize = filesize - i * partSize;
                std::cout << "cur:" << currentPartSize << std::endl;
            }
            else {
                currentPartSize = partSize ;
                std::cout << "cur:" << currentPartSize << std::endl;
            }
            std::cout << currentPartSize << std::endl;

            // 創建一個包含當前份內容的 std::vector<char>
            std::vector<char> buffer(currentPartSize);
            inputFile.read(buffer.data(), currentPartSize);

            // 將資料寫入新的檔案
            std::ofstream outputFile("PPM_file_part" + std::to_string(i) + ".exe", std::ios::binary);
            if (!outputFile) {
                std::cerr << "無法創建修改後的檔案\n";
                return 1;
            }

            // 使用當前份的大小而不是之前讀取的 fileSize
            outputFile.write(buffer.data(), currentPartSize);

            std::cout << "檔案第 " << i + 1 << " 部分複製完成\n";
            
        }

        */

        //std::vector<char> endata(cipher.begin(), cipher.end());
        //std::ofstream outputFile("enp.exe", std::ios::binary);
        //if (!outputFile) {
        //    std::cerr << "無法創建修改後的檔案\n";
        //    return 1;
        //}
        //outputFile.write(endata.data(), endata.size());

    }
    catch (const Exception& e)
    {
        std::cerr << e.what() << std::endl;
        exit(1);
    }

    /*********************************\
    \*********************************/




    return 0;
}