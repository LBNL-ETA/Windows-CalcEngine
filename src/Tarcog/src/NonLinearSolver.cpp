#include <cassert>
#include <limits>
#include <algorithm>

#include "NonLinearSolver.hpp"
#include "TarcogConstants.hpp"

namespace Tarcog::ISO15099
{

    CNonLinearSolver::CNonLinearSolver(CIGU & t_IGU, const size_t numberOfIterations) :
        m_IGU(t_IGU),
        m_QBalance(m_IGU),
        m_Tolerance(IterationConstants::CONVERGENCE_TOLERANCE),
        m_Iterations(numberOfIterations),
        m_RelaxParam(IterationConstants::RELAXATION_PARAMETER_MAX),
        m_SolutionTolerance(std::numeric_limits<double>::infinity())
    {}

    void CNonLinearSolver::initialize()
    {
        m_IGUState = m_IGU.getState();
        m_initialState = m_IGUState;
        m_bestSolution = m_IGUState;
        m_SolutionTolerance = std::numeric_limits<double>::infinity();
        m_Iterations = 0;
        m_RelaxParam = IterationConstants::RELAXATION_PARAMETER_MAX;
    }

    double CNonLinearSolver::performIteration()
    {
        // 1) Compute candidate solution
        auto aSolution = m_QBalance.calcBalanceMatrix();

        // 2) Precalculate and measure tolerance
        m_IGU.precalculateLayerStates();
        double tol = calculateTolerance(aSolution);

        // 3) Apply relaxation update
        estimateNewState(aSolution);
        m_IGU.setState(m_IGUState);
        m_IGU.updateDeflectionState();

        return tol;
    }

    void CNonLinearSolver::updateBestSolution(double achievedTolerance)
    {
        if(achievedTolerance < m_SolutionTolerance)
        {
            m_initialState = m_IGUState;
            m_SolutionTolerance = achievedTolerance;
            m_bestSolution = m_IGUState;
        }
    }

    void CNonLinearSolver::resetIfNeeded()
    {
        if(m_Iterations > IterationConstants::NUMBER_OF_STEPS)
        {
            m_Iterations = 0;
            m_RelaxParam -= IterationConstants::RELAXATION_PARAMETER_STEP;
            m_IGU.setState(m_initialState);
            m_IGUState = m_initialState;
        }
    }

    bool CNonLinearSolver::shouldContinue(double achievedTolerance) const
    {
        return (achievedTolerance > m_Tolerance)
               && (m_RelaxParam >= IterationConstants::RELAXATION_PARAMETER_MIN);
    }

    void CNonLinearSolver::solve()
    {
        initialize();

        while(true)
        {
            ++m_Iterations;
            double tol = performIteration();
            updateBestSolution(tol);
            resetIfNeeded();
            if(!shouldContinue(tol))
                break;
        }

        m_IGUState = m_bestSolution;
        m_IGU.setState(m_bestSolution);
    }

    void CNonLinearSolver::setTolerance(double const t_Tolerance)
    {
        m_Tolerance = t_Tolerance;
    }

    size_t CNonLinearSolver::getNumOfIterations() const
    {
        return m_Iterations;
    }

    double CNonLinearSolver::solutionTolerance() const
    {
        return m_SolutionTolerance;
    }

    bool CNonLinearSolver::isToleranceAchieved() const
    {
        return m_SolutionTolerance < m_Tolerance;
    }

    double CNonLinearSolver::calculateTolerance(const std::vector<double> & t_Solution) const
    {
        assert(t_Solution.size() == m_IGUState.size());
        double err = std::abs(t_Solution[0] - m_IGUState[0]);
        for(size_t i = 1; i < m_IGUState.size(); ++i)
        {
            err = std::max(err, std::abs(t_Solution[i] - m_IGUState[i]));
        }
        return err;
    }

    void CNonLinearSolver::estimateNewState(const std::vector<double> & t_Solution)
    {
        assert(t_Solution.size() == m_IGUState.size());
        for(size_t i = 0; i < m_IGUState.size(); ++i)
        {
            m_IGUState[i] = m_RelaxParam * t_Solution[i] + (1 - m_RelaxParam) * m_IGUState[i];
        }
    }

}   // namespace Tarcog::ISO15099
