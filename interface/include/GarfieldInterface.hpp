#ifndef RPC_SIMULATION_GARFIELDINTERFACE_HPP
#define RPC_SIMULATION_GARFIELDINTERFACE_HPP

#include <string>
#include <memory>

namespace RPC::GarfieldInterface {
    //////////////////////////
    // FORWARD DECLARATIONS //
    //////////////////////////
    class iRPCSimulation;
    class MartaSimulation;

    /// The two types of detectors we'll simulate.
    enum class DetectorType {
        MARTA, iRPC
    };
    /// Possible particles to be transmitted to Garfield++.
    enum class ParticleType {
        ELECTRON,
        POSITRON,
        MUON_MINUS,
        MUON_PLUS,
        // Maybe?
        TAU_MINUS,
        TAU_PLUS
    };

    struct vec3 {
        double x, y, z;
    };

    struct EventData {
        ParticleType type;
        vec3 position;
        vec3 momentum;
        double time;
        double energy;
    };

    struct DetectorConfig {
        std::string gasFile;
        double voltage;
    };

    constexpr double SIGNAL_WINDOW_NS = 15.0;
    constexpr int SIGNAL_WINDOW_BINS = 200;

    class Interface {
    public:
        Interface(const DetectorType& type, const DetectorConfig& config);
        ~Interface();

        void prepareDetectors() const;
        void useDetectorType(const DetectorType& type) { mType = type; }
        /**
         * Transmits a Geant4 particle into the Garfield++ side of the simulation.
         * @param event
         */
        void transmit(const EventData &event) const;

    private:
        DetectorType mType;
        std::unique_ptr<iRPCSimulation> miRPCSimulation;
        std::unique_ptr<MartaSimulation> mMartaSimulation;
    };
}

#endif //RPC_SIMULATION_GARFIELDINTERFACE_HPP
