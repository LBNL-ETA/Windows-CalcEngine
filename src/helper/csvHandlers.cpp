#include <iomanip>

#include "csvHandlers.hpp"

namespace Helper
{
    void writeVectorToCSV(const std::vector<std::vector<double>> & data,
                          const std::string & filename)
    {
        std::ofstream file(filename);
        if(!file.is_open())
        {
            throw std::runtime_error("Could not open file to write");
        }

        file << std::fixed << std::setprecision(9); // Set precision to 9 decimal places

        for(const auto & row : data)
        {
            for(size_t i = 0; i < row.size(); ++i)
            {
                file << row[i];
                if(i < row.size() - 1)
                {
                    file << ",";   // Add comma if not the last element
                }
            }
            file << "\n";
        }
        file.close();
    }

    std::vector<std::vector<double>> readVectorFromCSV(const std::string & filename)
    {
        std::ifstream file(filename);
        if(!file.is_open())
        {
            throw std::runtime_error("Could not open file to read");
        }

        std::vector<std::vector<double>> data;
        std::string line;

        while(std::getline(file, line))
        {
            std::vector<double> row;
            std::stringstream ss(line);
            std::string value;

            while(std::getline(ss, value, ','))
            {
                row.push_back(std::stod(value));   // Convert string to double and add to row
            }
            data.push_back(row);
        }
        file.close();
        return data;
    }
}   // namespace Helper
