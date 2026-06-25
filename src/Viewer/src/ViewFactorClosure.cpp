#include <cmath>
#include <map>

#include "ViewFactorClosure.hpp"


using namespace FenestrationCommon;

// VIEWR works on the symmetric "coefficient" form M(a,b) = F(a,b) * area_a (= F(b,a) * area_b by
// reciprocity); a row of M should sum to area_a, i.e. a view-factor row sums to one. The smoothing
// solves a small least-squares system per enclosure and applies a symmetric correction, so
// reciprocity is preserved. See VIEWR lines ~3905-3940 in conrad/viewer.for.
namespace Viewer
{
    namespace
    {
        // Indices of the segments in each enclosure.
        std::map<std::size_t, std::vector<std::size_t>>
          groupByEnclosure(const std::vector<std::size_t> & enclosureIds)
        {
            std::map<std::size_t, std::vector<std::size_t>> groups;
            for(std::size_t idx = 0; idx < enclosureIds.size(); ++idx)
            {
                groups[enclosureIds[idx]].push_back(idx);
            }
            return groups;
        }

        bool allFinite(const std::vector<double> & values)
        {
            for(const auto value : values)
            {
                if(!std::isfinite(value))
                {
                    return false;
                }
            }
            return true;
        }

        // Smooths one closed enclosure in place. indices are the global segment indices of this
        // enclosure; matrix is the full view-factor matrix; areas are the per-segment lengths.
        void smoothEnclosure(SquareMatrix & matrix,
                             const std::vector<double> & areas,
                             const std::vector<std::size_t> & indices)
        {
            const auto count = indices.size();

            // Coefficient submatrix M(a,b) = F(a,b) * area_a (symmetric).
            SquareMatrix coefficients{count};
            for(std::size_t row = 0; row < count; ++row)
            {
                for(std::size_t col = 0; col < count; ++col)
                {
                    coefficients(row, col) =
                      matrix(indices[row], indices[col]) * areas[indices[row]];
                }
            }

            // Residual r_a = area_a - sum_b M(a,b), and the normal-equation matrix
            // C(a,b) = M(a,b)^2 with C(a,a) = M(a,a)^2 + sum_b M(a,b)^2.
            std::vector<double> residual(count, 0.0);
            SquareMatrix normalEquations{count};
            for(std::size_t row = 0; row < count; ++row)
            {
                double rowSum = 0.0;
                double sumOfSquares = 0.0;
                for(std::size_t col = 0; col < count; ++col)
                {
                    const auto value = coefficients(row, col);
                    rowSum += value;
                    sumOfSquares += value * value;
                    normalEquations(row, col) = value * value;
                }
                residual[row] = areas[indices[row]] - rowSum;
                normalEquations(row, row) =
                  coefficients(row, row) * coefficients(row, row) + sumOfSquares;
            }

            // Solve C x = r. The no-pivot inverse can fail on a near-singular (e.g. bipartite
            // blocking) enclosure; if so, leave this enclosure unsmoothed rather than corrupt it.
            const auto multipliers = normalEquations.inverse() * residual;
            if(!allFinite(multipliers))
            {
                return;
            }

            // Symmetric correction M(a,b) += (x_a + x_b) * M(a,b)^2, written back as view factors.
            for(std::size_t row = 0; row < count; ++row)
            {
                for(std::size_t col = 0; col < count; ++col)
                {
                    const auto value = coefficients(row, col);
                    const auto corrected = value + (multipliers[row] + multipliers[col]) * value * value;
                    matrix(indices[row], indices[col]) = corrected / areas[indices[row]];
                }
            }
        }
    }   // namespace

    SquareMatrix smoothViewFactors(const SquareMatrix & viewFactors,
                                   const std::vector<double> & areas,
                                   const std::vector<std::size_t> & enclosureIds,
                                   const std::set<std::size_t> & openEnclosureIds)
    {
        auto result = viewFactors;
        for(const auto & [enclosureId, indices] : groupByEnclosure(enclosureIds))
        {
            // Open enclosures keep their deficit; a single segment has nothing to smooth.
            if(openEnclosureIds.contains(enclosureId) || indices.size() < 2)
            {
                continue;
            }
            smoothEnclosure(result, areas, indices);
        }
        return result;
    }
}   // namespace Viewer
