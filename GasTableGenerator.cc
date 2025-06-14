//
// Este script gera tabela de gases para uma dada mistura em linha de comando.
//

#include <cstring>
#include <iostream>
#include <thread>
#include "Garfield/MediumMagboltz.hh"
#include "Garfield/MagboltzInterface.hh"

int CORES = 1;
bool VERBOSE = false;

void PrintVersion() {
    std::cout << "GasTableGenerator Version 0.1" << std::endl;
    std::cout << std::endl;
}

void PrintHelp() {
    std::cout << "Usage: gtgen mode [options] <arguments>" << std::endl;
    std::cout << std::endl;
    std::cout << "This program generates Garfield++ gas table files from either std_in setup, "
                 "or by command-line arguments. It just runs Magboltz under the hood." << std::endl;
    std::cout << std::endl;
    std::cout << "Modes:" << std::endl;
    std::cout << "    setup : Enters new gas table setup mode." << std::endl;
    std::cout << "    create (NOT IMPLEMENTED YET) : Quickly creates a new gas table from the given arguments, without step-by-step setup." << std::endl;
    std::cout << "    list : List available gases in Magboltz." << std::endl;
    std::cout << "------------------------------------" << std::endl;
    std::cout << "'create' usage:" << std::endl;
    std::cout << "    gtgen create [options] <gases> <e_min> <e_max> <e_n> <temp>" << std::endl;
    std::cout << std::endl;
    std::cout << "    gases : List in the format \"[gas1],[prop1],[gas2],[prop2],...\" (quotes included) of gases and their proportions, respectively." << std::endl;
    std::cout << "    e_min/e_max : Smallest/Largest electric field to generate transport parameters, in V/cm." << std::endl;
    std::cout << "    e_n : Amount of fields in the [e_min,e_max] range to simulate." << std::endl;
    std::cout << "    temp : Temperature of the gas mixture in Kelvin. Pass '0' to disable thermal motion." << std::endl;
    std::cout << "------------------------------------" << std::endl;
    std::cout << "Options: (For multiple options, please input them separately, so no \"-vc\" for example)" << std::endl;
    std::cout << "    -h | --help : Shows this help." << std::endl;
    std::cout << "    -v | --verbose : Enables verbose output." << std::endl;
    std::cout << "    -c | --cores [n] : Specifies the amount of CPU cores to run Magboltz (default is 1)." << std::endl;
    std::cout << std::endl;
}

// Creates the gas table internally, regardless of how the parameters were passed.
Garfield::MediumMagboltz* CreateGasInternal(
    const double eMin, const double eMax, const int eN,
    const std::string& gas1, const double gasProp1,
    const std::string& gas2, const double gasProp2,
    const std::string& gas3, const double gasProp3,
    const std::string& gas4, const double gasProp4,
    const std::string& gas5, const double gasProp5,
    const std::string& gas6, const double gasProp6,
    double temperature, double pressure,
    const std::string& filename, const bool createFile
) {
    if (temperature < 0.0) {
        temperature = 0.0;
    }
    if (pressure < 0.0) {
        pressure = 0.0;
    }

    // Creating gas object.
    auto* gas = new Garfield::MediumMagboltz();
    gas->EnableDebugging();

    gas->SetComposition(
        gas1, gasProp1,
        gas2, gasProp2,
        gas3, gasProp3,
        gas4, gasProp4,
        gas5, gasProp5,
        gas6, gasProp6
    );

    if (temperature == 0.0) {
        gas->EnableThermalMotion(false);
    } else {
        gas->SetTemperature(temperature);
        gas->EnableThermalMotion(true);
    }

    gas->SetPressure(pressure);

    if (createFile) {
        gas->WriteGasFile(filename);
    } else {
        gas->LoadGasFile(filename);
    }

    double section = (eMax - eMin) / eN;
    for (int i = 0; i < eN; i++) {
        double begin = eMin + i * section;
        double end = begin + section;

        gas->SetFieldGrid(begin, end, 1, false);
        gas->GenerateGasTable(10, VERBOSE);
        gas->MergeGasFile(filename, true);
        gas->WriteGasFile(filename);
    }

    return gas;
}

void SetupNewGas() {
    std::string mixture, filename;
    double temperature = 0.0;

    std::cout << "Please define the gas mixture in the format: [gas1] [proportion1] [gas2] [proportion2] ..." << std::endl;
    std::cout << "It has to be made of at least 1 gas, and at most 6 gases, each with proportions in the [0, 1] range:" << std::endl;
    std::cout << "(Use 'gtgen list' to see a list of available gases and their aliases)" << std::endl;
    std::cout << std::endl;
    std::cout << ">> ";
    std::getline(std::cin, mixture);

    // ################
    // Parsing mixture.
    // ################

    if (mixture.empty()) {
        std::cerr << "Mixture is invalid. Aborting." << std::endl;
        exit(1);
    }

    // Declaring variables.
    std::string gas1, gas2, gas3, gas4, gas5, gas6;
    double gasProp1 = 0.0, gasProp2 = 0.0, gasProp3 = 0.0, gasProp4 = 0.0, gasProp5 = 0.0, gasProp6 = 0.0;

    std::string::size_type index = 0;
    int iter = 0;

    while (index != std::string::npos) {
        std::string::size_type nextIndex = mixture.find(' ', index);
        std::string substr = mixture.substr(index, nextIndex - index);

        switch (iter) {
            case 0: gas1 = substr; break;
            case 1: gasProp1 = std::stod(substr); break;
            case 2: gas2 = substr; break;
            case 3: gasProp2 = std::stod(substr); break;
            case 4: gas3 = substr; break;
            case 5: gasProp3 = std::stod(substr); break;
            case 6: gas4 = substr; break;
            case 7: gasProp4 = std::stod(substr); break;
            case 8: gas5 = substr; break;
            case 9: gasProp5 = std::stod(substr); break;
            case 10: gas6 = substr; break;
            case 11: gasProp6 = std::stod(substr); break;
            default:
                std::cerr << "Too many arguments. Ignoring." << std::endl;
                goto mixtureParseBreak;
        }

        // Updating iteration variables.
        iter++;
        if (nextIndex == std::string::npos) {
            break;
        }
        index = nextIndex + 1;
        if (index > mixture.length()) {
            break;
        }
    }
    mixtureParseBreak:

    std::cout << std::endl;
    std::cout << "I parsed the following gases from your input:" << std::endl;
    std::cout << "\"" << gas1 << "\" \"" << gas2 << "\" \"" << gas3 << "\" \"" << gas4 << "\" \"" << gas5 << "\" \"" << gas6 << "\"" << std::endl;
    std::cout << "And proportions:" << std::endl;
    std::cout << gasProp1 << " " << gasProp2 << " " << gasProp3 << " " << gasProp4<< " " << gasProp5 << " " << gasProp6 << std::endl;

    // ######################
    // Other setup arguments.
    // ######################

    // Thermal motion.
    std::cout << std::endl;
    std::cout << "Thermal motion? [Y/n] (Will override the option passed at program launch)" << std::endl;
    std::string answer;
    std::cout << std::endl;
    std::cout << ">> ";
    std::getline(std::cin, answer);

    if (answer == "Y" || answer == "y") {
        std::cout << std::endl;
        std::cout << "Define the temperature of the mixture. "
                     "Add 'C' right next to the number for Celsius, and 'K' (or nothing) for Kelvin." << std::endl;
        std::cout << std::endl;
        std::cout << ">> ";
        std::getline(std::cin, answer);
        if (answer.at(answer.length() - 1) == 'C' || answer.at(answer.length() - 1) == 'c') {
            temperature = std::stod(answer.erase(answer.length() - 1)) + 273.15;
        } else {
            temperature = std::stod(answer);
        }
    }

    // Filename.
    std::cout << std::endl;
    std::cout << "Choose a name for the new gas file:" << std::endl;
    std::cout << std::endl;
    std::cout << ">> ";
    std::getline(std::cin, filename);
    std::cout << std::endl;

    // Appending ".gas" file type if user did not do so.
    if (filename.length() > 4 && filename.substr(filename.length() - 4) != ".gas") {
        filename.append(".gas");
    }

    // ######################
    // Setting up gas object.
    // ######################

    double eMin = 5000.0;
    double eMax = 9000.0;
    int eN = 4;

    if (CORES > 1) {
        std::thread threads[CORES];
        Garfield::MediumMagboltz* gasObjects[CORES];

        double sectionSize = (eMax - eMin) / CORES;
        int runsPerCore = eN / CORES;
        double pressure = 760.0;
        for (int i = 0; i < CORES; i++) {
            double begin = eMin + i * sectionSize;
            double end = begin + sectionSize;



            threads[i] = std::thread(
                [&gasObjects, i,
                    begin, end, runsPerCore,
                    gas1, gasProp1, gas2, gasProp2,
                    gas3, gasProp3, gas4, gasProp4,
                    gas5, gasProp5, gas6, gasProp6,
                    temperature, pressure, filename]
            {
                gasObjects[i] = CreateGasInternal(begin, end, runsPerCore,
                    gas1, gasProp1, gas2, gasProp2,
                    gas3, gasProp3, gas4, gasProp4,
                    gas5, gasProp5, gas6, gasProp6,
                    temperature, pressure, filename, i == 0
                );
            });
        }

        for (int i = 0; i < CORES; i++) {
            threads[i].join();
        }

        // By now all threads are done.
        gasObjects[0]->WriteGasFile(filename);
        for (int i = 1; i < CORES; i++) {
            gasObjects[i]->MergeGasFile(filename, true);
        }

        for (int i = 0; i < CORES; i++) {
            delete gasObjects[i];
        }
    } else {
        Garfield::MediumMagboltz* gas = CreateGasInternal(
            eMin, eMax, eN,
            gas1, gasProp1,
            gas2, gasProp2,
            gas3, gasProp3,
            gas4, gasProp4,
            gas5, gasProp5,
            gas6, gasProp6,
            temperature, 760.0, filename,
            true
        );
        delete gas;
    }

    std::cout << "Gas Setup Finished." << std::endl;
}

int main(int argc, char *argv[]) {
    PrintVersion();

    if (argc == 1) {
        PrintHelp();
        return 0;
    }

    // Parsing options.
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            PrintHelp();
            return 0;
        }
        if (arg == "-v" || arg == "--verbose") {
            VERBOSE = true;
        } else if (arg == "-c" || arg == "--cores") {
            if (i == argc - 1) {
                // User passed -c option but nothing afterwards.
                std::cerr << "No value passed for amount of cores. Resorting to default." << std::endl;
                std::cout << std::endl;
            } else {
                CORES = std::stoi(argv[i + 1]);
            }
        }
    }

    std::cout << "CORES = " << CORES << std::endl;
    std::cout << std::endl;

    // Parsing mode.
    if (strcmp(argv[1], "setup") == 0) {
        std::cout << "Entering setup mode..." << std::endl;
        std::cout << std::endl;
        SetupNewGas();
    } else if (strcmp(argv[1], "list") == 0) {
        std::cout << "Listing gases..." << std::endl;
        std::cout << std::endl;
        Garfield::MediumMagboltz::PrintGases();
    } else if (strcmp(argv[1], "create") == 0) {

    } else {
        std::cout << "Unknown mode: \"" << argv[1] << "\"" << std::endl;
        std::cout << "Use -h or --help to see program usage." << std::endl;
        return 1;
    }

    return 0;
}
