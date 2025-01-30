#include "CppUnitTest.h"

import MOD_Vector2;

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace Simple;

namespace UnitTest1
{
	TEST_CLASS(UnitTest1)
	{
	public:

		TEST_METHOD(AdditionFloat)
		{
			const Vector2<float> vectorA(1.5f, 2.5f);
			const Vector2<float> vectorB(0.5f, 0.5f);
			const Vector2<float> expected(2.0f, 3.0f);
			const Vector2<float> result = vectorA + vectorB;

			Assert::AreEqual(expected.x, result.x, L"The value x should be equal.");
			Assert::AreEqual(expected.y, result.y, L"The value y should be equal.");
		}

		TEST_METHOD(AdditionInt)
		{
			const Vector2<int> vectorA(1, 2);
			const Vector2<int> vectorB(0, 5);
			const Vector2<int> expected(1, 7);
			const Vector2<int> result = vectorA + vectorB;

			Assert::AreEqual(expected.x, result.x, L"The value x should be equal.");
			Assert::AreEqual(expected.y, result.y, L"The value y should be equal.");
		}
	};
}
