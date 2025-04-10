//
// Created by callum on 26/11/24.
//

#include "physics/PrimaryGeneratorAction.h"

PrimaryGeneratorAction::PrimaryGeneratorAction() {
    fParticleSource = new G4GeneralParticleSource();

    G4ThreeVector pos(
        0. * m,
        0. * m,
        4. * m
    );

    G4ThreeVector dir(
        0., 0., -1.
    );

    G4ParticleTable *table = G4ParticleTable::GetParticleTable();

    auto *muPlus = table->FindParticle("mu+");
    auto *muMinus = table->FindParticle("mu-");

    fParticleSource->SetParticleDefinition(muMinus);
}

PrimaryGeneratorAction::~PrimaryGeneratorAction() {
    delete fParticleSource;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event *event) {
    fParticleSource->GeneratePrimaryVertex(event);
}
