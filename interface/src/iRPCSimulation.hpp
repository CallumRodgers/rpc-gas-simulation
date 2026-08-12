#ifndef RPC_SIMULATION_IRPCSIMULATION_HPP
#define RPC_SIMULATION_IRPCSIMULATION_HPP

#include "GarfieldInterface.hpp"

#include <Garfield/GeometrySimple.hh>
#include <Garfield/MediumMagboltz.hh>
#include <Garfield/ComponentParallelPlate.hh>
#include <Garfield/Sensor.hh>
#include <Garfield/TrackHeed.hh>
#include <Garfield/AvalancheGrid.hh>
#include <Garfield/AvalancheMicroscopic.hh>
#include <Garfield/ViewSignal.hh>

namespace RPC::GarfieldInterface {
    class iRPCSimulation {
    public:
        // Constantes para a geometria da iRPC do CERN em cm.
        static constexpr double GARFIELD_VOLUME_X = 50.0;
        static constexpr double GARFIELD_VOLUME_Y = 6 * 1.4e-1;
        static constexpr double GARFIELD_VOLUME_Z = 50.0;

        explicit iRPCSimulation(DetectorConfig config) : mDetectorConfig(std::move(config)) {}

        /**
        * Carregando arquivos de gás, inicializando campo elétrico, geometria, etc.
        */
        void initGarfield();
        void setConfig(const DetectorConfig& config) { mDetectorConfig = config; }
        void processEvent(EventData data);

    private:
        DetectorConfig mDetectorConfig;

        std::unique_ptr<Garfield::MediumMagboltz>           mGasMedium;
        std::unique_ptr<Garfield::GeometrySimple>           mGeometry;
        std::unique_ptr<Garfield::ComponentParallelPlate>   mRPC;
        std::unique_ptr<Garfield::Sensor>                   mSensor;
        std::unique_ptr<Garfield::TrackHeed>                mTrackHeed;
        std::unique_ptr<Garfield::AvalancheGrid>            mAvalancheGrid;
        std::unique_ptr<Garfield::AvalancheMicroscopic>     mAvalancheMicroscopic;
        std::unique_ptr<Garfield::ViewSignal>               mViewSignal;

        void setupGeometry();
        void setupGas();
        void setupHeed();
        void setupAvalanches();
    };
}

#endif //RPC_SIMULATION_IRPCSIMULATION_HPP
