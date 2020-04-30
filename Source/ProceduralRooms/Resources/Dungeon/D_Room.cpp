// Fill out your copyright notice in the Description page of Project Settings.


#include "D_Room.h"
#include <ctime>

D_Room::D_Room(int WallCount)
{
	i = math.RandRange(0, 1);
	if (WallCount == 1) {
		xWallCount = WallCount;
		yWallCount = WallCount;
	}
	else {
		if (i == 0) {
			xWallCount = WallCount;
			yWallCount = floor(WallCount * GoldenRatio);
		}
		else {
			xWallCount = floor(WallCount * GoldenRatio);
			yWallCount = WallCount;
		}
		coordinateCount = ((2 * xWallCount) + (2 * yWallCount));
	}
}

D_Room::~D_Room()
{
}

void D_Room::AddCoordinates(Coordinate* c)
{
	this->coordinates.Push(*c);
}

int D_Room::GetxCount()
{
	return xWallCount;
}

int D_Room::GetyCount()
{
	return yWallCount;
}

void D_Room::SetRange(Coordinate* c1)
{
	Coordinate c = *c1;
	switch (c.WallSide) {
	case 1:
		MinX = c.pos.X - (xWallCount * 500);
		MinY = c.pos.Y - ((yWallCount * 500));
		MaxX = c.pos.X;
		MaxY = c.pos.Y;
		break;
	case 2:
		MinX = c.pos.X - (xWallCount * 500);
		MinY = c.pos.Y;
		MaxX = c.pos.X;
		MaxY = c.pos.Y + (yWallCount * 500);
		break;
	case 3:
		MinX = c.pos.X;
		MinY = c.pos.Y;
		MaxX = c.pos.X + (xWallCount * 500);
		MaxY = c.pos.Y + (yWallCount * 500);
		break;
	case 4:
		MinX = c.pos.X;
		MinY = c.pos.Y - (yWallCount * 500);
		MaxX = c.pos.X + (xWallCount * 500);
		MaxY = c.pos.Y;
		break;
	}
}

