#include "../include/TrajectoryCalculator.h"
#include <iostream>
#include <vector>

int main() {
    TrajectoryCalculator calculator;
    calculator.CalculateData(25.0, 45.0, 0.05, 0.1, 9.81, 2.0, 0.0, 1.225);
    vector<double> xAxis = calculator.getXAxisCoordinates();
    vector<double> yAxis = calculator.getYAxisCoordinates();

    for (size_t i = 0; i < xAxis.size(); ++i) {
        cout << i << ". " << xAxis[i] << " - " << yAxis[i] << endl;
    }

    return 0;
}