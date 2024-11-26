//
// Created by callum on 26/11/24.
//

#ifndef ACTIONINITIALIZATION_H
#define ACTIONINITIALIZATION_H

#include "G4VUserActionInitialization.hh"

class ActionInitialization : public G4VUserActionInitialization {
    public:
        ActionInitialization() = default;
        ~ActionInitialization() override = default;
        void Build() const override;
};

#endif //ACTIONINITIALIZATION_H
