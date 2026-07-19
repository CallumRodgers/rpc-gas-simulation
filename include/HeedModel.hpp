#ifndef HEEDMODEL_HH
#define HEEDMODEL_HH

// Geant4 includes.
#include "G4VFastSimulationModel.hh"

// Garfield++ includes.
#include <G4Electron.hh>
#include <G4Positron.hh>

#include "MainParameters.hpp"
#include "Garfield/MediumMagboltz.hh"
#include "Garfield/GeometrySimple.hh"
#include "Garfield/ComponentParallelPlate.hh"
#include "Garfield/Sensor.hh"
#include "Garfield/TrackHeed.hh"
#include "Garfield/AvalancheGrid.hh"
#include "Garfield/AvalancheMicroscopic.hh"
#include "Garfield/ViewSignal.hh"

namespace RPC::GarfieldInterface {

    /** Partículas que devem ser simuladas no Garfield. */
    inline const G4String DELTA_ELECTRON_PARTICLES[] = {
        "e-"
    };

    /** Partículas que devem ser simuladas no Garfield. */
    inline const G4String NEW_TRACK_PARTICLES[] = {
        "mu-", "mu+", "e-", "e+"
    };

    constexpr bool USING_NEW_TRACK = true;

    class HeedModel : public G4VFastSimulationModel {
    public:
        explicit HeedModel(G4Envelope* gasRegion);
        ~HeedModel() override;

        G4bool IsApplicable(const G4ParticleDefinition &) override;
        G4bool ModelTrigger(const G4FastTrack &) override;

        void DoIt(const G4FastTrack &, G4FastStep &) override;

        void InitialiseGarfieldMarta(
            G4double halfX, G4double halfY, G4double halfZ,
            G4double hv, const G4String& gasFile,
            G4double padX, G4double padZ
        );
        void InitialiseGarfieldIRPC(
            G4double halfX, G4double halfY, G4double halfZ,
            G4double hv, const G4String& gasFile,
            G4int nStrips
        );

    protected:
        // Objetos do Garfield++ são prefixos com "g".
        Garfield::MediumMagboltz* gGasMedium{};
        Garfield::GeometrySimple* gGeometry{};
        Garfield::ComponentParallelPlate* gRPC{};
        Garfield::Sensor* gSensor{};
        Garfield::TrackHeed* gTrackHeed{};
        Garfield::AvalancheGrid* gAvalancheGrid{};
        Garfield::AvalancheMicroscopic* gAvalancheMicroscopic{};
        Garfield::ViewSignal* gViewSignal{};

        void DoItNewTrack(const G4FastTrack &, G4FastStep &) const;
        void DoItDeltaElectron(const G4FastTrack &, G4FastStep &) const;

    private:
        G4double dGasGap;
        G4double dResistivePlate;

        void MakeGas(const G4String& gasFile);
        void MakeGeometry(double halfXcm, double halfYcm, double halfZcm);
        void SetupAvalanches(double halfXcm, double halfYcm, double halfZcm);
        void SetupHeed();
    };

}

#endif