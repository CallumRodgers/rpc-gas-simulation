#include <iostream>
#include <vector>
#include <string>
#include <omp.h>
#include <ctime>
#include <numeric>

#include <TApplication.h>
#include <TCanvas.h>
#include <TROOT.h>
#include <TSystem.h>

#include "Garfield/MediumMagboltz.hh"
#include "Garfield/MediumConductor.hh"
#include "Garfield/MediumPlastic.hh"
#include "Garfield/SolidBox.hh"
#include "Garfield/GeometrySimple.hh"
#include "Garfield/ComponentNeBem3d.hh"
#include "Garfield/Sensor.hh"
#include "Garfield/ViewField.hh"
#include "Garfield/ViewGeometry.hh"
#include "Garfield/ViewSignal.hh"
#include "Garfield/TrackHeed.hh"
#include "Garfield/AvalancheMicroscopic.hh"
#include "Garfield/AvalancheMC.hh" 

using namespace Garfield;

int main(int argc, char * argv[]) {
    // Inicialização da aplicação ROOT
    TApplication app("app", &argc, argv);

    auto canvas = TCanvas("canvas", "Contornos", 1200, 400);

    // Configuração de Threads
    int numThreads = omp_get_max_threads() / 4;
    if (numThreads < 1) numThreads = 1;
    //ROOT::EnableImplicitMT(numThreads);
    omp_set_num_threads(numThreads);
    
    std::cout << "Inicializando com " << numThreads << " threads." << std::endl;
    gROOT->SetBatch(true); 

    // =========================================================
    // 1. Definição dos Materiais
    // =========================================================
    MediumConductor graphiteMedium;
    
    MediumPlastic bakeliteMedium;
    bakeliteMedium.SetDielectricConstant(5.0);

    MediumMagboltz gasMedium;
    const std::string gasFile = "cms_rpc_95.2_4.5_0.3_25-40kV.gas";
    
    std::cout << "A carregar o ficheiro de gás..." << std::endl;
    if (!gasMedium.LoadGasFile(gasFile)) {
        gasMedium.SetComposition("ar", 70., "co2", 30.);
    } else {
        gasMedium.Initialise(true);
    }
    gasMedium.SetDielectricConstant(1.0); 

    // =========================================================
    // 2. Geometria DUPLO GAP (Gás como Meio Global)
    // =========================================================
    GeometrySimple geo;
    geo.SetMedium(&gasMedium); 

    const double hX = 15.0 / 2.0; 
    const double hY = 20.0 / 2.0; 
    
    // TENSÃO ATUALIZADA PARA 7 kV (7000 V)
    const double rpcVoltage = 16000.0;

    // --- PAINEL CENTRAL (Leitura - Ground) ---
    // Z = -0.01 a 0.01 (0.2 mm de espessura)
    SolidBox graphiteCenter(0., 0., 0.0, hX, hY, 0.01);
    graphiteCenter.SetBoundaryPotential(0.0);
    const std::string readoutLabel = "ReadoutPlane";
    graphiteCenter.SetLabel(readoutLabel); 
    geo.AddSolid(&graphiteCenter, &graphiteMedium);

    // Baquelite a isolar o painel central
    SolidBox bakeliteCenterTop(0., 0., 0.06, hX, hY, 0.05);    // Z = 0.01 a 0.11
    bakeliteCenterTop.SetBoundaryDielectric();
    geo.AddSolid(&bakeliteCenterTop, &bakeliteMedium);

    SolidBox bakeliteCenterBottom(0., 0., -0.06, hX, hY, 0.05); // Z = -0.01 a -0.11
    bakeliteCenterBottom.SetBoundaryDielectric();
    geo.AddSolid(&bakeliteCenterBottom, &bakeliteMedium);

    // --- GAP SUPERIOR (Z = 0.11 a 0.31 -> 2 mm de gás) ---
    
    // Placas Superiores Exteriores
    SolidBox bakeliteTop(0., 0., 0.41, hX, hY, 0.10);          // Z = 0.31 a 0.51
    bakeliteTop.SetBoundaryDielectric();
    geo.AddSolid(&bakeliteTop, &bakeliteMedium);

    SolidBox graphiteTop(0., 0., 0.52, hX, hY, 0.01);          // Z = 0.51 a 0.53
    graphiteTop.SetBoundaryPotential(rpcVoltage);              // ALTA TENSÃO
    geo.AddSolid(&graphiteTop, &graphiteMedium);

    // --- GAP INFERIOR (Z = -0.31 a -0.11 -> 2 mm de gás) ---

    // Placas Inferiores Exteriores
    SolidBox bakeliteBottom(0., 0., -0.41, hX, hY, 0.10);      // Z = -0.31 a -0.51
    bakeliteBottom.SetBoundaryDielectric();
    geo.AddSolid(&bakeliteBottom, &bakeliteMedium);

    SolidBox graphiteBottom(0., 0., -0.52, hX, hY, 0.01);      // Z = -0.51 a -0.53
    graphiteBottom.SetBoundaryPotential(rpcVoltage);           // ALTA TENSÃO
    geo.AddSolid(&graphiteBottom, &graphiteMedium);

    // =========================================================
    // 3. Configurar o Solver neBEM3D e Sensor
    // =========================================================
    ComponentNeBem3d nebem;
    nebem.SetGeometry(&geo);
    nebem.SetNumberOfThreads(numThreads);
    nebem.SetTargetElementSize(1); 
    nebem.SetMinMaxNumberOfElements(3, 20); 
    
    std::cout << "A inicializar o neBEM..." << std::endl;
    nebem.Initialise();

    Sensor sensor;
    sensor.AddComponent(&nebem);
    sensor.AddElectrode(&nebem, readoutLabel);

    // =========================================================
    // LOGS DE VERIFICAÇÃO DE SANIDADE: CAMPOS E POTENCIAIS NOS GAPS
    // =========================================================
    std::cout << "\n=========================================================" << std::endl;
    std::cout << "          DOUBLE CHECK: CAMPOS E POTENCIAIS (7 kV)       " << std::endl;
    std::cout << "=========================================================" << std::endl;
    double ex = 0., ey = 0., ez = 0., v = 0.;
    Medium* m = nullptr;
    int statusField = 0;
    
    // Sondagem no centro do Gap Inferior (Z = -0.21 cm)
    sensor.ElectricField(0.0, 0.0, -0.21, ex, ey, ez, v, m, statusField);
    std::cout << "Centro Gap Inferior (Z = -0.21 cm) -> Ez: " << ez << " V/cm | Potencial: " << v << " V" << std::endl;
    
    // Sondagem no centro do Gap Superior (Z = +0.21 cm)
    sensor.ElectricField(0.0, 0.0, 0.21, ex, ey, ez, v, m, statusField);
    std::cout << "Centro Gap Superior (Z = +0.21 cm) -> Ez: " << ez << " V/cm | Potencial: " << v << " V" << std::endl;
    
    // Sondagem no Painel Central de Leitura (Ground, Z = 0.0)
    sensor.ElectricField(0.0, 0.0, 0.0, ex, ey, ez, v, m, statusField);
    std::cout << "Painel de Leitura   (Z =  0.00 cm) -> Ez: " << ez << " V/cm | Potencial: " << v << " V" << std::endl;
    std::cout << "=========================================================\n" << std::endl;

    // =========================================================
    // 4. Secção Gráfica: Campos e Potencial Total
    // =========================================================

    ViewField view;
    view.SetSensor(&sensor);
    view.SetCanvas(&canvas);
    
    view.SetPlane(0, -1, 0, 0, 0, 0); 
    view.SetArea(5.0, -0.6, 8.0, 0.6); 
    view.SetNumberOfContours(100);
    
    std::cout << "A desenhar Potencial Total..." << std::endl;
    view.PlotContour("v");
    canvas.Update();
    canvas.SaveAs("RPC_Potencial_Total.png");

    canvas.Clear();
    std::cout << "A desenhar Campo Elétrico Total..." << std::endl;
    view.PlotContour("e");
    canvas.Update();
    canvas.SaveAs("RPC_CampoEletrico_Total.png");

    // =========================================================
    // 5. Secção Gráfica: Weighting Field
    // =========================================================
    canvas.Clear();
    std::cout << "A desenhar o Potencial do Weighting Field (2D)..." << std::endl;
    view.PlotContourWeightingField(readoutLabel, "v");
    canvas.Update();
    canvas.SaveAs("RPC_WeightingField_Contorno.png");

    TCanvas canvasPerfil("canvasPerfil", "Perfil 1D", 600, 600);
    canvasPerfil.SetLeftMargin(0.16);
    view.SetCanvas(&canvasPerfil);
    
    std::cout << "A desenhar o Perfil do Weighting Field ao longo de Z..." << std::endl;
    view.PlotProfileWeightingField(readoutLabel, 
                                   0., 0., -0.6, 
                                   0., 0.,  0.6, 
                                   "v", true);
    canvasPerfil.Update();
    canvasPerfil.SaveAs("RPC_WeightingField_Perfil.png");

    // =========================================================
    // 6. Simulação do Sinal Induzido (Heed + Avalanche)
    // =========================================================
    std::cout << "\n--- A INICIAR SIMULAÇÃO DO SINAL ---" << std::endl;

    const std::size_t nTimeBins = 200;
    const double tmin = 0.;
    const double tmax = 50.0; 
    const double tstep = (tmax - tmin) / nTimeBins;
    sensor.SetTimeWindow(tmin, tstep, nTimeBins);

    // CONFIGURADO: Múon de 150 GeV do SPS
    TrackHeed track(&sensor);
    track.SetParticle("muon");
    track.SetMomentum(150.e9); 
    track.CrossInactiveMedia(true); 

    // --- ESTÁGIO 1: Avalanche Microscópica ---
    AvalancheMicroscopic aval(&sensor);
    aval.EnableSignalCalculation(); 
    const double tMaxWindow = 0.1;  // Segue com precisão microscópica nos primeiros 0.1 ns
    aval.SetTimeWindow(0., tMaxWindow);

    // --- ESTÁGIO 2: Avalanche Macroscópica 3D ---
    AvalancheMC avalmc(&sensor);
    avalmc.EnableSignalCalculation();
    avalmc.SetTimeSteps(0.05); 

    std::clock_t start = std::clock();

    std::cout << "A simular a passagem do múon (Heed)..." << std::endl;
    track.NewTrack(0., 0., -0.3099, 0., 0., 0., 1.);

    std::cout << "A simular Avalanches (Micro -> MC 3D)..." << std::endl;

    for (const auto &cluster : track.GetClusters()) {
        for (const auto &electron : cluster.electrons) {
            
            aval.AvalancheElectron(electron.x, electron.y, electron.z, electron.t, 0.1, 0., 0., 0.);
            
            const int np = aval.GetNumberOfElectronEndpoints();
            for (int i = 0; i < np; ++i) {
                double x0, y0, z0, t0, e0;
                double x1, y1, z1, t1, e1;
                int status;
                
                aval.GetElectronEndpoint(i, x0, y0, z0, t0, e0, x1, y1, z1, t1, e1, status);
                
                // INTEGRALIZAÇÃO DE SEGURANÇA:
                // Passa para o AvalancheMC apenas se o elétron microscópico 
                // parou porque atingiu a janela de tempo de 0.1 ns (status = -5).
                // Evita passar elétrons que já colidiram com o eletrodo (-1) ou anexaram (-7).
                if (status == -5) {
                    avalmc.AvalancheElectron(x1, y1, z1, t1); 
                }
            }
        }
    }

    double duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
    std::cout << "Deriva concluída! Demorou " << duration << " segundos." << std::endl;

    // =========================================================
    // 7. Gráficos do Sinal (Corrente e Carga)
    // =========================================================
    TCanvas cSignal("cSignal", "Corrente Induzida", 600, 600);
    ViewSignal signalView(&sensor);
    signalView.SetCanvas(&cSignal);
    
    std::cout << "A gerar gráfico da corrente induzida..." << std::endl;
    signalView.PlotSignal(readoutLabel);
    cSignal.Update();
    cSignal.SaveAs("RPC_Sinal_Corrente.png");
    gSystem->ProcessEvents();

    TCanvas cCharge("cCharge", "Carga Induzida", 600, 600);
    ViewSignal chargeView(&sensor);
    chargeView.SetCanvas(&cCharge);
    
    std::cout << "A gerar gráfico da carga induzida..." << std::endl;
    sensor.IntegrateSignal(readoutLabel);
    chargeView.PlotSignal(readoutLabel);
    cCharge.Update();
    cCharge.SaveAs("RPC_Sinal_Carga.png");
    gSystem->ProcessEvents();

    std::cout << "\n>> Carga total induzida = " 
              << sensor.GetTotalInducedCharge(readoutLabel) 
              << " [fC] <<" << std::endl;

    std::cout << "Fim do Programa!" << std::endl;
    
    return 0;
}