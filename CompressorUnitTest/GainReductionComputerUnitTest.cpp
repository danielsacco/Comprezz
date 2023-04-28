#include "pch.h"
#include "CppUnitTest.h"
#include "../../DspLibs/GainReductionComputer.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace GainReductionComputerUnitTest
{
	TEST_CLASS(GainReduction_HardKnee)
	{
  private:
    const double knee_0 = 0.;
    const double threshold = -10.;

	public:
		
		TEST_METHOD(TestSampleBelowThreshold)
		{
      GainReductionComputer sut{ threshold, 10., knee_0 };

      double result = sut.Compute(threshold  - 10);

      Assert::AreEqual(0., result);

		}

    TEST_METHOD(TestSampleOnThreshold)
    {
      GainReductionComputer sut{ threshold, 10., knee_0 };

      double result = sut.Compute(threshold);

      Assert::AreEqual(0., result);

    }

    TEST_METHOD(TestSampleAboveThresholdRatio2)
    {
      GainReductionComputer sut{ threshold, 2., knee_0 };

      double result = sut.Compute(threshold + 10);

      Assert::AreEqual(-5., result);

    }

    TEST_METHOD(TestSampleAboveThresholdRatio10)
    {
      GainReductionComputer sut{ threshold, 10., knee_0 };

      double result = sut.Compute(threshold + 10);

      Assert::AreEqual(-9., result);

    }
	};

  TEST_CLASS(GainReduction_SoftKnee)
  {
  private:
    const double knee_20 = 20.;
    const double threshold = -20.;

  public:

    TEST_METHOD(SampleBelowKnee_ShouldNotReduce)
    {
      GainReductionComputer sut{ threshold, 10., knee_20 };

      double result = sut.Compute(threshold - knee_20 / 2 - 10);

      Assert::AreEqual(0., result);

    }

    TEST_METHOD(SampleAtKneeStart_ShouldNotReduce)
    {
      GainReductionComputer sut{ threshold, 10., knee_20 };

      double result = sut.Compute(threshold - knee_20 / 2);

      Assert::AreEqual(0., result);

    }

    TEST_METHOD(SampleAtKneeEnd_ShouldFullReduce)
    {
      GainReductionComputer sut{ threshold, 10., knee_20 };

      double result = sut.Compute(threshold + knee_20 / 2);

      Assert::AreEqual(-9., result);

    }

    TEST_METHOD(SampleAboveKneeEnd_ShouldFullReduce)
    {
      GainReductionComputer sut{ threshold, 10., knee_20 };

      double result = sut.Compute(threshold + knee_20 / 2 + 10);

      Assert::AreEqual(-18., result);

    }

    TEST_METHOD(SampleAtThreshold_ShouldHalfReduce)
    {
      GainReductionComputer sut{ threshold, 2., knee_20 };

      double result = sut.Compute(threshold);

      Assert::AreEqual(-1.25, result);

    }

    TEST_METHOD(SampleBetweenKneeStartAndThreshold_ShouldReduce)
    {
      GainReductionComputer sut{ threshold, 2., knee_20 };

      double result = sut.Compute(threshold - 5.);

      Assert::AreEqual(-.15625, result);

    }

    TEST_METHOD(SampleBetweenKneeEndAndThreshold_ShouldReduce)
    {
      GainReductionComputer sut{ threshold, 2., knee_20 };

      double result = sut.Compute(threshold + 5.);

      Assert::AreEqual(-4.21875, result);

    }


  };

}
