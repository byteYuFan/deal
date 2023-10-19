//
// Created by wangyufan on 2023/10/19.
//

#ifndef DATA_DEAL_MYERROR_H
#define DATA_DEAL_MYERROR_H
#include <string>
#include <utility>

class MyError {
public:
    enum ErrorCode {
        NO_ERROR = 0,
        INVALID_INPUT ,
        DIVIDE_BY_ZERO,
        FILE_NOT_FOUND,
        CUSTOM_ERROR,
        FILE_CAN_NOT_OPEN
    };
    MyError()= default;
    MyError(ErrorCode code, std::string  description)
            : error_code(code), error_description(std::move(description)) {}
    void setError(ErrorCode code, std::string description){
            this->error_code=code;
            this->error_description=std::move(description);
    }
    [[nodiscard]] ErrorCode getErrorCode() const {
        return error_code;
    }

    [[nodiscard]] const std::string& getErrorDescription() const {
        return error_description;
    }
    bool pass(){
        return error_code == NO_ERROR;
    }

private:
    ErrorCode error_code = NO_ERROR;
    std::string error_description;
};



#endif //DATA_DEAL_MYERROR_H
