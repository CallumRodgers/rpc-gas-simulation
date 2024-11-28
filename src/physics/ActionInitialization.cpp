//
// Created by callum on 26/11/24.
//

#include "physics/ActionInitialization.h"

#include "physics/PrimaryGeneratorAction.h"

void ActionInitialization::Build() const {
    SetUserAction(new PrimaryGeneratorAction());
}
