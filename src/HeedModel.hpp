#ifndef HEEDMODEL_HH
#define HEEDMODEL_HH

#include "GarfieldInterface.hpp"

// Geant4 includes.
#include "G4VFastSimulationModel.hh"

// Garfield++ includes.
#include <G4Electron.hh>
#include <G4Positron.hh>

namespace RPC::GarfieldInterface {

    /** Partículas que devem ser simuladas no Garfield. */
    inline const G4String DELTA_ELECTRON_PARTICLES[] = {
        "e-"
    };

    /** Partículas que devem ser simuladas no Garfield. */
    inline const G4String INTERFACE_PARTICLES[] = {
        "mu-", "mu+", "e-", "e+"
    };

    constexpr bool USING_NEW_TRACK = true;

    class HeedModel : public G4VFastSimulationModel {
    public:
        HeedModel(G4Envelope* gasRegion, DetectorType type, const DetectorConfig& config);

        G4bool IsApplicable(const G4ParticleDefinition &) override;
        G4bool ModelTrigger(const G4FastTrack &) override;
        void DoIt(const G4FastTrack &, G4FastStep &) override;

    private:
        Interface mInterface;
    };

}

#endif