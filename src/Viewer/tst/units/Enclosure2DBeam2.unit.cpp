#include <memory>
#include <gtest/gtest.h>

#include <memory>

#include "WCEViewer.hpp"
#include "WCECommon.hpp"


using namespace Viewer;
using namespace FenestrationCommon;

class TestEnclosure2DBeam2 : public testing::Test
{
private:
    std::shared_ptr<CGeometry2DBeam> m_Enclosures2DBeam;

protected:
    void SetUp() override
    {
        m_Enclosures2DBeam = std::make_shared<CGeometry2DBeam>();

        //////////////////////////////////////////////////////////////////////
        ///  Enclosure 1
        //////////////////////////////////////////////////////////////////////
        CGeometry2D aEnclosure1;

        // Segment 1_1
        CPoint2D aStartPoint1_1{8, 12};
        CPoint2D aEndPoint1_1{9, 17};

        CViewSegment2D aSegment1_1(aStartPoint1_1, aEndPoint1_1);
        aEnclosure1.appendSegment(aSegment1_1);

        // Segment 1_2
        CPoint2D aStartPoint1_2{9, 17};
        CPoint2D aEndPoint1_2{7, 10};

        CViewSegment2D aSegment1_2(aStartPoint1_2, aEndPoint1_2);
        aEnclosure1.appendSegment(aSegment1_2);

        // Segment 1_3
        CPoint2D aStartPoint1_3{7, 10};
        CPoint2D aEndPoint1_3{4, 17};

        CViewSegment2D aSegment1_3(aStartPoint1_3, aEndPoint1_3);
        aEnclosure1.appendSegment(aSegment1_3);

        // Segment 1_4
        CPoint2D aStartPoint1_4{4, 17};
        CPoint2D aEndPoint1_4{3, 12};

        CViewSegment2D aSegment1_4(aStartPoint1_4, aEndPoint1_4);
        aEnclosure1.appendSegment(aSegment1_4);

        m_Enclosures2DBeam->appendGeometry2D(aEnclosure1);

        //////////////////////////////////////////////////////////////////////
        ///  Enclosure 2
        //////////////////////////////////////////////////////////////////////
        CGeometry2D aEnclosure2;

        // Segment 2_1
        CPoint2D aStartPoint2_1{9, 13};
        CPoint2D aEndPoint2_1{8, 2};

        CViewSegment2D aSegment2_1(aStartPoint2_1, aEndPoint2_1);
        aEnclosure2.appendSegment(aSegment2_1);

        // Segment 2_2
        CPoint2D aStartPoint2_2{8, 2};
        CPoint2D aEndPoint2_2{5, 10};

        CViewSegment2D aSegment2_2(aStartPoint2_2, aEndPoint2_2);
        aEnclosure2.appendSegment(aSegment2_2);

        // Segment 2_3
        CPoint2D aStartPoint2_3{5, 10};
        CPoint2D aEndPoint2_3{3, 3};

        CViewSegment2D aSegment2_3(aStartPoint2_3, aEndPoint2_3);
        aEnclosure2.appendSegment(aSegment2_3);

        m_Enclosures2DBeam->appendGeometry2D(aEnclosure2);
    }

public:
    std::shared_ptr<CGeometry2DBeam> getEnclosure() const
    {
        return m_Enclosures2DBeam;
    };
};

TEST_F(TestEnclosure2DBeam2, Enclosure2DBeam1)
{
    SCOPED_TRACE("Begin Test: 2D Enclosure - Test incoming beam view factors and direct-direct "
                 "component (45 deg).");

    std::shared_ptr<CGeometry2DBeam> aEnclosure = getEnclosure();
    double profileAngle = 45;

    /////////////////////////////////////////////////////////////
    //  Incoming beam
    /////////////////////////////////////////////////////////////
    Side aSide = Side::Front;

    const auto aViewFactors =
      aEnclosure->beamViewFactors(profileAngle, aSide);

    EXPECT_EQ(0, aEnclosure->directToDirect(profileAngle, aSide));

    size_t correctSize = 2;

    EXPECT_EQ(correctSize, aViewFactors.size());

    // Create correct results
    std::vector<BeamViewFactor> correctResults;

    size_t enclosureIndex = 0;
    size_t segmentIndex = 2;
    double viewFactor = 4.0 / 9.0;
    double percentHit = 0.4;
    BeamViewFactor aVF1 = BeamViewFactor(enclosureIndex, segmentIndex, viewFactor, percentHit);
    correctResults.push_back(aVF1);

    enclosureIndex = 1;
    segmentIndex = 2;
    viewFactor = 5.0 / 9.0;
    percentHit = 1;
    BeamViewFactor aVF2 = BeamViewFactor(enclosureIndex, segmentIndex, viewFactor, percentHit);
    correctResults.push_back(aVF2);

    for(size_t i = 0; i < correctResults.size(); ++i)
    {
        EXPECT_EQ(correctResults[i].enclosureIndex, aViewFactors[i].enclosureIndex);
        EXPECT_EQ(correctResults[i].segmentIndex, aViewFactors[i].segmentIndex);
        EXPECT_NEAR(correctResults[i].value, aViewFactors[i].value, 1e-6);
        EXPECT_NEAR(correctResults[i].percentHit, aViewFactors[i].percentHit, 1e-6);
    }
}

TEST_F(TestEnclosure2DBeam2, Enclosure2DBeam2)
{
    SCOPED_TRACE("Begin Test: 2D Enclosure - Test incoming beam view factors and direct-direct "
                 "component (0 deg).");

    std::shared_ptr<CGeometry2DBeam> aEnclosure = getEnclosure();
    double profileAngle = 0;

    /////////////////////////////////////////////////////////////
    //  Incoming beam
    /////////////////////////////////////////////////////////////
    Side aSide = Side::Front;

    const auto aViewFactors =
      aEnclosure->beamViewFactors(profileAngle, aSide);

    EXPECT_EQ(0, aEnclosure->directToDirect(profileAngle, aSide));

    size_t correctSize = 2;

    EXPECT_EQ(correctSize, aViewFactors.size());

    // Create correct results
    std::vector<BeamViewFactor> correctResults;

    size_t enclosureIndex = 0;
    size_t segmentIndex = 2;
    double viewFactor = 2.0 / 9.0;
    double percentHit = 2.0 / 7.0;
    BeamViewFactor aVF1 = BeamViewFactor(enclosureIndex, segmentIndex, viewFactor, percentHit);
    correctResults.push_back(aVF1);

    enclosureIndex = 1;
    segmentIndex = 2;
    viewFactor = 7.0 / 9.0;
    percentHit = 1;
    BeamViewFactor aVF2 = BeamViewFactor(enclosureIndex, segmentIndex, viewFactor, percentHit);
    correctResults.push_back(aVF2);

    for(size_t i = 0; i < correctResults.size(); ++i)
    {
        EXPECT_EQ(correctResults[i].enclosureIndex, aViewFactors[i].enclosureIndex);
        EXPECT_EQ(correctResults[i].segmentIndex, aViewFactors[i].segmentIndex);
        EXPECT_NEAR(correctResults[i].value, aViewFactors[i].value, 1e-6);
        EXPECT_NEAR(correctResults[i].percentHit, aViewFactors[i].percentHit, 1e-6);
    }
}

TEST_F(TestEnclosure2DBeam2, Enclosure2DBeam3)
{
    SCOPED_TRACE(
      "Begin Test: 2D Enclosure - Test beam view factors and direct-direct component (-45 deg).");

    std::shared_ptr<CGeometry2DBeam> aEnclosure = getEnclosure();
    double profileAngle = -45;

    /////////////////////////////////////////////////////////////
    //  Incoming beam
    /////////////////////////////////////////////////////////////
    Side aSide = Side::Front;

    const auto aViewFactors =
      aEnclosure->beamViewFactors(profileAngle, aSide);

    size_t correctSize = 1;

    EXPECT_EQ(correctSize, aViewFactors.size());

    // Create correct results
    std::vector<BeamViewFactor> correctResults;

    size_t enclosureIndex = 1;
    size_t segmentIndex = 2;
    double viewFactor = 1;
    double percentHit = 1;
    BeamViewFactor aVF1 = BeamViewFactor(enclosureIndex, segmentIndex, viewFactor, percentHit);
    correctResults.push_back(aVF1);

    for(size_t i = 0; i < correctResults.size(); ++i)
    {
        EXPECT_EQ(correctResults[i].enclosureIndex, aViewFactors[i].enclosureIndex);
        EXPECT_EQ(correctResults[i].segmentIndex, aViewFactors[i].segmentIndex);
        EXPECT_NEAR(correctResults[i].value, aViewFactors[i].value, 1e-6);
        EXPECT_NEAR(correctResults[i].percentHit, aViewFactors[i].percentHit, 1e-6);
    }
}
