#pragma once

#include <vector>

#include "HeatFlowBalance.hpp"
#include "IGU.hpp"

namespace Tarcog::ISO15099
{

    class CNonLinearSolver
    {
    public:
        explicit CNonLinearSolver(CIGU & t_IGU, size_t numberOfIterations = 0u);

        // sets tolerance for solution
        void setTolerance(double t_Tolerance);

        // returns number of iterations for current solution.
        [[nodiscard]] size_t getNumOfIterations() const;

        // run the solver
        void solve();

        [[nodiscard]] double solutionTolerance() const;
        [[nodiscard]] bool isToleranceAchieved() const;

    private:
        // Core steps extracted for clarity:
        void initialize();
        double performIteration();
        void updateBestSolution(double achievedTolerance);
        void resetIfNeeded();
        [[nodiscard]] bool shouldContinue(double achievedTolerance) const;

        // low-level helpers from original
        [[nodiscard]] double calculateTolerance(const std::vector<double> & t_Solution) const;
        void estimateNewState(const std::vector<double> & t_Solution);

        // solver state
        CIGU & m_IGU;
        CHeatFlowBalance m_QBalance;
        std::vector<double> m_IGUState;
        std::vector<double> m_initialState;
        std::vector<double> m_bestSolution;
        double m_Tolerance;
        size_t m_Iterations;
        double m_RelaxParam;
        double m_SolutionTolerance;
    };

}   // namespace Tarcog::ISO15099
