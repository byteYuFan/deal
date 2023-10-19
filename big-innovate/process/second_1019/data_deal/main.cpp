#include <iostream>
#include "src/header/deal.h"
#include "src/header/CONSTVALUE.h"
#include "src/header/compressor.h"
#include <bitset>
void test(){
    UserInfo user1(1, 'M', 25, 7, 95122);
    UserInfo user2(2, 'F', 35, 3, 98105);

    // 创建 BitCompressor
    BitCompressor compressor;

    // 压缩 UserInfo 对象
    compressor.Append(user1.userID, 16);
    compressor.Append(user1.gender == 'M' ? 0 : 1, 1);
    compressor.Append(user1.age, 3);
    compressor.Append(user1.occupation, 10);
    compressor.Append(user1.zipCode, 32);

    compressor.Append(user2.userID, 16);
    compressor.Append(user2.gender == 'M' ? 0 : 1, 1);
    compressor.Append(user2.age, 3);
    compressor.Append(user2.occupation, 10);
    compressor.Append(user2.zipCode, 32);
    compressor.Flush();

    // 获取压缩数据
    const std::vector<uint32_t>& compressedData = compressor.GetCompressedData();

    // 创建 BitDecompressor
    BitDecompressor decompressor(compressedData);

    // 解压缩 UserInfo 对象
    UserInfo restoredUser1(0, 'M', 0, 0, 0);
    decompressor.GetNextValue(restoredUser1.userID, 16);
    char genderBit;
    decompressor.GetNextValue(genderBit, 1);
    restoredUser1.gender = (genderBit == 0) ? 'M' : 'F';
    decompressor.GetNextValue(restoredUser1.age, 3);
    decompressor.GetNextValue(restoredUser1.occupation, 10);
    decompressor.GetNextValue(restoredUser1.zipCode, 32);

    UserInfo restoredUser2(0, 'M', 0, 0, 0);
    decompressor.GetNextValue(restoredUser2.userID, 16);
    decompressor.GetNextValue(genderBit, 1);
    restoredUser2.gender = (genderBit == 0) ? 'M' : 'F';
    decompressor.GetNextValue(restoredUser2.age, 3);
    decompressor.GetNextValue(restoredUser2.occupation, 10);
    decompressor.GetNextValue(restoredUser2.zipCode, 32);

    // 输出恢复的 UserInfo 对象
    std::cout << "Restored User 1: ID=" << restoredUser1.userID << ", Gender=" << restoredUser1.gender << ", Age=" << static_cast<int>(restoredUser1.age) << ", Occupation=" << restoredUser1.occupation << ", ZipCode=" << restoredUser1.zipCode << std::endl;
    std::cout << "Restored User 2: ID=" << restoredUser2.userID << ", Gender=" << restoredUser2.gender << ", Age=" << static_cast<int>(restoredUser2.age) << ", Occupation=" << restoredUser2.occupation << ", ZipCode=" << restoredUser2.zipCode << std::endl;



}

int main() {
//    MyError error;
//    std::map<std::string, std::string> occupation;
//    std::vector<UserInfo> users;
//    read_occupation(DATADIR+"occupation.txt",occupation,error);
//    if(!error.pass()){
//        std::cout<<error.getErrorCode()<<" "<<error.getErrorDescription()<<std::endl;
//    }
//    read_user_info(DATADIR+"users.dat",users,error);
//    if(!error.pass()){
//        std::cout<<error.getErrorCode()<<" "<<error.getErrorDescription()<<std::endl;
//    }


    test();


}
