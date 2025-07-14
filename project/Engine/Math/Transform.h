#include "Vector3.h"
#include "Vector4.h"
#include "Quaternion.h"

#pragma once
struct Transform {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

struct QuaternionTransform
{
	Vector3 scale;
	Quaternion rotate;
	Vector3 translate;
};

//class Transform {};
