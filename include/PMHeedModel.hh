#ifndef PMHEEDMODEL_HH
#define PMHEEDMODEL_HH

// Geant4 includes.
#include "G4VFastSimulationModel.hh"

// Garfield++ includes.
#include <G4Electron.hh>
#include <G4Positron.hh>

#include "PMMainParameters.hh"
#include "Garfield/MediumMagboltz.hh"
#include "Garfield/GeometrySimple.hh"
#include "Garfield/ComponentConstant.hh"
#include "Garfield/Sensor.hh"
#include "Garfield/TrackHeed.hh"

namespace GarfieldInterface {

    /** Partículas que devem ser simuladas no Garfield. */
    inline const G4ParticleDefinition* INTERFACE_PARTICLES[] = {
        G4Electron::Definition()
    };

    class PMHeedModel : public G4VFastSimulationModel {
    public:
        explicit PMHeedModel(G4Envelope* gasRegion);
        ~PMHeedModel() override;

        G4bool IsApplicable(const G4ParticleDefinition &) override;
        G4bool ModelTrigger(const G4FastTrack &) override;

        void DoIt(const G4FastTrack &, G4FastStep &) override;

        void InitialiseGarfield(PMMainParameters* params);

    protected:
        // Objetos do Garfield++ são prefixados com "g".
        G4String gGasFile;
        Garfield::MediumMagboltz* gGasMedium{};
        Garfield::GeometrySimple* gGeometry{};
        Garfield::ComponentConstant* gComp{};
        Garfield::Sensor* gSensor{};
        Garfield::TrackHeed* gTrackHeed{};

    };
}

#endif