
#ifndef RUNACTION_H
#define RUNACTION_H

#include "G4UserRunAction.hh"
#include "G4Run.hh"
#include "G4AnalysisManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

class RunAction : public G4UserRunAction
{
public:
    RunAction();
    ~RunAction();

    virtual void BeginOfRunAction(const G4Run *);
    virtual void EndOfRunAction(const G4Run *);
};

#endif //RUNACTION_H
