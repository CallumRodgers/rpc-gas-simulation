#ifndef ACTIONINITIALIZATION_HH
#define ACTIONINITIALIZATION_HH

#include "G4VUserActionInitialization.hh"

#include "PrimaryGenerator.hpp"
#include "RunAction.hpp"

namespace RPC {
    class ActionInitialization : public G4VUserActionInitialization {
    public:
        ActionInitialization();
        ~ActionInitialization();

        virtual void BuildForMaster() const;
        virtual void Build() const;
    };
}

#endif