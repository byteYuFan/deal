//
// Created by wangyufan on 2023/10/19.
//

#ifndef DATA_DEAL_USERINFO_H
#define DATA_DEAL_USERINFO_H

#include <string>
#include <vector>
#include <cstdint>
class UserInfo {
public:
    enum class AgeRange {
        UNDER_18,
        AGE_18_24,
        AGE_25_34,
        AGE_35_44,
        AGE_45_49,
        AGE_50_55,
        ABOVE_56
    };
    UserInfo() {}
    UserInfo(int userID, char gender, AgeRange age, int occupation, int zipCode)
        : userID(userID), gender(gender), age(age), occupation(occupation), zipCode(zipCode) {}

    [[nodiscard]] int getUserID() const { return userID; }
    [[nodiscard]] char getGender() const { return gender; }
    [[nodiscard]] AgeRange getAge() const { return age; }
    [[nodiscard]] int getOccupation() const { return occupation; }
    [[nodiscard]] int getZipCode() const { return zipCode; }

private:
    int userID;
    char gender;
    AgeRange age;
    int occupation;
    int zipCode;
};


class BitDecompressor {
public:
    BitDecompressor(const std::vector<uint32_t>& compressedData)
        : compressedData_(compressedData), data_(0), numBits_(0), dataIndex_(0) {}

    bool GetNextUserInfo(UserInfo& userInfo) {
        if (dataIndex_ >= compressedData_.size() * 32) {
            return false; // 已经处理完所有数据
        }

        uint32_t compressedValue = 0;
        for (int i = 0; i < 32; ++i) {
            if (dataIndex_ >= compressedData_.size() * 32) {
                break;
            }
            compressedValue |= (compressedData_[dataIndex_ / 32] >> (dataIndex_ % 32) & 1) << i;
            dataIndex_++;
        }

        int userID = compressedValue >> 16;
        char gender = (compressedValue >> 15) & 1 ? 'F' : 'M';
        int age = (compressedValue >> 12) & 7;
        int occupation = (compressedValue >> 2) & 1023;
        int zipCode = compressedValue & 0x3FF;

        UserInfo::AgeRange ageRange;
        if (age == 0) {
            ageRange = UserInfo::AgeRange::UNDER_18;
        }
        else if (age == 1) {
            ageRange = UserInfo::AgeRange::AGE_18_24;
        }
        else if (age == 2) {
            ageRange = UserInfo::AgeRange::AGE_25_34;
        }
        else if (age == 3) {
            ageRange = UserInfo::AgeRange::AGE_35_44;
        }
        else if (age == 4) {
            ageRange = UserInfo::AgeRange::AGE_45_49;
        }
        else if (age == 5) {
            ageRange = UserInfo::AgeRange::AGE_50_55;
        }
        else {
            ageRange = UserInfo::AgeRange::ABOVE_56;
        }

        userInfo = UserInfo(userID, gender, ageRange, occupation, zipCode);
        return true;
    }

private:
    const std::vector<uint32_t>& compressedData_;
    uint32_t data_;
    int numBits_;
    size_t dataIndex_;
};

#endif //DATA_DEAL_USERINFO_H
