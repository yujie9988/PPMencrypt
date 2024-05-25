#include "cryptlib.h"
#include "rijndael.h"
#include "modes.h"
#include "files.h"
#include "osrng.h"
#include "hex.h"
#include "sha.h"

#include <iostream>
#include <string>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <vector>
#include <cmath>
#include <filesystem>

using namespace CryptoPP;
namespace fs = std::filesystem;


class ppmc{

public:


    void splitAndWriteToFile(const std::string& inputString,const int length) {
        // 計算每一份的大小
        const int numParts = 5;
        const int filenameLength = 10;
        // 初始化亂數種子
        std::srand(std::time(0));

        static const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        static const int charsetSize = sizeof(charset) - 1;

        std::string randomFilename;

        randomFilename.reserve(length);

        std::size_t partSize = std::ceil(static_cast<double>(inputString.size()) / numParts);

        HexEncoder encoder(new FileSink(std::cout));

        std::string digest;

        SHA256 hash;

        for (int i = 0; i < numParts; ++i) {
            // 計算當前份的起始索引和大小
            randomFilename = "";
            for (int i = 0; i < length; ++i) {
                randomFilename += charset[rand() % charsetSize];
            }
            std::size_t startIndex = i * partSize;
            std::size_t currentPartSize = std::min(partSize, inputString.size() - startIndex);

            // 提取當前份的內容
            std::string currentPart = inputString.substr(startIndex, currentPartSize);

            // 建立檔案名稱
            fs::create_directories("data");
            std::string fileName = "data//" + randomFilename + ".exe";

            digest = "";

            hash.Update((const byte*)currentPart.data(), currentPart.size());
            digest.resize(hash.DigestSize());
            hash.Final((byte*)&digest[0]);

            // 將當前份的內容寫入檔案
            std::ofstream outputFile(fileName, std::ios::binary);
            if (outputFile) {
                outputFile.write(currentPart.c_str(), currentPart.size());
                std::string encodedDigest;
                std::cout << "檔案 " << i << ":" << fileName << "輸出完成";
                std::cout << "Hash: ";
                StringSource(digest, true, new HexEncoder(new StringSink(encodedDigest)));
                StringSource(digest, true, new Redirector(encoder));
                std::cout << std::endl;
            }
            else {
                std::cerr << "無法打開檔案: " << fileName << "\n";
            }
        }
        system("pause");
    }

    std::string generateRandomFilename(int length) {
        static const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        static const int charsetSize = sizeof(charset) - 1;

        std::string randomFilename;
        randomFilename.reserve(length);

        for (int i = 0; i < length; ++i) {
            randomFilename += charset[rand() % charsetSize];
        }

        return randomFilename;
    }

    void ppmcrypt(const char* input) {
        // 確定字串長度
        size_t length = strlen(input);

        // 建立一個 CryptoPP::byte 陣列
        CryptoPP::byte* byteArray = new CryptoPP::byte[33];

        // 將 char 陣列轉換為 CryptoPP::byte 陣列
        for (size_t i = 0; i < 33; ++i) {
            byteArray[i] = static_cast<CryptoPP::byte>(input[i]);
        }

        processmain(byteArray);


        // 刪除動態分配的陣列
        delete[] byteArray;
    }


    void processmain(const CryptoPP::byte* key)
    {
        
        using namespace CryptoPP;
        const int filenameLength = 10;

        byte iv[33] = "0123456789ABCDEF0123456789ABCDE";

        std::ifstream inputFile("updater.exe", std::ios::binary);
        if (!inputFile) {
        std::cerr << "無法打開原始檔案\n";
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

        std::string cipher, recovered;



        try
        {


            CBC_Mode< AES >::Encryption e;
            e.SetKeyWithIV(key, 32, iv);

            StringSource s(fileContent, true,
                new StreamTransformationFilter(e,
                    new StringSink(cipher)
                ) // StreamTransformationFilter
            ); // StringSource


            splitAndWriteToFile(cipher, filenameLength);




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




    }
};