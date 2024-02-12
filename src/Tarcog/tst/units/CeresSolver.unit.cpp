#include <memory>
#include <gtest/gtest.h>

#include "ceres/ceres.h"

// Inspired by http://ceres-solver.org/nnls_tutorial.html#hello-world

struct CostFunctor {
   template <typename T>
   bool operator()(const T* const x, T* residual) const {
     residual[0] = 10.0 - x[0];
     return true;
   }
};

class CeresSolver : public testing::Test
{
private:

protected:
    void SetUp() override
    {
    }

public:
};

TEST_F(CeresSolver, ProofOfConcept)
{
    SCOPED_TRACE("Begin Test: Hello World");

    // The variable to solve for with its initial value.
    double initial_x = 5.0;
    double x = initial_x;

    // Build the problem.
    ceres::Problem problem;

    // Set up the only cost function (also known as residual). This uses
    // auto-differentiation to obtain the derivative (jacobian).
    ceres::CostFunction* cost_function =
        new ceres::AutoDiffCostFunction<CostFunctor, 1, 1>(new CostFunctor);
    problem.AddResidualBlock(cost_function, nullptr, &x);

    // Run the solver!
    ceres::Solver::Options options;
    options.linear_solver_type = ceres::DENSE_QR;
    options.minimizer_progress_to_stdout = true;
    ceres::Solver::Summary summary;
    Solve(options, &problem, &summary);

    EXPECT_NEAR(9.999999, x, 1e-4);
}