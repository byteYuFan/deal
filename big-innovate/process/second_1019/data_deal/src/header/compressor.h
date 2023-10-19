//
// Created by wangyufan on 2023/10/19.
//

#ifndef DATA_DEAL_COMPRESSOR_H
#define DATA_DEAL_COMPRESSOR_H


#include <iostream>
#include <vector>
#include <cstdint>
#include <bitset>
class BitCompressor {
public:
    BitCompressor() : data_(0), numBits_(0) {}

    template <typename T>
    void Append(T value, uint8_t numBits) {
        if (numBits_ + numBits > 32) {
            // 如果当前 32 位不够存放，需要刷新
            Flush();
        }
        data_ |= (value & ((1U << numBits) - 1)) << numBits_;
        numBits_ += numBits;
    }

    void Flush() {

        if (numBits_ > 0) {
            // 将当前 32 位添加到压缩数据
            compressedData_.push_back(data_);
            data_ = 0;
            numBits_ = 0;
        }
    }

    [[nodiscard]] const std::vector<uint32_t>& GetCompressedData() const {
        return compressedData_;
    }

private:
    uint32_t data_;              // 用于存放当前 32 位数据
    uint8_t numBits_;            // 当前已存的比特数
    std::vector<uint32_t> compressedData_;  // 存放压缩后的数据
};

class BitDecompressor {
public:
    explicit BitDecompressor(const std::vector<uint32_t>& compressedData)
            : compressedData_(compressedData), dataIndex_(0), currentData_(0), currentBit_(0) {}

    template <typename T>
    bool GetNextValue(T& value, uint8_t numBits) {
        if (dataIndex_ >= compressedData_.size() * 32) {
            return false; // 已经处理完所有数据
        }

        // 从当前数据中提取 numBits 位数据
        uint32_t mask = (1U << numBits) - 1;
        value = static_cast<T>((compressedData_[dataIndex_ / 32] >> currentBit_) & mask);

        // 更新当前位偏移和索引
        currentBit_ += numBits;
        if (currentBit_ >= 32) {
            currentBit_ = 0;
            dataIndex_++;
        }

        return true;
    }

private:
    const std::vector<uint32_t>& compressedData_;
    size_t dataIndex_;
    uint32_t currentData_;
    uint8_t currentBit_;
};

#endif //DATA_DEAL_COMPRESSOR_H
