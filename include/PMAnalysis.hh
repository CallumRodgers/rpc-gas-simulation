#ifndef PMANALYSIS_HH
#define PMANALYSIS_HH

#include "G4Run.hh"

class PMAnalysis {
public:
    static void CreateHistogramsMarta();

    static void BeginRun(const G4Run* run);
    static void EndRun(const G4Run* run);

    static void InsertHit();

    static constexpr G4double SIGNAL_WINDOW_NS = 50.0;
    static constexpr G4int SIGNAL_WINDOW_BINS = 1024;

private:
    PMAnalysis() {}
};

#endif //PMANALYSIS_HH
