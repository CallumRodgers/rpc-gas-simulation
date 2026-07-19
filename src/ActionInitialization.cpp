#include "ActionInitialization.hpp"

#include <G4RunManager.hh>

#include "DetectorConstructionMarta.hpp"
#include "SteppingAction.hpp"

using namespace RPC;

ActionInitialization::ActionInitialization()
{}

ActionInitialization::~ActionInitialization()
{}

void ActionInitialization::BuildForMaster() const
{
    RunAction *runAction = new RunAction();
    SetUserAction(runAction);
}

void ActionInitialization::Build() const
{
    PrimaryGenerator *generator = new PrimaryGenerator();
    SetUserAction(generator);

    RunAction *runAction = new RunAction();
    SetUserAction(runAction);

    auto steppingAction = new SteppingAction();
    SetUserAction(steppingAction);
}