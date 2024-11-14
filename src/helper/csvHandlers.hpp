#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>

namespace Helper
{
    // Function to write a 2D vector to a CSV file
    void writeMatrixToCSV(const std::vector<std::vector<double>> & data,
                          const std::string & filename);

    // Function to read a 2D vector from a CSV file
    std::vector<std::vector<double>> readMatrixFromCSV(const std::string & filename);
}   // namespace Helper