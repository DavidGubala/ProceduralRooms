// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


struct Coordinate {
	FVector pos;
	FRotator rot;
	int SpawnType = 0;// 0 = wall + pillar, 1 = Door, 2 = nothing;
	int WallSide = 0;
	bool partOfPerimeter = true; //  1 for yes it is part of the perimenter or 0 if it is an inside wall
	bool canBeDoor = true;
	Coordinate* next = NULL;
	Coordinate* prev = NULL;

};

class PROCEDURALROOMS_API D_Room
{
private:
	struct FMath math;
	float GoldenRatio = ((1 + sqrt(5)) / 2) - 1;
	int i = 0, coordinateCount = 0, xWallCount = 0, yWallCount = 0;

public:
	TArray<Coordinate> coordinates;
	D_Room(int WallCount);
	~D_Room();
	void AddCoordinates(Coordinate* c);
	int GetxCount();
	int GetyCount();
	void SetRange(Coordinate* c1);
	int MaxX = 0, MaxY = 0, MinX = 0, MinY = 0;
};
