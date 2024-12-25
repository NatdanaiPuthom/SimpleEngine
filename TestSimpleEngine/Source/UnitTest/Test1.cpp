#include "CppUnitTest.h"
#include "Source/TestExample.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest1
{
	TEST_CLASS(UnitTest1)
	{
	public:

		TEST_METHOD(Addition)
		{
			Simple::MathTest mathTest;

			int value1 = 5;
			int value2 = 5;
			int expected = 10;

			int actual = mathTest.Add(value1, value2);

			Assert::AreEqual(expected, actual, L"The value should be equal.");
		}
	};
}
