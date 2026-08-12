
#include <TCanvas.h>
#include <TF1.h>

#include "TApplication.h"
#include "TTree.h"
#include "ROOT/RDataFrame.hxx"

int main(int argc, char **argv) {
    auto app = TApplication("AnalysisApp", &argc, argv);

    auto canvas = TCanvas("c1", "Signal");

    //auto tree = TTree();
    //tree.ReadFile("signal.csv", "time:charge:z:w");
    //tree.Draw("time", "charge", "hist");
    //auto htemp = (TH1F*) gPad->GetPrimitive("htemp");
    //htemp->SetTitle("Readout Signal");
    //htemp->SetXTitle("Time [ns]");
    //htemp->SetYTitle("Induced Charge [fC]");
    app.Run();
    return 0;
}
