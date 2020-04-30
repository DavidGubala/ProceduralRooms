// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Assets/D_Ceiling.h"
#include "Assets/D_Door.h"
#include "Assets/D_Floor.h"
#include "Assets/D_Pillar.h"
#include "Assets/D_Wall.h"
#include "Assets/D_torch.h"
#include "D_Room.h"
#include "Algo/Reverse.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DungeonController.generated.h"

UCLASS()
class PROCEDURALROOMS_API ADungeonController : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADungeonController();
	float GoldenRatio = ((1 + sqrt(5)) / 2) - 1;
	int WallSize = 500;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	struct FMath math;
	void GenerateWallCount();
	bool GenerateRoom(int wc, int rc);
	void SpawnDungeon(UWorld* world, int rc);
	void PerimeterSort(TArray<Coordinate*> p, int roomOrDungeon);
	bool playerSpawned = false;
	int i, j, k, l, x, y, failCount = 0, pon, m_Scount = 0, m_Fcount = 0;
	int WallCount, RoomCount, RoomStyle, PreviousLocation = 0, StartingLocation = 0, removedPoints = 0;
	int flipFactor_x1 = 0,
		flipFactor_x2 = 0,
		flipFactor_y1 = 0,
		flipFactor_y2 = 0,
		flipX = 0,
		flipY = 0,
		wall1 = 0,
		wall2 = 0,
		wall3 = 0,
		wall4 = 0,
		spawnFactor = 0;
	D_Room* room;
	//match* m;
	//TArray<match> matches;
	TArray<int>
		m_NA,
		m_placeholderN,
		m_PA,
		m_placeholderP,
		cantBeDoors;
	TArray<D_Room> Rooms;
	TArray<Coordinate> sortedCoordinates;
	TArray<Coordinate*> roomPoints, Perimeter, SortedPerim, notPop;
	Coordinate SpawnCoor, RefCoor;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(EditAnywhere)
		TSubclassOf<AD_Wall> dungeonWall;
	UPROPERTY(EditAnywhere)
		TSubclassOf<AD_Pillar> dungeonPillar;
	UPROPERTY(EditAnywhere)
		TSubclassOf<AD_Door> dungeonDoor;
	UPROPERTY(EditAnywhere)
		TSubclassOf<AD_Ceiling> dungeonCeiling;
	UPROPERTY(EditAnywhere)
		TSubclassOf<AD_Floor> dungeonFloor;
	UPROPERTY(EditAnywhere)
		TSubclassOf<AD_torch> dungeonTorch;
	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> playerSpawn;
};
struct match {
public:
	int nr, p; // new room and perimeter
	match(int m_NewRoom, int m_Perimeter) {
		nr = m_NewRoom;
		p = m_Perimeter;
	}
};
