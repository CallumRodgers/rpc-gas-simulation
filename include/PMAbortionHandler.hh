#ifndef PMABORTIONHANDLER_HH
#define PMABORTIONHANDLER_HH

#include "G4VStateDependent.hh"

class PMAbortionHandler : public G4VStateDependent {
    public:
        PMAbortionHandler();
        ~PMAbortionHandler() override;

        G4bool Notify(G4ApplicationState requestedState) override;
};

#endif