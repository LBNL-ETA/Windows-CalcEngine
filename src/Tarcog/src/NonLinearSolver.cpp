#include <cassert>
#include <cmath>
#include <algorithm>

#include <WCECommon.hpp>

#include "NonLinearSolver.hpp"
#include "TarcogConstants.hpp"
#include "IGU.hpp"


namespace Tarcog::ISO15099
{
    CNonLinearSolver::CNonLinearSolver(CIGU & t_IGU, const size_t numberOfIterations) :
        m_IGU(t_IGU),
        m_QBalance(m_IGU),
        m_Tolerance(IterationConstants::CONVERGENCE_TOLERANCE),
        m_Iterations(numberOfIterations),
        m_RelaxParam(IterationConstants::RELAXATION_PARAMETER_MAX),
        m_SolutionTolerance(0)
    {}

    double CNonLinearSolver::calculateTolerance(std::vector<double> const & t_Solution) const
    {
        assert(t_Solution.size() == m_IGUState.size());
        auto aError = std::abs(t_Solution[0] - m_IGUState[0]);
        for(size_t i = 1; i < m_IGUState.size(); ++i)
        {
            aError = std::max(aError, std::abs(t_Solution[i] - m_IGUState[i]));
        }
        return aError;
    }

    void CNonLinearSolver::estimateNewState(std::vector<double> const & t_Solution)
    {
        assert(t_Solution.size() == m_IGUState.size());
        for(size_t i = 0; i < m_IGUState.size(); ++i)
        {
            m_IGUState[i] = m_RelaxParam * t_Solution[i] + (1 - m_RelaxParam) * m_IGUState[i];
        }
    }

    void CNonLinearSolver::setTolerance(double const t_Tolerance)
    {
        m_Tolerance = t_Tolerance;
    }

    size_t CNonLinearSolver::getNumOfIterations() const
    {
        return m_Iterations;
    }

    void CNonLinearSolver::solve()
    {
        m_IGUState = m_IGU.getState();
        std::vector<double> initialState(m_IGUState);
        std::vector<double> bestSolution(m_IGUState.size());
        auto achievedTolerance = std::numeric_limits<double>::max();
        m_SolutionTolerance = achievedTolerance;

        m_Iterations = 0;
        bool iterate = true;

        while(iterate)
        {
            ++m_Iterations;
            std::vector<double> aSolution = m_QBalance.calcBalanceMatrix();

            m_IGU.precalculateLayerStates();
            achievedTolerance = calculateTolerance(aSolution);

            estimateNewState(aSolution);

            m_IGU.setState(m_IGUState);

            m_IGU.updateDeflectionState();

            if(achievedTolerance < m_SolutionTolerance)
            {
                initialState = m_IGUState;
                m_SolutionTolerance = std::min(achievedTolerance, m_SolutionTolerance);
                bestSolution = m_IGUState;
            }

            if(m_Iterations > IterationConstants::NUMBER_OF_STEPS)
            {
                m_Iterations = 0;
                m_RelaxParam -= IterationConstants::RELAXATION_PARAMETER_STEP;

                m_IGU.setState(initialState);
                m_IGUState = initialState;
            }

            iterate = achievedTolerance > m_Tolerance;

            if(m_RelaxParam < IterationConstants::RELAXATION_PARAMETER_MIN)
            {
                iterate = false;
            }
        }
        m_IGUState = bestSolution;
    }

    double CNonLinearSolver::solutionTolerance() const
    {
        return m_SolutionTolerance;
    }

    bool CNonLinearSolver::isToleranceAchieved() const
    {
        return m_SolutionTolerance < m_Tolerance;
    }

}   // namespace Tarcog::ISO15099
