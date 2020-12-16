#ifndef LOADMODEL_H
#define LOADMODEL_H

#include <string>
#include <unordered_map>
#include <vector>

#include "SNPE/SNPE.hpp"
#include "DlSystem/ITensor.hpp"
#include "DlSystem/UserBufferMap.hpp"

std::unique_ptr<zdl::SNPE::SNPE> loadModel(std::string &dlc, int systemType);

#endif
