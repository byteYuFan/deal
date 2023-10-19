cmake_minimum_required(VERSION 3.26)
project(data_deal)

set(CMAKE_CXX_STANDARD 17)

add_executable(data_deal main.cpp
        src/source/deal.cpp
        src/header/deal.h
        src/header/CONSTVALUE.h
        src/source/MyError.cpp
        src/header/MyError.h
        src/source/UserInfo.cpp
        src/header/UserInfo.h
        src/header/compressor.h)
