#pragma once
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/Math.hpp"
#include <string>
#include <cassert>
#include <iomanip>

namespace Math
{
	template <typename T>
	class Matrix4x4
	{
	public:
		Matrix4x4<T>();
		Matrix4x4<T>(const Matrix4x4<T>& aMatrix);

		T& operator()(const int aRow, const int aColumn);
		const T& operator()(const int aRow, const int aColumn) const;
		void operator=(const Matrix4x4<T>& aMatrix);

		void DecomposeMatrix(Vector3<T>& aPosition, Vector3<T>& aRotation, Vector3<T>& aScale);

		void LookAt(const Vector3<T>& aTargetPoint);
		void SetPosition(const Vector3<T>& aPosition);
		void SetLocalRotation(const Vector3<T>& aRotationInDegree);
		void SetScale(const Vector3<T>& aScale);
		void SetForward(const Vector3<T>& aForward);
		void SetRight(const Vector3<T>& aRight);
		void SetUp(const Vector3<T>& aUp);

		Vector3<T> GetPosition() const;
		Vector3<T> GetScale() const;
		Vector3<T> GetUp() const;
		Vector3<T> GetRight() const;
		Vector3<T> GetForward() const;

		static Matrix4x4<T> Identity();
		static Matrix4x4<T> CreateRotationAroundX(const T aAngleInRadians);
		static Matrix4x4<T> CreateRotationAroundY(const T aAngleInRadians);
		static Matrix4x4<T> CreateRotationAroundZ(const T aAngleInRadians);
		static Matrix4x4<T> CreateTranslationMatrix(const Matrix4x4<T>& aMatrix);
		static Matrix4x4<T> CreateRotationMatrix(const Matrix4x4<T>& aMatrix);
		static Matrix4x4<T> CreateScaleMatrix(const Matrix4x4<T>& aMatrix);

		static Matrix4x4<T> Transpose(const Matrix4x4<T>& aMatrixToTranspose);

		static Matrix4x4<T> GetInverse(Matrix4x4<T> aMatrixToInverse);
		static Matrix4x4<T> GetFastInverse(const Matrix4x4<T>& aTransform);
		Matrix4x4<T> GetFastInverse();

	private:
		static void SwapRows(Matrix4x4<T>& aMatrix, const int aRow1, const int aRow2);
		static void ScaleRow(Matrix4x4<T>& aMatrix, const int aRow, const T aFactor);
		static void AddScaledRow(Matrix4x4<T>& aMatrix, const int aRow1, const int aRow2, const T aFactor);
	private:
		T myMatrix[4][4];
	};

	typedef Matrix4x4<float> Matrix4x4f;
	typedef Matrix4x4<int> Matrix4x4i;

	template <typename T> void operator+=(Matrix4x4<T>& aMatrixA, const Matrix4x4<T>& aMatrixB);
	template <typename T> void operator-=(Matrix4x4<T>& aMatrixA, const Matrix4x4<T>& aMatrixB);
	template <typename T> void operator*=(Matrix4x4<T>& aMatrixA, const Matrix4x4<T>& aMatrixB);

	template <typename T> Matrix4x4<T> operator+(const Matrix4x4<T>& aMatrixA, const Matrix4x4<T>& aMatrixB);
	template <typename T> Matrix4x4<T> operator-(const Matrix4x4<T>& aMatrixA, const Matrix4x4<T>& aMatrixB);
	template <typename T> Matrix4x4<T> operator*(const Matrix4x4<T>& aMatrixA, const Matrix4x4<T>& aMatrixB);
	template <typename T> Vector4<T> operator*(const Vector4<T>& aVector, const Matrix4x4<T>& aMatrix);
	template <typename T> Vector4<T> operator*(const Matrix4x4<T>& aMatrix, const Vector4<T>& aVector);

	template <typename T> bool operator==(const Matrix4x4<T>& aMatrixA, const Matrix4x4<T>& aMatrixB);

	template <class T> std::ostream& operator<<(std::ostream& aOS, const Matrix4x4<T>& aMatrix);

	template<typename T>
	inline T& Matrix4x4<T>::operator()(const int aRow, const int aColumn)
	{
		assert(!(aRow - 1 < 0 || aColumn - 1 < 0) && "Cannot have negative indexes!");

		return myMatrix[aRow - 1][aColumn - 1];
	}

	template<typename T>
	inline const T& Matrix4x4<T>::operator()(const int aRow, const int aColumn) const
	{
		assert(!(aRow - 1 < 0 || aColumn - 1 < 0) && "Cannot have negative indexes!");

		return myMatrix[aRow - 1][aColumn - 1];
	}

	template<typename T>
	void operator+=(Matrix4x4<T>& aMatrixA, const Matrix4x4<T>& aMatrixB)
	{
		aMatrixA[0][0] += aMatrixB(1, 1);
		aMatrixA[0][1] += aMatrixB(1, 2);
		aMatrixA[0][2] += aMatrixB(1, 3);
		aMatrixA[0][3] += aMatrixB(1, 4);

		aMatrixA[1][0] += aMatrixB(2, 1);
		aMatrixA[1][1] += aMatrixB(2, 2);
		aMatrixA[1][2] += aMatrixB(2, 3);
		aMatrixA[1][3] += aMatrixB(2, 4);

		aMatrixA[2][0] += aMatrixB(3, 1);
		aMatrixA[2][1] += aMatrixB(3, 2);
		aMatrixA[2][2] += aMatrixB(3, 3);
		aMatrixA[2][3] += aMatrixB(3, 4);

		aMatrixA[3][0] += aMatrixB(4, 1);
		aMatrixA[3][1] += aMatrixB(4, 2);
		aMatrixA[3][2] += aMatrixB(4, 3);
		aMatrixA[3][3] += aMatrixB(4, 4);
	}

	template<typename T>
	Matrix4x4<T> operator+(const Matrix4x4<T>& aMatrixA, const Matrix4x4<T>& aMatrixB)
	{
		Matrix4x4<T> matrix;

		matrix(1, 1) = aMatrixA(1, 1) + aMatrixB(1, 1);
		matrix(1, 2) = aMatrixA(1, 2) + aMatrixB(1, 2);
		matrix(1, 3) = aMatrixA(1, 3) + aMatrixB(1, 3);
		matrix(1, 4) = aMatrixA(1, 4) + aMatrixB(1, 4);

		matrix(2, 1) = aMatrixA(2, 1) + aMatrixB(2, 1);
		matrix(2, 2) = aMatrixA(2, 2) + aMatrixB(2, 2);
		matrix(2, 3) = aMatrixA(2, 3) + aMatrixB(2, 3);
		matrix(2, 4) = aMatrixA(2, 4) + aMatrixB(2, 4);

		matrix(3, 1) = aMatrixA(3, 1) + aMatrixB(3, 1);
		matrix(3, 2) = aMatrixA(3, 2) + aMatrixB(3, 2);
		matrix(3, 3) = aMatrixA(3, 3) + aMatrixB(3, 3);
		matrix(3, 4) = aMatrixA(3, 4) + aMatrixB(3, 4);

		matrix(4, 1) = aMatrixA(4, 1) + aMatrixB(4, 1);
		matrix(4, 2) = aMatrixA(4, 2) + aMatrixB(4, 2);
		matrix(4, 3) = aMatrixA(4, 3) + aMatrixB(4, 3);
		matrix(4, 4) = aMatrixA(4, 4) + aMatrixB(4, 4);

		return matrix;
	}

	template<typename T>
	void operator-=(Matrix4x4<T>& aMatrixA, const Matrix4x4<T>& aMatrixB)
	{
		aMatrixA[0][0] -= aMatrixB(1, 1);
		aMatrixA[0][1] -= aMatrixB(1, 2);
		aMatrixA[0][2] -= aMatrixB(1, 3);
		aMatrixA[0][3] -= aMatrixB(1, 4);

		aMatrixA[1][0] -= aMatrixB(2, 1);
		aMatrixA[1][1] -= aMatrixB(2, 2);
		aMatrixA[1][2] -= aMatrixB(2, 3);
		aMatrixA[1][3] -= aMatrixB(2, 4);

		aMatrixA[2][0] -= aMatrixB(3, 1);
		aMatrixA[2][1] -= aMatrixB(3, 2);
		aMatrixA[2][2] -= aMatrixB(3, 3);
		aMatrixA[2][3] -= aMatrixB(3, 4);

		aMatrixA[3][0] -= aMatrixB(4, 1);
		aMatrixA[3][1] -= aMatrixB(4, 2);
		aMatrixA[3][2] -= aMatrixB(4, 3);
		aMatrixA[3][3] -= aMatrixB(4, 4);
	}

	template<typename T>
	void operator*=(Matrix4x4<T>& aMatrixA, const Matrix4x4<T>& aMatrixB)
	{
		Matrix4x4<T> matrix;

		for (int row = 0; row < 4; ++row)
		{
			for (int column = 0; column < 4; ++column)
			{
				matrix(row + 1, column + 1) = 0;

				for (int k = 0; k < 4; ++k)
				{
					matrix(row + 1, column + 1) += aMatrixA(row + 1, k + 1) * aMatrixB(k + 1, column + 1);
				}
			}
		}

		aMatrixA = matrix;
	}

	template<typename T>
	Matrix4x4<T> operator-(const Matrix4x4<T>& aMatrixA, const Matrix4x4<T>& aMatrixB)
	{
		Matrix4x4<T> matrix;

		matrix(1, 1) = aMatrixA(1, 1) - aMatrixB(1, 1);
		matrix(1, 2) = aMatrixA(1, 2) - aMatrixB(1, 2);
		matrix(1, 3) = aMatrixA(1, 3) - aMatrixB(1, 3);
		matrix(1, 4) = aMatrixA(1, 4) - aMatrixB(1, 4);

		matrix(2, 1) = aMatrixA(2, 1) - aMatrixB(2, 1);
		matrix(2, 2) = aMatrixA(2, 2) - aMatrixB(2, 2);
		matrix(2, 3) = aMatrixA(2, 3) - aMatrixB(2, 3);
		matrix(2, 4) = aMatrixA(2, 4) - aMatrixB(2, 4);

		matrix(3, 1) = aMatrixA(3, 1) - aMatrixB(3, 1);
		matrix(3, 2) = aMatrixA(3, 2) - aMatrixB(3, 2);
		matrix(3, 3) = aMatrixA(3, 3) - aMatrixB(3, 3);
		matrix(3, 4) = aMatrixA(3, 4) - aMatrixB(3, 4);

		matrix(4, 1) = aMatrixA(4, 1) - aMatrixB(4, 1);
		matrix(4, 2) = aMatrixA(4, 2) - aMatrixB(4, 2);
		matrix(4, 3) = aMatrixA(4, 3) - aMatrixB(4, 3);
		matrix(4, 4) = aMatrixA(4, 4) - aMatrixB(4, 4);

		return matrix;
	}

	template<typename T>
	Matrix4x4<T> operator*(const Matrix4x4<T>& aMatrixA, const Matrix4x4<T>& aMatrixB)
	{
		Matrix4x4<T> matrix;

		for (int row = 0; row < 4; ++row)
		{
			for (int column = 0; column < 4; ++column)
			{
				matrix(row + 1, column + 1) = 0;

				for (int k = 0; k < 4; ++k)
				{
					matrix(row + 1, column + 1) += aMatrixA(row + 1, k + 1) * aMatrixB(k + 1, column + 1);
				}
			}
		}

		return matrix;
	}

	template<typename T>
	Vector4<T> operator*(const Vector4<T>& aVector, const Matrix4x4<T>& aMatrix)
	{
		Vector4<T> result;

		result.x = aVector.x * aMatrix(1, 1) + aVector.y * aMatrix(2, 1) + aVector.z * aMatrix(3, 1) + aVector.w * aMatrix(4, 1);
		result.y = aVector.x * aMatrix(1, 2) + aVector.y * aMatrix(2, 2) + aVector.z * aMatrix(3, 2) + aVector.w * aMatrix(4, 2);
		result.z = aVector.x * aMatrix(1, 3) + aVector.y * aMatrix(2, 3) + aVector.z * aMatrix(3, 3) + aVector.w * aMatrix(4, 3);
		result.w = aVector.x * aMatrix(1, 4) + aVector.y * aMatrix(2, 4) + aVector.z * aMatrix(3, 4) + aVector.w * aMatrix(4, 4);

		return result;
	}

	template<typename T>
	Vector4<T> operator*(const Matrix4x4<T>& aMatrix, const Vector4<T>& aVector)
	{
		Vector4<T> result;

		result.x = aVector.x * aMatrix(1, 1) + aVector.y * aMatrix(2, 1) + aVector.z * aMatrix(3, 1) + aVector.w * aMatrix(4, 1);
		result.y = aVector.x * aMatrix(1, 2) + aVector.y * aMatrix(2, 2) + aVector.z * aMatrix(3, 2) + aVector.w * aMatrix(4, 2);
		result.z = aVector.x * aMatrix(1, 3) + aVector.y * aMatrix(2, 3) + aVector.z * aMatrix(3, 3) + aVector.w * aMatrix(4, 3);
		result.w = aVector.x * aMatrix(1, 4) + aVector.y * aMatrix(2, 4) + aVector.z * aMatrix(3, 4) + aVector.w * aMatrix(4, 4);

		return result;
	}

	template<typename T>
	bool operator==(const Matrix4x4<T>& aMatrixA, const Matrix4x4<T>& aMatrixB)
	{
		for (int row = 0; row < 4; ++row)
		{
			for (int column = 0; column < 4; ++column)
			{
				if (aMatrixA(row + 1, column + 1) != aMatrixB(row + 1, column + 1))
					return false;
			}
		}

		return true;
	}

	template<typename T>
	inline void Matrix4x4<T>::operator=(const Matrix4x4<T>& aMatrix)
	{
		myMatrix[0][0] = aMatrix(1, 1);
		myMatrix[0][1] = aMatrix(1, 2);
		myMatrix[0][2] = aMatrix(1, 3);
		myMatrix[0][3] = aMatrix(1, 4);

		myMatrix[1][0] = aMatrix(2, 1);
		myMatrix[1][1] = aMatrix(2, 2);
		myMatrix[1][2] = aMatrix(2, 3);
		myMatrix[1][3] = aMatrix(2, 4);

		myMatrix[2][0] = aMatrix(3, 1);
		myMatrix[2][1] = aMatrix(3, 2);
		myMatrix[2][2] = aMatrix(3, 3);
		myMatrix[2][3] = aMatrix(3, 4);

		myMatrix[3][0] = aMatrix(4, 1);
		myMatrix[3][1] = aMatrix(4, 2);
		myMatrix[3][2] = aMatrix(4, 3);
		myMatrix[3][3] = aMatrix(4, 4);
	}

	//TO-DO: This does not work at all LOL, will continue one day
	template<typename T>
	inline void Matrix4x4<T>::DecomposeMatrix(Vector3<T>& aPosition, Vector3<T>& aRotation, Vector3<T>& aScale)
	{
		aPosition.x = myMatrix[3][0];
		aPosition.y = myMatrix[3][1];
		aPosition.z = myMatrix[3][2];

		aScale.x = std::sqrt(myMatrix[0][0] * myMatrix[0][0] + myMatrix[1][0] * myMatrix[1][0] + myMatrix[2][0] * myMatrix[2][0]);
		aScale.y = std::sqrt(myMatrix[0][1] * myMatrix[0][1] + myMatrix[1][1] * myMatrix[1][1] + myMatrix[2][1] * myMatrix[2][1]);
		aScale.z = std::sqrt(myMatrix[0][2] * myMatrix[0][2] + myMatrix[1][2] * myMatrix[1][2] + myMatrix[2][2] * myMatrix[2][2]);

		const float det = myMatrix[0][0] * (myMatrix[1][1] * myMatrix[2][2] - myMatrix[1][2] * myMatrix[2][1])
			- myMatrix[0][1] * (myMatrix[1][0] * myMatrix[2][2] - myMatrix[1][2] * myMatrix[2][0])
			+ myMatrix[0][2] * (myMatrix[1][0] * myMatrix[2][1] - myMatrix[1][1] * myMatrix[2][0]);

		if (det < 0.0f)
		{
			aScale.x = -aScale.x;
		}

		constexpr float EPSILON = 1e-6f;

		float x = 0;
		float y = 0;
		float z = 0;
		float w = 1.0f;

		if (det > EPSILON * EPSILON)
		{
			const Vector3<T> xAxis = { myMatrix[0][0] / aScale.x, myMatrix[0][1] / aScale.x, myMatrix[0][2] / aScale.x };
			const Vector3<T> yAxis = { myMatrix[1][0] / aScale.y, myMatrix[1][1] / aScale.y, myMatrix[1][2] / aScale.y };
			const Vector3<T> zAxis = { myMatrix[2][0] / aScale.z, myMatrix[2][1] / aScale.z, myMatrix[2][2] / aScale.z };

			const float trace = xAxis.x + yAxis.y + zAxis.z;

			if (trace > 0.0f)
			{
				const float s = 0.5f / std::sqrt(trace + 1.0f);

				x = (yAxis.z - zAxis.y) * s;
				y = (zAxis.x - xAxis.z) * s;
				z = (xAxis.y - yAxis.x) * s;
				w = 0.25f / s;
			}
			else
			{
				if (xAxis.x > yAxis.y && xAxis.x > zAxis.z)
				{
					const float s = 2.0f * std::sqrt(1.0f + xAxis.x - yAxis.y - zAxis.z);

					x = 0.25f * s;
					y = (yAxis.x + xAxis.y) / s;
					z = (zAxis.x + xAxis.z) / s;
					w = (yAxis.z - zAxis.y) / s;
				}
				else if (yAxis.y > zAxis.z)
				{
					const float s = 2.0f * std::sqrt(1.0f + yAxis.y - xAxis.x - zAxis.z);

					x = (yAxis.x + xAxis.y) / s;
					y = 0.25f * s;
					z = (zAxis.y + yAxis.z) / s;
					w = (zAxis.x - xAxis.z) / s;
				}
				else
				{
					const float s = 2.0f * std::sqrt(1.0f + zAxis.z - xAxis.x - yAxis.y);

					x = (zAxis.x + xAxis.z) / s;
					y = (zAxis.y + yAxis.z) / s;
					z = 0.25f * s;
					w = (xAxis.y - yAxis.x) / s;
				}
			}
		}

		const float length = std::sqrt(x * x + y * y + z * z + w * w);

		x /= length;
		y /= length;
		z /= length;
		w /= length;

		aRotation.x = x;
		aRotation.y = y;
		aRotation.z = z;
	}

	//TO-DO: Fix so it work with scaling and also show rotation values and rotate correct
	template<typename T>
	inline void Matrix4x4<T>::LookAt(const Vector3<T>& aTargetPoint)
	{
		const Vector3<T> position = GetPosition();
		const Vector3<T> scale = GetScale();

		Vector3<T> forward = (aTargetPoint - position);
		forward.Normalize();

		Vector3<T> right = Math::Cross({ 0,1,0 }, forward);
		right.Normalize();

		Vector3<T> up = Math::Cross(forward, right);
		up.Normalize();

		myMatrix[0][0] = right.x * scale.x;
		myMatrix[0][1] = right.y * scale.x;
		myMatrix[0][2] = right.z * scale.x;

		myMatrix[1][0] = up.x * scale.y;
		myMatrix[1][1] = up.y * scale.y;
		myMatrix[1][2] = up.z * scale.y;

		myMatrix[2][0] = forward.x * scale.z;
		myMatrix[2][1] = forward.y * scale.z;
		myMatrix[2][2] = forward.z * scale.z;
	}

	template<typename T>
	inline void Matrix4x4<T>::SetPosition(const Vector3<T>& aPosition)
	{
		myMatrix[3][0] = aPosition.x;
		myMatrix[3][1] = aPosition.y;
		myMatrix[3][2] = aPosition.z;
	}

	//TO-DO: This does not work as intended but it is good enough for now will continue one day
	template<typename T>
	inline void Matrix4x4<T>::SetLocalRotation(const Vector3<T>& aRotationInDegree)
	{
		const Vector3<T> rad = aRotationInDegree * globalDegToRad;

		const Matrix4x4<T> scaleMatrix = Matrix4x4<T>::CreateScaleMatrix(*this);
		Matrix4x4<T> rotationMatrix = Matrix4x4<T>::Identity();

		rotationMatrix *= Matrix4x4<T>::CreateRotationAroundX(rad.x);
		rotationMatrix *= Matrix4x4<T>::CreateRotationAroundY(rad.y);
		rotationMatrix *= Matrix4x4<T>::CreateRotationAroundZ(rad.z);

		const Matrix4x4<T> newRotationMatrix = scaleMatrix * rotationMatrix;

		myMatrix[0][0] = newRotationMatrix(1, 1);
		myMatrix[0][1] = newRotationMatrix(1, 2);
		myMatrix[0][2] = newRotationMatrix(1, 3);
		myMatrix[0][3] = newRotationMatrix(1, 4);

		myMatrix[1][0] = newRotationMatrix(2, 1);
		myMatrix[1][1] = newRotationMatrix(2, 2);
		myMatrix[1][2] = newRotationMatrix(2, 3);
		myMatrix[1][3] = newRotationMatrix(2, 4);

		myMatrix[2][0] = newRotationMatrix(3, 1);
		myMatrix[2][1] = newRotationMatrix(3, 2);
		myMatrix[2][2] = newRotationMatrix(3, 3);
		myMatrix[2][3] = newRotationMatrix(3, 4);
	}

	template<typename T>
	inline void Matrix4x4<T>::SetScale(const Vector3<T>& aScale)
	{
		const float minScale = 0.001f;

		myMatrix[0][0] = (aScale.x < minScale) ? minScale : aScale.x;
		myMatrix[1][1] = (aScale.y < minScale) ? minScale : aScale.y;
		myMatrix[2][2] = (aScale.z < minScale) ? minScale : aScale.z;
	}

	template<typename T>
	inline void Matrix4x4<T>::SetForward(const Vector3<T>& aForward)
	{
		myMatrix[2][0] = aForward.x;
		myMatrix[2][1] = aForward.y;
		myMatrix[2][2] = aForward.z;
	}

	template<typename T>
	inline void Matrix4x4<T>::SetRight(const Vector3<T>& aRight)
	{
		myMatrix[0][0] = aRight.x;
		myMatrix[0][1] = aRight.y;
		myMatrix[0][2] = aRight.z;
	}

	template<typename T>
	inline void Matrix4x4<T>::SetUp(const Vector3<T>& aUp)
	{
		myMatrix[1][0] = aUp.x;
		myMatrix[1][1] = aUp.y;
		myMatrix[1][2] = aUp.z;
	}

	template<typename T>
	inline Vector3<T> Matrix4x4<T>::GetScale() const
	{
		Vector3<T> scale;

		scale.x = GetRight().Length();
		scale.y = GetUp().Length();
		scale.z = GetForward().Length();

		return scale;
	}

	template<typename T>
	inline Vector3<T> Matrix4x4<T>::GetPosition() const
	{
		return Vector3<T>(myMatrix[3][0], myMatrix[3][1], myMatrix[3][2]);
	}

	template<typename T>
	inline Vector3<T> Matrix4x4<T>::GetUp() const
	{
		return Vector3<T>(myMatrix[1][0], myMatrix[1][1], myMatrix[1][2]);
	}

	template<typename T>
	inline Vector3<T> Matrix4x4<T>::GetRight() const
	{
		return Vector3<T>(myMatrix[0][0], myMatrix[0][1], myMatrix[0][2]);
	}

	template<typename T>
	inline Vector3<T> Matrix4x4<T>::GetForward() const
	{
		return Vector3<T>(myMatrix[2][0], myMatrix[2][1], myMatrix[2][2]);
	}

	template<typename T>
	inline Matrix4x4<T>::Matrix4x4()
	{
		myMatrix[0][0] = 1;
		myMatrix[0][1] = 0;
		myMatrix[0][2] = 0;
		myMatrix[0][3] = 0;

		myMatrix[1][0] = 0;
		myMatrix[1][1] = 1;
		myMatrix[1][2] = 0;
		myMatrix[1][3] = 0;

		myMatrix[2][0] = 0;
		myMatrix[2][1] = 0;
		myMatrix[2][2] = 1;
		myMatrix[2][3] = 0;

		myMatrix[3][0] = 0;
		myMatrix[3][1] = 0;
		myMatrix[3][2] = 0;
		myMatrix[3][3] = 1;
	}

	template<typename T>
	inline Matrix4x4<T>::Matrix4x4(const Matrix4x4<T>& aMatrix)
	{
		myMatrix[0][0] = aMatrix(1, 1);
		myMatrix[0][1] = aMatrix(1, 2);
		myMatrix[0][2] = aMatrix(1, 3);
		myMatrix[0][3] = aMatrix(1, 4);

		myMatrix[1][0] = aMatrix(2, 1);
		myMatrix[1][1] = aMatrix(2, 2);
		myMatrix[1][2] = aMatrix(2, 3);
		myMatrix[1][3] = aMatrix(2, 4);

		myMatrix[2][0] = aMatrix(3, 1);
		myMatrix[2][1] = aMatrix(3, 2);
		myMatrix[2][2] = aMatrix(3, 3);
		myMatrix[2][3] = aMatrix(3, 4);

		myMatrix[3][0] = aMatrix(4, 1);
		myMatrix[3][1] = aMatrix(4, 2);
		myMatrix[3][2] = aMatrix(4, 3);
		myMatrix[3][3] = aMatrix(4, 4);
	}

	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::Identity()
	{
		return Matrix4x4<T>();
	}

	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundX(const T aAngleInRadians)
	{
		Matrix4x4<T> rotationMatrix;
		const T cosA = static_cast<T>(std::cos(aAngleInRadians));
		const T sinA = static_cast<T>(std::sin(aAngleInRadians));

		rotationMatrix(2, 2) = cosA;
		rotationMatrix(2, 3) = sinA;
		rotationMatrix(3, 2) = -sinA;
		rotationMatrix(3, 3) = cosA;

		return rotationMatrix;
	}

	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundY(const T aAngleInRadians)
	{
		Matrix4x4<T> rotationMatrix;
		const T cosA = static_cast<T>(std::cos(aAngleInRadians));
		const T sinA = static_cast<T>(std::sin(aAngleInRadians));

		rotationMatrix(1, 1) = cosA;
		rotationMatrix(1, 3) = -sinA;
		rotationMatrix(3, 1) = sinA;
		rotationMatrix(3, 3) = cosA;

		return rotationMatrix;
	}

	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundZ(const T aAngleInRadians)
	{
		Matrix4x4<T> rotationMatrix;
		const T cosA = static_cast<T>(std::cos(aAngleInRadians));
		const T sinA = static_cast<T>(std::sin(aAngleInRadians));

		rotationMatrix(1, 1) = cosA;
		rotationMatrix(1, 2) = sinA;
		rotationMatrix(2, 1) = -sinA;
		rotationMatrix(2, 2) = cosA;

		return rotationMatrix;
	}

	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateTranslationMatrix(const Matrix4x4<T>& aMatrix)
	{
		Matrix4x4<T> result = Matrix4x4<T>::Identity();

		result(4, 1) = aMatrix(4, 1);
		result(4, 2) = aMatrix(4, 2);
		result(4, 3) = aMatrix(4, 3);

		return result;
	}

	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationMatrix(const Matrix4x4<T>& aMatrix)
	{
		const Vector3<T> scale = aMatrix.GetScale();

		Matrix4x4<T> rotationMatrix = Matrix4x4<T>::Identity();

		rotationMatrix(1, 1) = aMatrix(1, 1) / scale.x;
		rotationMatrix(2, 1) = aMatrix(2, 1) / scale.x;
		rotationMatrix(3, 1) = aMatrix(3, 1) / scale.x;

		rotationMatrix(1, 2) = aMatrix(1, 2) / scale.y;
		rotationMatrix(2, 2) = aMatrix(2, 2) / scale.y;
		rotationMatrix(3, 2) = aMatrix(3, 2) / scale.y;

		rotationMatrix(1, 3) = aMatrix(1, 3) / scale.z;
		rotationMatrix(2, 3) = aMatrix(2, 3) / scale.z;
		rotationMatrix(3, 3) = aMatrix(3, 3) / scale.z;

		return rotationMatrix;
	}

	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateScaleMatrix(const Matrix4x4<T>& aMatrix)
	{
		Matrix4x4<T> result = Matrix4x4<T>::Identity();

		result(1, 1) = aMatrix.GetRight().Length();
		result(2, 2) = aMatrix.GetUp().Length();
		result(3, 3) = aMatrix.GetForward().Length();

		return result;
	}

	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::Transpose(const Matrix4x4<T>& aMatrixToTranspose)
	{
		Matrix4x4<T> transposed;

		for (int i = 1; i <= 4; i++)
		{
			for (int j = 1; j <= 4; j++)
			{
				transposed(i, j) = aMatrixToTranspose(j, i);
			}
		}

		return transposed;
	}

	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::GetFastInverse(const Matrix4x4<T>& aTransform)
	{
		Matrix4x4<T> inverse = aTransform;

		inverse(1, 2) = aTransform(2, 1);
		inverse(1, 3) = aTransform(3, 1);

		inverse(2, 1) = aTransform(1, 2);
		inverse(2, 3) = aTransform(3, 2);

		inverse(3, 1) = aTransform(1, 3);
		inverse(3, 2) = aTransform(2, 3);

		inverse(4, 1) = ((-aTransform(4, 1)) * inverse(1, 1) - aTransform(4, 2) * inverse(2, 1) - aTransform(4, 3) * inverse(3, 1));
		inverse(4, 2) = ((-aTransform(4, 1)) * inverse(1, 2) - aTransform(4, 2) * inverse(2, 2) - aTransform(4, 3) * inverse(3, 2));
		inverse(4, 3) = ((-aTransform(4, 1)) * inverse(1, 3) - aTransform(4, 2) * inverse(2, 3) - aTransform(4, 3) * inverse(3, 3));

		inverse(4, 4) = aTransform(4, 4);

		return inverse;
	}

	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::GetInverse(Matrix4x4<T> aMatrixToInverse) //Dear Savior ChatGPT, no clue how it work compared to GetFastInverse but the returned value seem to be correct using Matrix Calculator on internet
	{
		Matrix4x4<T> inverse = Matrix4x4<T>::Identity();

		for (int i = 0; i < 4; ++i)
		{
			int pivotRow = i + 1;

			while (pivotRow < 4 && aMatrixToInverse(pivotRow, i + 1) == 0)
			{
				++pivotRow;
			}

			if (pivotRow == 4 + 1)
				assert(false && "Matrix is singular, no unique inverse.");

			SwapRows(aMatrixToInverse, i + 1, pivotRow);
			SwapRows(inverse, i + 1, pivotRow);

			float pivotElement = aMatrixToInverse(i + 1, i + 1);
			ScaleRow(aMatrixToInverse, i + 1, 1 / pivotElement);
			ScaleRow(inverse, i + 1, 1 / pivotElement);

			for (int j = 0; j < 4; ++j)
			{
				if (j != i)
				{
					float factor = -aMatrixToInverse(j + 1, i + 1);
					AddScaledRow(aMatrixToInverse, j + 1, i + 1, factor);
					AddScaledRow(inverse, j + 1, i + 1, factor);
				}
			}
		}

		return inverse;
	}

	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::GetFastInverse()
	{
		return GetFastInverse(*this);
	}

	template<typename T>
	inline void Matrix4x4<T>::SwapRows(Matrix4x4<T>& aMatrix, const int aRow1, const int aRow2)
	{
		for (int i = 0; i < 4; ++i)
		{
			Math::Swap(aMatrix(aRow1, i + 1), aMatrix(aRow2, i + 1));
		}
	}

	template<typename T>
	inline void Matrix4x4<T>::ScaleRow(Matrix4x4<T>& aMatrix, const int aRow, const T aFactor)
	{
		for (int i = 0; i < 4; ++i)
		{
			aMatrix(aRow, i + 1) *= aFactor;
		}
	}

	template<typename T>
	inline void Matrix4x4<T>::AddScaledRow(Matrix4x4<T>& aMatrix, const int aRow1, const int aRow2, const T aFactor)
	{
		for (int i = 0; i < 4; ++i)
		{
			aMatrix(aRow1, i + 1) += aFactor * aMatrix(aRow2, i + 1);
		}
	}

	template <class T>
	std::ostream& operator<<(std::ostream& aOS, const Matrix4x4<T>& aMatrix)
	{
		float max = 1;

		for (int row = 1; row <= 4; ++row)
		{
			for (int column = 1; column <= 4; ++column)
			{
				max = Math::GetMax(max, static_cast<float>(aMatrix(row, column)));
			}
		}

		const std::string maxString = std::to_string(max);
		const size_t characterLength = maxString.length() + 1;
		const size_t totalLength = characterLength * 4 + 2;

		return aOS
			<< " " << std::string(totalLength, '-') << std::endl
			<< "| " << std::setw(characterLength) << aMatrix(1, 1) << std::setw(characterLength) << aMatrix(1, 2) << std::setw(characterLength) << aMatrix(1, 3) << std::setw(characterLength) << aMatrix(1, 4) << " |" << std::endl
			<< "| " << std::setw(characterLength) << aMatrix(2, 1) << std::setw(characterLength) << aMatrix(2, 2) << std::setw(characterLength) << aMatrix(2, 3) << std::setw(characterLength) << aMatrix(2, 4) << " |" << std::endl
			<< "| " << std::setw(characterLength) << aMatrix(3, 1) << std::setw(characterLength) << aMatrix(3, 2) << std::setw(characterLength) << aMatrix(3, 3) << std::setw(characterLength) << aMatrix(3, 4) << " |" << std::endl
			<< "| " << std::setw(characterLength) << aMatrix(4, 1) << std::setw(characterLength) << aMatrix(4, 2) << std::setw(characterLength) << aMatrix(4, 3) << std::setw(characterLength) << aMatrix(4, 4) << " |" << std::endl
			<< " " << std::string(totalLength, '-') << std::endl;
	}
}