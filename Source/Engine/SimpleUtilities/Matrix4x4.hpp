#pragma once
//TO-DO: Fix the operator shit

namespace SimpleUtilities
{
	template<typename T>
	class Vector3;

	template<typename T>
	class Vector4;

	static float GetRadToDeg();
	static float GetDegToRad();

	template <typename T>
	class Matrix4x4
	{
	public:
		Matrix4x4<T>();
		Matrix4x4<T>(const Matrix4x4<T>& aMatrix);

		T& operator()(const int aRow, const int aColumn);
		const T& operator()(const int aRow, const int aColumn) const;
		void operator=(const Matrix4x4<T>& aMatrix);

		void SetPosition(const Vector3<T>& aPosition);
		void SetLocalRotation(const Vector3<T>& aRotationInDegree);

		Vector3<T> GetPosition() const;

		static Matrix4x4<T> Identity();
		static Matrix4x4<T> CreateRotationAroundX(const T aAngleInRadians);
		static Matrix4x4<T> CreateRotationAroundY(const T aAngleInRadians);
		static Matrix4x4<T> CreateRotationAroundZ(const T aAngleInRadians);

		static Matrix4x4<T> Transpose(const Matrix4x4<T>& aMatrixToTranspose);

		static Matrix4x4<T> GetFastInverse(const Matrix4x4<T>& aTransform);
		Matrix4x4<T> GetFastInverse();

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

	template<typename T>
	inline void Matrix4x4<T>::SetPosition(const SimpleUtilities::Vector3<T>& aPosition)
	{
		myMatrix[3][0] = aPosition.x;
		myMatrix[3][1] = aPosition.y;
		myMatrix[3][2] = aPosition.z;
	}

	template<typename T>
	inline void Matrix4x4<T>::SetLocalRotation(const SimpleUtilities::Vector3<T>& aRotationInDegree)
	{
		const SimpleUtilities::Vector3<T> rad = aRotationInDegree * GetDegToRad();

		SimpleUtilities::Matrix4x4<T> rotationMatrix = SimpleUtilities::Matrix4x4<T>::Identity();

		rotationMatrix *= SimpleUtilities::Matrix4x4<T>::CreateRotationAroundX(rad.x);
		rotationMatrix *= SimpleUtilities::Matrix4x4<T>::CreateRotationAroundY(rad.y);
		rotationMatrix *= SimpleUtilities::Matrix4x4<T>::CreateRotationAroundZ(rad.z);

		myMatrix[0][0] = rotationMatrix(1, 1);
		myMatrix[0][1] = rotationMatrix(1, 2);
		myMatrix[0][2] = rotationMatrix(1, 3);
		myMatrix[0][3] = rotationMatrix(1, 4);

		myMatrix[1][0] = rotationMatrix(2, 1);
		myMatrix[1][1] = rotationMatrix(2, 2);
		myMatrix[1][2] = rotationMatrix(2, 3);
		myMatrix[1][3] = rotationMatrix(2, 4);

		myMatrix[2][0] = rotationMatrix(3, 1);
		myMatrix[2][1] = rotationMatrix(3, 2);
		myMatrix[2][2] = rotationMatrix(3, 3);
		myMatrix[2][3] = rotationMatrix(3, 4);

		//Doesn't take row 4 to keep it's current position
	}

	template<typename T>
	inline Vector3<T> Matrix4x4<T>::GetPosition() const
	{
		const Vector3<T> position(myMatrix[3][0], myMatrix[3][1], myMatrix[3][2]);
		return position;
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
		return Matrix4x4<T>(); //Just For Dummies Like Me
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
	inline Matrix4x4<T> Matrix4x4<T>::GetFastInverse()
	{
		return GetFastInverse(*this);
	}
}
