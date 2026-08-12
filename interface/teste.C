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
// --- ADICIONADO: Header do AvalancheMC ---
#include "Garfield/AvalancheMC.hh" 

using namespace Garfield;

int main(int argc, char * argv[]) {
    // Inicialização da aplicação ROOT
    TApplication app("app", &argc, argv);
    
    // Configuração de Threads
    int numThreads = omp_get_max_threads() / 4;
    if (numThreads < 1) numThreads = 1;
    ROOT::EnableImplicitMT(numThreads);
    omp_set_num_threads(numThreads);
    
    std::cout << "Inicializando com " << numThreads << " threads." << std::endl;
    
    // Modo silencioso: guarda apenas as imagens PNG sem abrir janelas
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
    // 2. Geometria (Gás como Meio Global)
    // =========================================================
    GeometrySimple geo;
    geo.SetMedium(&gasMedium); 

    const double hX = 15.0 / 2.0; 
    const double hY = 20.0 / 2.0; 
    const double rpcVoltage = 10000.0;

    // --- PLACAS SUPERIORES ---
    SolidBox bakeliteTop(0., 0., 0.14, hX, hY, 0.07);
    bakeliteTop.SetBoundaryDielectric();
    geo.AddSolid(&bakeliteTop, &bakeliteMedium);

    SolidBox graphiteTop(0., 0., 0.22, hX, hY, 0.01);
    graphiteTop.SetBoundaryPotential(rpcVoltage);
    geo.AddSolid(&graphiteTop, &graphiteMedium);

    // --- PLACAS INFERIORES ---
    SolidBox bakeliteBottom(0., 0., -0.14, hX, hY, 0.07);
    bakeliteBottom.SetBoundaryDielectric();
    geo.AddSolid(&bakeliteBottom, &bakeliteMedium);

    SolidBox graphiteBottom(0., 0., -0.22, hX, hY, 0.01);
    graphiteBottom.SetBoundaryPotential(0.0);
    
    // --- LABEL DO SINAL ---
    const std::string readoutLabel = "ReadoutPlane";
    graphiteBottom.SetLabel(readoutLabel); 
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
    // 4. Secção Gráfica: Campos e Potencial Total
    // =========================================================
    TCanvas canvas("canvas", "Contornos", 1200, 400);
    ViewField view;
    view.SetSensor(&sensor);
    view.SetCanvas(&canvas);
    
    view.SetPlane(0, -1, 0, 0, 0, 0); 
    view.SetArea(-8.0, -0.5, 8.0, 0.5); 
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
                                   0., 0., -0.3, 
                                   0., 0.,  0.3, 
                                   "v", true);
    canvasPerfil.Update();
    canvasPerfil.SaveAs("RPC_WeightingField_Perfil.png");

    // =========================================================
    // 6. Simulação do Sinal Induzido (Heed + Avalanche)
    // =========================================================
    std::cout << "\n--- A INICIAR SIMULAÇÃO DO SINAL ---" << std::endl;

    // Configurar a janela de tempo do sensor (0 a 10 ns)
    const std::size_t nTimeBins = 200;
    const double tmin = 0.;
    const double tmax = 10.0; 
    const double tstep = (tmax - tmin) / nTimeBins;
    sensor.SetTimeWindow(tmin, tstep, nTimeBins);

    TrackHeed track(&sensor);
    track.SetParticle("pion");
    track.SetMomentum(7.e9);
    track.CrossInactiveMedia(true);

    // --- ESTÁGIO 1: Avalanche Microscópica (Apenas para o arranque inicial) ---
    AvalancheMicroscopic aval(&sensor);
    aval.EnableSignalCalculation(); 
    const double tMaxWindow = 0.1;  // Simula microscopicamente apenas 0.1 ns
    aval.SetTimeWindow(0., tMaxWindow);

    // --- ESTÁGIO 2: Avalanche Macroscópica 3D (Substitui o AvalancheGrid) ---
    AvalancheMC avalmc(&sensor);
    avalmc.EnableSignalCalculation();
    // Definir passos de tempo de integração no campo 3D (ex: 0.05 ns)
    avalmc.SetTimeSteps(0.05); 

    std::clock_t start = std::clock();

    std::cout << "A simular a passagem do pião (Heed)..." << std::endl;
    track.NewTrack(0., 0., -0.0699, 0., 0., 0., 1.);

    std::cout << "A simular Avalanches (Micro -> MC 3D)..." << std::endl;

    for (const auto &cluster : track.GetClusters()) {
        for (const auto &electron : cluster.electrons) {
            
            // 1. Processar a fase microscópica inicial
            aval.AvalancheElectron(electron.x, electron.y, electron.z, electron.t, 0.1, 0., 0., 0.);
            
            // 2. Extrair os eletrões resultantes e passá-los para a fase macroscópica
            const int np = aval.GetNumberOfElectronEndpoints();
            for (int i = 0; i < np; ++i) {
                double x0, y0, z0, t0, e0;
                double x1, y1, z1, t1, e1;
                int status;
                
                // Vai buscar a posição (x1, y1, z1) e tempo (t1) finais do rastreio microscópico
                aval.GetElectronEndpoint(i, x0, y0, z0, t0, e0, x1, y1, z1, t1, e1, status);
                
                // Inicia o rastreio Macroscópico 3D a partir desse exato momento e posição
                // O avalmc trata de calcular a estatística de Townsend passo a passo a partir daqui.
                avalmc.AvalancheElectron(x1, y1, z1, t1); 
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