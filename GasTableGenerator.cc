//
// Este script gera tabela de gases para uma dada mistura em linha de comando.
//

#include <cstring>
#include <iostream>
#include <thread>
#include <unistd.h>
#include <sys/wait.h>

#include "Garfield/MediumMagboltz.hh"

using namespace std;
int CORES = 1;
bool VERBOSE = false;

void PrintVersion() {
    cout << "GasTableGenerator Version 0.1" << endl;
    cout << endl;
}

void PrintHelp() {
    cout << "Usage: gtgen mode [options]" << endl;
    cout << endl;
    cout << "This program manages Garfield++ gas table files from step-by-step command line setup."
                 " For gas file generation, it just runs Magboltz under the hood." << endl;
    cout << endl;
    cout << "Modes:" << endl;
    cout << "    create : Enters new gas table setup mode." << endl;
    cout << "    merge : Enters merging mode." << endl;
    cout << "    list : List available gases in Magboltz." << endl;
    cout << "------------------------------------" << endl;
    cout << "Options: (For multiple options, please input them separately, so no \"-vc\" for example)" << endl;
    cout << "    -h | --help : Shows this help." << endl;
    cout << "    -v | --verbose : Enables verbose output." << endl;
    cout << "    -c | --cores [n] : Specifies the amount of CPU cores to run Magboltz (default is 1)." << endl;
    cout << endl;
}

// Creates the gas table internally, regardless of how the parameters were passed.
void CreateGasInternal(
    const double eMin, const double eMax, const int eN,
    const string& gas1, const double gasProp1,
    const string& gas2, const double gasProp2,
    const string& gas3, const double gasProp3,
    const string& gas4, const double gasProp4,
    const string& gas5, const double gasProp5,
    const string& gas6, const double gasProp6,
    double temperature, double pressure,
    const string& filename
) {
    if (temperature < 0.0) {
        temperature = 0.0;
    }
    if (pressure < 0.0) {
        pressure = 0.0;
    }

    // Creating gas object.
    auto* gas = new Garfield::MediumMagboltz();

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

    gas->SetFieldGrid(eMin, eMax, eN, false);
    gas->GenerateGasTable(10, VERBOSE);
    gas->WriteGasFile(filename);

    delete gas;
}

void SetupNewGas() {
    string mixture, filename;
    double temperature = 0.0;

    cout << "Please define the gas mixture in the format: [gas1] [proportion1] [gas2] [proportion2] ..." << endl;
    cout << "It has to be made of at least 1 gas, and at most 6 gases, each with proportions in the [0, 1] range:" << endl;
    cout << "(Use 'gtgen list' to see a list of available gases and their aliases)" << endl;
    cout << endl;
    cout << ">> ";
    getline(cin, mixture);

    // ################
    // Parsing mixture.
    // ################

    if (mixture.empty()) {
        cerr << "Mixture is invalid. Aborting." << endl;
        exit(1);
    }

    // Declaring variables.
    string gas1, gas2, gas3, gas4, gas5, gas6;
    double gasProp1 = 0.0, gasProp2 = 0.0, gasProp3 = 0.0, gasProp4 = 0.0, gasProp5 = 0.0, gasProp6 = 0.0;

    string::size_type index = 0;
    int iter = 0;

    while (index != string::npos) {
        string::size_type nextIndex = mixture.find(' ', index);
        string substr = mixture.substr(index, nextIndex - index);

        switch (iter) {
            case 0: gas1 = substr; break;
            case 1: gasProp1 = stod(substr); break;
            case 2: gas2 = substr; break;
            case 3: gasProp2 = stod(substr); break;
            case 4: gas3 = substr; break;
            case 5: gasProp3 = stod(substr); break;
            case 6: gas4 = substr; break;
            case 7: gasProp4 = stod(substr); break;
            case 8: gas5 = substr; break;
            case 9: gasProp5 = stod(substr); break;
            case 10: gas6 = substr; break;
            case 11: gasProp6 = stod(substr); break;
            default:
                cerr << "Too many arguments. Ignoring." << endl;
                goto mixtureParseBreak;
        }

        // Updating iteration variables.
        iter++;
        if (nextIndex == string::npos) {
            break;
        }
        index = nextIndex + 1;
        if (index > mixture.length()) {
            break;
        }
    }
    mixtureParseBreak:

    cout << endl;
    cout << "I parsed the following gases from your input:" << endl;
    cout << "'" << gas1 << "' '" << gas2 << "' '" << gas3 << "' '" << gas4 << "' '" << gas5 << "' '" << gas6 << "'" << endl;
    cout << "And proportions:" << endl;
    cout << gasProp1 << " " << gasProp2 << " " << gasProp3 << " " << gasProp4<< " " << gasProp5 << " " << gasProp6 << endl;

    // ######################
    // Other setup arguments.
    // ######################

    // Thermal motion.
    cout << endl;
    cout << "Enable thermal motion? (\"Y\" for yes, anything else for no)" << endl;
    string answer;
    cout << endl;
    cout << ">> ";
    getline(cin, answer);

    if (answer == "Y" || answer == "y") {
        cout << endl;
        cout << "Define the temperature of the mixture. "
                     "Add 'C' right next to the number for Celsius, and 'K' (or nothing) for Kelvin." << endl;
        cout << endl;
        cout << ">> ";
        getline(cin, answer);
        if (answer.at(answer.length() - 1) == 'C' || answer.at(answer.length() - 1) == 'c') {
            temperature = stod(answer.erase(answer.length() - 1)) + 273.15;
        } else {
            temperature = stod(answer);
        }
    }

    // Filename.
    cout << endl;
    cout << "Choose a name for the new gas file:" << endl;
    cout << endl;
    cout << ">> ";
    getline(cin, filename);
    cout << endl;

    // Appending ".gas" file type if user did not do so.
    if (filename.length() > 4 && filename.substr(filename.length() - 4) != ".gas") {
        filename.append(".gas");
    }

    // ######################
    // Setting up gas object.
    // ######################

    double eMin = 5000.0;
    double eMax = 9000.0;
    int eN = 8;
    double pressure = 760.0;

    if (CORES > 1) {
        double sectionSize = (eMax - eMin) / CORES;
        int runsPerCore = eN / CORES;

        vector<__pid_t> pids;

        for (int i = 0; i < CORES; i++) {
            double begin = eMin + i * sectionSize;
            double end = begin + sectionSize / 2;

            pid_t child = fork();

            if (child == -1) {
                cerr << "Fork failed." << endl;
            } else if (child == 0) {
                // We're the child process.
                cout << "Running Magboltz from Process " << getpid() << "." << endl;

                // Creating gas table segment from this process.
                CreateGasInternal(
                    begin, end, runsPerCore,
                    gas1, gasProp1,
                    gas2, gasProp2,
                    gas3, gasProp3,
                    gas4, gasProp4,
                    gas5, gasProp5,
                    gas6, gasProp6,
                    temperature, pressure, filename + "temp" + to_string(i)
                );

                cout << "Process " << getpid() << " finished Gas Table generation." << endl;
                exit(0);
            } else {
                // We're the parent process.
                cout << "Created Process " << child << "." << endl;
                pids.push_back(child);
            }
        }

        for (__pid_t pid : pids) {
            int status = 0;
            pid_t finishedPID = waitpid(pid, &status, 0);
            if (finishedPID == -1) {
                cerr << "Error while waiting for process " << pid << "." << endl;
            } else if (WIFEXITED(status)) {
                cout << "Process " << finishedPID << " finished with exit status " << WEXITSTATUS(status) << endl;
            } else {
                cout << "Process " << finishedPID << " finished abnormally." << endl;
            }
        }

        cout << "All child processes finished work. Merging gas files." << endl;

        Garfield::MediumMagboltz gas;

        for (int i = 0; i < CORES; i++) {
            string tempFile = filename + "temp" + to_string(i);
            if (i == 0) {
                gas.LoadGasFile(tempFile);
            } else {
                gas.MergeGasFile(tempFile, true);
            }
            //remove(tempFile.c_str());
        }

        gas.WriteGasFile(filename);
    } else {
        CreateGasInternal(
            eMin, eMax, eN,
            gas1, gasProp1,
            gas2, gasProp2,
            gas3, gasProp3,
            gas4, gasProp4,
            gas5, gasProp5,
            gas6, gasProp6,
            temperature, pressure, filename
        );
    }

    cout << "Gas Setup Finished." << endl;
}

void MergeGasFiles() {
    cout << "Write the names of all the files you want to merge in a list with the format: [filename1] [filename2] [...]" << endl;
    cout << "Note: All gas files must be of the same mixture." << endl;
    cout << endl;

    string files;
    cout << ">> ";
    getline(cin, files);
    cout << endl;

    cout << "Now, define the name of the result file (it can be one of the previously given files):" << endl;
    cout << endl;
    string resultName;
    cout << ">> ";
    getline(cin, resultName);
    cout << endl;

    cout << "Delete previous files? (\"Y\" for yes, anything else for no)" << endl;
    cout << endl;
    string deleteFiles;
    cout << ">> ";
    getline(cin, deleteFiles);
    cout << endl;

    // Actually performing merge.
    Garfield::MediumMagboltz gas;

    string::size_type index = 0;
    bool removeOld = (deleteFiles == "Y" || deleteFiles == "y");
    int iter = 0;
    while (index != string::npos) {

        string::size_type nextIndex;
        string filename;
        bool isInQuotes = files[index] == '\"';
        if (isInQuotes) {
            nextIndex = files.find('\"', index + 1);
            filename = files.substr(index + 1, nextIndex - index - 1);
        } else {
            nextIndex = files.find(' ', index);
            filename = files.substr(index, nextIndex - index);
        }

        cout << "Merging '" << filename << "' to gas table..." << endl;
        if (iter == 0) {
            if (gas.LoadGasFile(filename)) {
                cout << "Successfully loaded first gas file." << endl;
            } else {
                cout << "Failed to load gas file." << endl;
            }
        } else {
            if (gas.MergeGasFile(filename, false)) {
                cout << "Successfully merged gas file." << endl;
            } else {
                cout << "Error while merging gas file." << endl;
            }
        }

        if (removeOld) {
            cout << "Removing old gas file: " << filename << endl;
            remove(filename.c_str());
        }

        // Updating iteration variables.
        if (nextIndex == string::npos) {
            break;
        }
        if (isInQuotes) {
            index = nextIndex + 2;
        } else {
            index = nextIndex + 1;
        }
        iter++;
        if (index > files.length()) {
            break;
        }
    }

    cout << "Writing final gas table to '" << resultName << "'..." << endl;

    gas.EnableDebugging();
    if (gas.WriteGasFile(resultName)) {
        cout << "Successfully wrote merged gas file." << endl;
    } else {
        cout << "Failed to write merged gas file." << endl;
    }

    cout << "Gas file merging finished." << endl;
}

int main(int argc, char *argv[]) {
    PrintVersion();

    if (argc == 1) {
        PrintHelp();
        return 0;
    }

    // Parsing options.
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            PrintHelp();
            return 0;
        }
        if (arg == "-v" || arg == "--verbose") {
            VERBOSE = true;
        } else if (arg == "-c" || arg == "--cores") {
            if (i == argc - 1) {
                // User passed -c option but nothing afterwards.
                cerr << "No value passed for amount of cores. Resorting to default." << endl;
                cout << endl;
            } else {
                CORES = stoi(argv[i + 1]);
            }
        }
    }

    cout << "CORES = " << CORES << endl;
    cout << endl;

    // Parsing mode.
    if (strcmp(argv[1], "create") == 0) {
        cout << "Entering create mode..." << endl;
        cout << endl;
        SetupNewGas();
    } else if (strcmp(argv[1], "list") == 0) {
        cout << "Listing gases..." << endl;
        cout << endl;
        Garfield::MediumMagboltz::PrintGases();
    } else if (strcmp(argv[1], "merge") == 0) {
        cout << "Entering merge mode..." << endl;
        cout << endl;
        MergeGasFiles();
    } else {
        cout << "Unknown mode: \"" << argv[1] << "\"" << endl;
        cout << "Use -h or --help to see program usage." << endl;
        return 1;
    }

    return 0;
}
