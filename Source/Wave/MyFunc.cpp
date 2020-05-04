// Fill out your copyright notice in the Description page of Project Settings.


#include "MyFunc.h"

// ��( axis ) ��]�p�x( angle( ���W�A���p ) )
FQuat MyFunc::SetAxisAngle(const FVector & axis, float angle)
{
	FQuat q;
	float t = FMath::Sin(angle / 2);
	q.X = axis.X * t;
	q.Y = axis.Y * t;
	q.Z = axis.Z * t;
	q.W = FMath::Cos(angle / 2);

	return q;
}