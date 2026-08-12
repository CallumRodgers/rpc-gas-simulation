
#include "GarfieldInterface.hpp"

#include <cassert>

#include "iRPCSimulation.hpp"
#include "MartaSimulation.hpp"

using namespace RPC::GarfieldInterface;

Interface::Interface(const DetectorType& type, const DetectorConfig& config) {
    mType = type;
    if (type == DetectorType::iRPC) {
        miRPCSimulation  = std::make_unique<iRPCSimulation>(config);
    } else if (type == DetectorType::MARTA) {
        mMartaSimulation = std::make_unique<MartaSimulation>(config);
    }
}

Interface::~Interface() {}

void Interface::prepareDetectors() const {
    if (mType == DetectorType::iRPC) {
        miRPCSimulation->initGarfield();
    } else if (mType == DetectorType::MARTA) {
        mMartaSimulation->initGarfield();
    }
}

void Interface::transmit(const EventData& event) const {
    if (mType == DetectorType::iRPC) {
        assert(miRPCSimulation != nullptr);
        miRPCSimulation->processEvent(event);
    } else if (mType == DetectorType::MARTA) {
        assert(mMartaSimulation != nullptr);
        mMartaSimulation->processEvent(event);
    }
}
