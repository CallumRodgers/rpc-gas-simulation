
#include "physics/ActionInitialization.h"

#include "physics/PrimaryGeneratorAction.h"
#include "physics/RunAction.h"

void ActionInitialization::BuildForMaster() const
{
    auto *runAction = new RunAction();
    SetUserAction(runAction);
}

void ActionInitialization::Build() const {
    SetUserAction(new PrimaryGeneratorAction());
    SetUserAction(new RunAction());
}
