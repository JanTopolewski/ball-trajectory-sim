#include "../include/TrajectoryCalculator.h"
#include "../include/FilesManager.h"
#include "../include/Simulation.h"
#include <iostream>
#include <vector>
#include <filesystem>

using namespace std;

int main() {
    TrajectoryCalculator calculator;
    calculator.CalculateData(50.0, 45.0, 0.05, 2.0, 9.81, 5.0, 180.0, 1.225, 0.0);
    vector<double> xAxis = calculator.getXAxisCoordinates();
    vector<double> yAxis = calculator.getYAxisCoordinates();

    for (size_t i = 0; i < xAxis.size(); ++i) {
        cout << i << ". " << xAxis[i] << " - " << yAxis[i] << endl;
    }
    
    string resultWarning = calculator.getWarning();

    calculator.CalculateData(25.0, 45.0, 0.05, 0.1, 9.81, 0.0, 0.0, 0.0, 0.0);
    vector<double> xAxis2 = calculator.getXAxisCoordinates();
    vector<double> yAxis2 = calculator.getYAxisCoordinates();

    for (size_t i = 0; i < xAxis2.size(); ++i) {
        cout << i << ". " << xAxis2[i] << " - " << yAxis2[i] << endl;
    }

    cout << "Do you want to save? (Y/n)";
    char answer;
    cin >> answer;

    if (answer == 'y')
    {
        Simulation *simulation = new Simulation({50.0, 45.0, 0.05, 2.0, 9.81, 5.0, 180.0, 1.225, 0.0, xAxis, yAxis, resultWarning});

        cout << "Name a file: ";
        string fileName;
        cin >> fileName;

        if(filesystem::exists("data/"+fileName+".bin"))
        {
            cout << "Do you want to overwrite it? (Y/n)";
            char answer;
            cin >> answer;
            if (answer == 'y')
            {
                FilesManager *fileManager = new FilesManager();
                fileManager->saveSimulationData(simulation, fileName);
                delete fileManager;
            }
        }
        else
        {
            FilesManager *fileManager = new FilesManager();
            fileManager->saveSimulationData(simulation, fileName);
            delete fileManager;
        }
        // delete simulation;
    }

    return 0;
}