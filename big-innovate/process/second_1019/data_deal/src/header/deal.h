//
// Created by wangyufan on 2023/10/19.
//

#ifndef DATA_DEAL_DEAL_H
#define DATA_DEAL_DEAL_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include "MyError.h"
#include "UserInfo.h"

void read_occupation(std::string ,std::map<std::string, std::string>&,MyError);
void read_user_info(std::string file_path,std::vector<UserInfo>&users ,MyError error);









#endif //DATA_DEAL_DEAL_H
