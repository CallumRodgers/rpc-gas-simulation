#ifndef ABORTIONHANDLER_HH
#define ABORTIONHANDLER_HH

#include "G4VStateDependent.hh"

namespace RPC {
    class AbortionHandler : public G4VStateDependent {
    public:
        AbortionHandler();
        ~AbortionHandler() override;

        G4bool Notify(G4ApplicationState requestedState) override;
    };
}

#endif