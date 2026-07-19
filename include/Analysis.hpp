#ifndef ANALYSIS_HH
#define ANALYSIS_HH

#include "G4Run.hh"

namespace RPC {
    class Analysis {
    public:
        static void CreateHistogramsMarta();

        static void BeginRun(const G4Run* run);
        static void EndRun(const G4Run* run);

        static void InsertHit();

        static constexpr G4double SIGNAL_WINDOW_NS = 15.0;
        static constexpr G4int SIGNAL_WINDOW_BINS = 200;

    private:
        Analysis() {}
    };
}

#endif //ANALYSIS_HH
