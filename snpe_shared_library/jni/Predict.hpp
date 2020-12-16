#ifndef PREDICT_H
#define PREDICT_H

#include <string>
#include <unordered_map>
#include <vector>

#include "SNPE/SNPE.hpp"
#include "DlSystem/ITensor.hpp"
#include "DlSystem/UserBufferMap.hpp"

std::vector<float> predictSnpe(std::unique_ptr<zdl::SNPE::SNPE> &snpe, std::vector<uint8_t> &inBuffer);

#endif