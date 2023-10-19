//
// Created by wangyufan on 2023/10/19.
//

#ifndef DATA_DEAL_USERINFO_H
#define DATA_DEAL_USERINFO_H

#include <string>


class UserInfo {
public:

    UserInfo()= default;
    UserInfo(int userID, char gender, int age, int occupation, int zipCode)
            : userID(userID), gender(gender), age(age), occupation(occupation), zipCode(zipCode) {}

    [[nodiscard]] int getUserID() const { return userID; }
    [[nodiscard]] char getGender() const { return gender; }
    [[nodiscard]] int getAge() const { return age; }
    [[nodiscard]] int getOccupation() const { return occupation; }
    [[nodiscard]] int getZipCode() const { return zipCode; }

public:
    int userID{};
    char gender{};
    int age{};
    int occupation{};
    int zipCode{};
};


#endif //DATA_DEAL_USERINFO_H
