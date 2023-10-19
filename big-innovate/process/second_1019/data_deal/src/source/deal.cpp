//
// Created by wangyufan on 2023/10/19.
//

#include "../header/deal.h"


void read_occupation(std::string file_path,std::map<std::string, std::string>& keyValueMap,MyError error){
    std::ifstream file(file_path); // 打开名为"occupation.txt"的文件
    if (!file.is_open()) {
        error.setError(MyError::FILE_CAN_NOT_OPEN,"can not open the file.");
        std::cout<<error.getErrorCode()<<" "<<error.getErrorDescription()<<std::endl;
        return;
    }
    std::string line;
    while (std::getline(file, line)) {
        size_t pos = line.find(':');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            keyValueMap[key] = value;
        }
    }

    file.close(); // 关闭文件

}
static std::vector<std::string> splitString(const std::string& input, const std::string& delimiter) {
    std::vector<std::string> tokens;
    size_t pos = 0;
    size_t start = 0;
    while ((pos = input.find(delimiter, start)) != std::string::npos) {
        tokens.push_back(input.substr(start, pos - start));
        start = pos + delimiter.length();
    }
    tokens.push_back(input.substr(start, input.length() - start));
    return tokens;
}

//void read_user_info(std::string file_path,std::vector<UserInfo>&users ,MyError error){
//    std::ifstream file(file_path); // 替换为您的文件名
//
//    if (!file.is_open()) {
//        error.setError(MyError::FILE_CAN_NOT_OPEN,"can not open the file.");
//        std::cout<<error.getErrorCode()<<" "<<error.getErrorDescription()<<std::endl;
//        return;
//    }
//
//
//    std::string line;
//    while (std::getline(file, line)) {
//        std::vector<std::string> tokens = splitString(line, "::");
//        if (tokens.size() == 5) {
//            int userID = std::stoi(tokens[0]);
//            char gender = tokens[1][0];
//            UserInfo::AgeRange age = strToAgeRange(tokens[2]);
//            int occupation = std::stoi(tokens[3]);
//            int zipCode = std::stoi(tokens[4]);
//            users.emplace_back(userID, gender, age, occupation, zipCode);
//        }
//
//    }
//    file.close();
//
//}