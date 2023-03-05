#include "Transform.h"
#include <math.h>

CadMath::Matrix4x4 Transform::GetMatrix() const
{
	return GetLocationMatrix()*GetRotationMatrix()*GetScaleMatrix();
}

CadMath::Matrix4x4 Transform::GetRotationMatrix() const
{
	CadMath::Matrix4x4 rotationX = {
	  { {1.0f,0.0f,0.0f,0.0f},
		{0.0f,cosf(rotation.X()),-sinf(rotation.X()),0.0f},
		{0.0f,sinf(rotation.X()), cosf(rotation.X()),0.0f},
		{0.0f,0.0f,0.0f,1.0f} } };
	CadMath::Matrix4x4 rotationY = {
	  { { cosf(rotation.Y()),0.0f,sinf(rotation.Y()),0.0f},
		{0.0f,1.0f,0.0f,0.0f},
		{-sinf(rotation.Y()),0.0f,cosf(rotation.Y()),0.0f},
		{0.0f,0.0f,0.0f,1.0f} } };
	CadMath::Matrix4x4 rotationZ = {
	  { {cosf(rotation.Z()),-sinf(rotation.Z()),0.0f,0.0f},
		{sinf(rotation.Z()), cosf(rotation.Z()),0.0f,0.0f},
		{0.0f,0.0f,1.0f,0.0f},
		{0.0f,0.0f,0.0f,1.0f} } };
	return rotationX*rotationY*rotationZ;
}

CadMath::Matrix4x4 Transform::GetLocationMatrix() const
{
	return { {
		{1.0f,0.0f,0.0f,location.X()},
		{0.0f,1.0f,0.0f,location.Y()},
		{0.0f,0.0f,1.0f,location.Z()},
		{0.0f,0.0f,0.0f,1.0f}}};
}

CadMath::Matrix4x4 Transform::GetScaleMatrix() const
{
	return { {
		{scale.X(),0.0f,     0.0f,     0.0f},
		{0.0f,     scale.Y(),0.0f,     0.0f},
		{0.0f,     0.0f,     scale.Z(),0.0f},
		{0.0f,     0.0f,     0.0f,     1.0f}} };
}

Transform Transform::operator+(const Transform& t)
{
	return Transform(location+t.location,rotation+t.rotation,scale+t.scale);
}
