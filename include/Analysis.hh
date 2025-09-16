#ifndef GarfieldAnalysis_h
#define GarfieldAnalysis_h 

#include "G4Version.hh"

#if (G4VERSION_NUMBER < 1100)
  #include "g4root.hh"
#else
  #include "G4AnalysisManager.hh"
#endif 

#endif