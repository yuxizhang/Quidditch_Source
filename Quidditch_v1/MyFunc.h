// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
/**
 * 
 */
class QUIDDITCH_V1_API MyFunc
{
public:
	MyFunc();
	~MyFunc();
	bool OutofBoundary(FVector Location);
	FVector SetinBoundary(FVector Location);
	static FRotator MapRotatorToPi(FRotator rotation);
};
