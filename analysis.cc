
#include <TCanvas.h>

#include "TApplication.h"
#include "ROOT/RDataFrame.hxx"

int main(int argc, char **argv) {
    auto app = TApplication("AnalysisApp", &argc, argv);
    auto tree = TTree();
    tree.ReadFile("signal.csv", "x:y:z:w");
    tree.SetTitle("Readout Signal");
    tree.Draw("x", "y", "HIST");
    app.Run();
    return 0;
}
