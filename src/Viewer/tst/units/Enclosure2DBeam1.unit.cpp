#include <gtest/gtest.h>
#include <memory>

#include <memory>

#include "WCECommon.hpp"
#include "WCEViewer.hpp"

using namespace Viewer;
using namespace FenestrationCommon;

class TestEnclosure2DBeam1 : public testing::Test
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

        // Segment 1
        CPoint2D aStartPoint1{3, 2};
        CPoint2D aEndPoint1{5, 5};

        CViewSegment2D aSegment1(aStartPoint1, aEndPoint1);
        aEnclosure1.appendSegment(aSegment1);

        // Segment 2
        CPoint2D aStartPoint2{5, 5};
        CPoint2D aEndPoint2{8, 4};

        CViewSegment2D aSegment2(aStartPoint2, aEndPoint2);
        aEnclosure1.appendSegment(aSegment2);

        // Segment 3
        CPoint2D aStartPoint3{8, 4};
        CPoint2D aEndPoint3{9, 9};

        CViewSegment2D aSegment3(aStartPoint3, aEndPoint3);
        aEnclosure1.appendSegment(aSegment3);

        m_Enclosures2DBeam->appendGeometry2D(aEnclosure1);

        //////////////////////////////////////////////////////////////////////
        ///  Enclosure 2
        //////////////////////////////////////////////////////////////////////
        CGeometry2D aEnclosure2;

        // Segment 4
        CPoint2D aStartPoint4{3, 10};
        CPoint2D aEndPoint4{7, 11};

        CViewSegment2D aSegment4(aStartPoint4, aEndPoint4);
        aEnclosure2.appendSegment(aSegment4);

        // Segment 5
        CPoint2D aStartPoint5{7, 11};
        CPoint2D aEndPoint5{6, 14};

        CViewSegment2D aSegment5(aStartPoint5, aEndPoint5);
        aEnclosure2.appendSegment(aSegment5);

        // Segment 6
        CPoint2D aStartPoint6{6, 14};
        CPoint2D aEndPoint6{12, 16};

        CViewSegment2D aSegment6(aStartPoint6, aEndPoint6);
        aEnclosure2.appendSegment(aSegment6);

        m_Enclosures2DBeam->appendGeometry2D(aEnclosure2);
    }

public:
    std::shared_ptr<CGeometry2DBeam> getEnclosure() const
    {
        return m_Enclosures2DBeam;
    };
};

TEST_F(TestEnclosure2DBeam1, Enclosure2DBeam1)
{
    SCOPED_TRACE("Begin Test: 2D Enclosure - Test incoming beam view factors and "
                 "direct-direct component (45 deg).");

    std::shared_ptr<CGeometry2DBeam> aEnclosure = getEnclosure();
    double profileAngle = 45;

    /////////////////////////////////////////////////////////////
    ///  Incoming beam
    /////////////////////////////////////////////////////////////

    Side aSide = Side::Front;

    const auto aViewFactors =
      aEnclosure->beamViewFactors(profileAngle, aSide);

    EXPECT_NEAR(0.5, aEnclosure->directToDirect(profileAngle, aSide), 1e-6);

    size_t correctSize = 2;

    EXPECT_EQ(correctSize, aViewFactors.size());

    // Create correct results
    std::vector<BeamViewFactor> correctResults;

    size_t enclosureIndex = 1;
    size_t segmentIndex = 0;
    double viewFactor = 0.375;
    double percentHit = 1;
    BeamViewFactor aVF1 = BeamViewFactor(enclosureIndex, segmentIndex, viewFactor, percentHit);
    correctResults.push_back(aVF1);

    enclosureIndex = 0;
    segmentIndex = 0;
    viewFactor = 0.125;
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

TEST_F(TestEnclosure2DBeam1, Enclosure2DBeam2)
{
    SCOPED_TRACE("Begin Test: 2D Enclosure - Test incoming beam view factors and "
                 "direct-direct component (0 deg).");

    std::shared_ptr<CGeometry2DBeam> aEnclosure = getEnclosure();
    double profileAngle = 0;

    /////////////////////////////////////////////////////////////
    //  Incoming beam
    /////////////////////////////////////////////////////////////

    Side aSide = Side::Front;

    const auto aViewFactors =
      aEnclosure->beamViewFactors(profileAngle, aSide);

    EXPECT_EQ(0.125, aEnclosure->directToDirect(profileAngle, aSide));

    size_t correctSize = 2;

    EXPECT_EQ(correctSize, aViewFactors.size());

    // Create correct results
    std::vector<BeamViewFactor> correctResults;

    size_t enclosureIndex = 0;
    size_t segmentIndex = 2;
    double viewFactor = 0.5;
    double percentHit = 0.8;
    BeamViewFactor aVF1 = BeamViewFactor(enclosureIndex, segmentIndex, viewFactor, percentHit);
    correctResults.push_back(aVF1);

    enclosureIndex = 0;
    segmentIndex = 0;
    viewFactor = 0.375;
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

TEST_F(TestEnclosure2DBeam1, Enclosure2DBeam3)
{
    SCOPED_TRACE("Begin Test: 2D Enclosure - Test incoming beam view factors and "
                 "direct-direct component (-45 deg).");

    std::shared_ptr<CGeometry2DBeam> aEnclosure = getEnclosure();
    double profileAngle = -45;

    /////////////////////////////////////////////////////////////
    //  Incoming beam
    /////////////////////////////////////////////////////////////

    Side aSide = Side::Front;

    const auto aViewFactors =
      aEnclosure->beamViewFactors(profileAngle, aSide);

    EXPECT_EQ(0.0, aEnclosure->directToDirect(profileAngle, aSide));

    size_t correctSize = 3;

    EXPECT_EQ(correctSize, aViewFactors.size());

    // Create correct results
    std::vector<BeamViewFactor> correctResults;

    size_t enclosureIndex = 0;
    size_t segmentIndex = 2;
    double viewFactor = 0.125;
    double percentHit = 0.16666666666666666;
    BeamViewFactor aVF1 = BeamViewFactor(enclosureIndex, segmentIndex, viewFactor, percentHit);
    correctResults.push_back(aVF1);

    enclosureIndex = 0;
    segmentIndex = 1;
    viewFactor = 0.25;
    percentHit = 1;
    BeamViewFactor aVF2 = BeamViewFactor(enclosureIndex, segmentIndex, viewFactor, percentHit);
    correctResults.push_back(aVF2);

    enclosureIndex = 0;
    segmentIndex = 0;
    viewFactor = 0.625;
    percentHit = 1;
    BeamViewFactor aVF3 = BeamViewFactor(enclosureIndex, segmentIndex, viewFactor, percentHit);
    correctResults.push_back(aVF3);

    for(size_t i = 0; i < correctResults.size(); ++i)
    {
        EXPECT_EQ(correctResults[i].enclosureIndex, aViewFactors[i].enclosureIndex);
        EXPECT_EQ(correctResults[i].segmentIndex, aViewFactors[i].segmentIndex);
        EXPECT_NEAR(correctResults[i].value, aViewFactors[i].value, 1e-6);
        EXPECT_NEAR(correctResults[i].percentHit, aViewFactors[i].percentHit, 1e-6);
    }
}

TEST_F(TestEnclosure2DBeam1, Enclosure2DBeam4)
{
    SCOPED_TRACE("Begin Test: 2D Enclosure - Test outgoing beam view factors and "
                 "direct-direct component (45 deg).");

    std::shared_ptr<CGeometry2DBeam> aEnclosure = getEnclosure();
    double profileAngle = 45;

    /////////////////////////////////////////////////////////////
    //  Outgoing beam
    /////////////////////////////////////////////////////////////

    Side aSide = Side::Back;

    const auto aViewFactors =
      aEnclosure->beamViewFactors(profileAngle, aSide);

    EXPECT_EQ(1, aEnclosure->directToDirect(profileAngle, aSide));

    size_t correctSize = 0;

    EXPECT_EQ(correctSize, aViewFactors.size());
}

TEST_F(TestEnclosure2DBeam1, Enclosure2DBeam5)
{
    SCOPED_TRACE("Begin Test: 2D Enclosure - Test outgoing beam view factors and "
                 "direct-direct component (0 deg).");

    std::shared_ptr<CGeometry2DBeam> aEnclosure = getEnclosure();
    double profileAngle = 0;

    /////////////////////////////////////////////////////////////
    //  Outgoing beam
    /////////////////////////////////////////////////////////////

    Side aSide = Side::Back;

    const auto aViewFactors =
      aEnclosure->beamViewFactors(profileAngle, aSide);

    EXPECT_NEAR(1.0 / 7.0, aEnclosure->directToDirect(profileAngle, aSide), 1e-6);

    size_t correctSize = 3;

    EXPECT_EQ(correctSize, aViewFactors.size());

    // Create correct results
    std::vector<BeamViewFactor> correctResults;

    size_t enclosureIndex = 1;
    size_t segmentIndex = 2;
    double viewFactor = 2.0 / 7.0;
    double percentHit = 1;
    BeamViewFactor aVF1 = BeamViewFactor(enclosureIndex, segmentIndex, viewFactor, percentHit);
    correctResults.push_back(aVF1);

    enclosureIndex = 1;
    segmentIndex = 1;
    viewFactor = 3.0 / 7.0;
    percentHit = 1;
    BeamViewFactor aVF2 = BeamViewFactor(enclosureIndex, segmentIndex, viewFactor, percentHit);
    correctResults.push_back(aVF2);

    enclosureIndex = 1;
    segmentIndex = 0;
    viewFactor = 1.0 / 7.0;
    percentHit = 1;
    BeamViewFactor aVF3 = BeamViewFactor(enclosureIndex, segmentIndex, viewFactor, percentHit);
    correctResults.push_back(aVF3);

    for(size_t i = 0; i < correctResults.size(); ++i)
    {
        EXPECT_EQ(correctResults[i].enclosureIndex, aViewFactors[i].enclosureIndex);
        EXPECT_EQ(correctResults[i].segmentIndex, aViewFactors[i].segmentIndex);
        EXPECT_NEAR(correctResults[i].value, aViewFactors[i].value, 1e-6);
        EXPECT_NEAR(correctResults[i].percentHit, aViewFactors[i].percentHit, 1e-6);
    }
}

TEST_F(TestEnclosure2DBeam1, Enclosure2DBeam6)
{
    SCOPED_TRACE("Begin Test: 2D Enclosure - Test outgoing beam view factors and "
                 "direct-direct component (-45 deg).");

    std::shared_ptr<CGeometry2DBeam> aEnclosure = getEnclosure();
    double profileAngle = -45;

    /////////////////////////////////////////////////////////////
    //  Outgoing beam
    /////////////////////////////////////////////////////////////

    Side aSide = Side::Back;

    const auto aViewFactors =
      aEnclosure->beamViewFactors(profileAngle, aSide);

    EXPECT_EQ(0.0, aEnclosure->directToDirect(profileAngle, aSide));

    size_t correctSize = 2;

    EXPECT_EQ(correctSize, aViewFactors.size());

    // Create correct results
    std::vector<BeamViewFactor> correctResults;

    size_t enclosureIndex = 1;
    size_t segmentIndex = 2;
    double viewFactor = 0.8;
    double percentHit = 1;
    BeamViewFactor aVF1 = BeamViewFactor(enclosureIndex, segmentIndex, viewFactor, percentHit);
    correctResults.push_back(aVF1);

    enclosureIndex = 1;
    segmentIndex = 1;
    viewFactor = 0.2;
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
