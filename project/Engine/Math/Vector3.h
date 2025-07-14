#pragma once

struct Vector3
{
	float x;
	float y;
	float z;

	// 減算演算子のオーバーロード
	Vector3 operator-(const Vector3& other) const;
};