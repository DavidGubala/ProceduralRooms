// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonController.h"
#include "Engine/World.h"

// Sets default values
ADungeonController::ADungeonController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADungeonController::BeginPlay()
{
	Super::BeginPlay();
	UWorld* world = GetWorld();

	if (world) {
		for (k = 0; k < 20; k++) {
			GenerateWallCount();
			if (!GenerateRoom(WallCount, RoomCount)) {
				k--; //  this will keep the loop running until we have a valid combination of rooms. 
			}
		}
		SpawnDungeon(world, RoomCount);
	}
}
// This Generates a random number of walls that will be used on one side of the room-> The rest of the room will be built using this WallCount and the GoldenRatio.
void ADungeonController::GenerateWallCount()
{
	RoomStyle = math.RandRange(0, 2);

	switch (RoomStyle) {
	case 0: // Little Room
		WallCount = math.RandRange(1, 3);
		break;
	case 1: // Medium Room
		WallCount = math.RandRange(3, 5);
		break;
	case 2: // Large Room
		WallCount = math.RandRange(6, 9);
		break;
	}
}


bool ADungeonController::GenerateRoom(int wc, int rc) // changed this to return a boolean.
{

	do {
		if (failCount == 2) {
			wc = 3;
			RefCoor = *Perimeter[(StartingLocation)];
		}
		else if (failCount == 3) {
			wc = 2;
			RefCoor = *Perimeter[(StartingLocation)];
		}
		else if (failCount == 4) {
			wc = 1;
			RefCoor = *Perimeter[(StartingLocation)];
		}
		else if (failCount > 4) {
			spawnFactor++;
			StartingLocation += spawnFactor;
			StartingLocation = (StartingLocation % Perimeter.Num());
			RefCoor = *Perimeter[(StartingLocation)];
		}

		room = new D_Room(wc);

		if (rc == 0) {
			RefCoor.pos.Set(0, 0, 0);
			RefCoor.WallSide = 3;
		}
		else if (failCount < 1) {
			StartingLocation = (floor(Perimeter.Num() * GoldenRatio) + PreviousLocation) - 2; // this will declare the starting position a 0.61 turn from the previous spawn point
			StartingLocation = (StartingLocation % Perimeter.Num());
			RefCoor = *Perimeter[(StartingLocation)];
		}

		if (!RefCoor.canBeDoor) {
			PreviousLocation++;
			delete room;
			return false;
		}

	} while (!RefCoor.canBeDoor);

	switch (RefCoor.WallSide) {
	case 1: // Flip Y + Flip X Left
		flipFactor_x1 = -1;
		flipFactor_x2 = 0;
		flipFactor_y1 = -1;
		flipFactor_y2 = 0;
		flipX = -1; // changed this
		flipY = -1;
		wall1 = (WallSize * room->GetyCount());
		wall2 = (WallSize * room->GetxCount());
		wall3 = 0;
		wall4 = 0;
		break;

	case 2: // Flip X Down
		flipFactor_x1 = -1;
		flipFactor_x2 = 0;
		flipFactor_y1 = 0;
		flipFactor_y2 = -1;
		flipX = -1;
		flipY = 1;
		wall1 = 0;
		wall2 = (WallSize * room->GetxCount());
		wall3 = (WallSize * room->GetyCount());
		wall4 = 0;
		break;

	case 3: // Right
		flipFactor_x1 = 0;
		flipFactor_x2 = -1;
		flipFactor_y1 = 0;
		flipFactor_y2 = -1;
		flipX = 1;
		flipY = 1;
		wall1 = 0;
		wall2 = 0;
		wall3 = (WallSize * room->GetyCount());
		wall4 = (WallSize * room->GetxCount());
		break;

	case 4: // Up
		flipFactor_x1 = 0;
		flipFactor_x2 = -1;
		flipFactor_y1 = -1;
		flipFactor_y2 = 0;
		flipX = 1;
		flipY = -1; // changed this
		wall1 = (WallSize * room->GetyCount());
		wall2 = 0;
		wall3 = 0;
		wall4 = (WallSize * room->GetxCount());
		break;
	}

	for (i = 1; i <= room->GetxCount(); i++) {

		SpawnCoor.pos.Set((RefCoor.pos.X + (WallSize * (i + flipFactor_x1)) * flipX), (RefCoor.pos.Y + (wall1 * flipY)), (RefCoor.pos.Z + 0));
		SpawnCoor.rot = FRotator::ZeroRotator;
		SpawnCoor.WallSide = 1;
		room->AddCoordinates(&SpawnCoor);

		SpawnCoor.pos.Set((RefCoor.pos.X + (WallSize * (i + flipFactor_x2)) * flipX), (RefCoor.pos.Y + (wall3 * flipY)), (RefCoor.pos.Z + 0));
		SpawnCoor.rot = FRotator(0.0f, 180.0f, 0.0f);
		SpawnCoor.WallSide = 3;
		room->AddCoordinates(&SpawnCoor);
	}

	for (i = 1; i <= room->GetyCount(); i++) {

		SpawnCoor.pos.Set((RefCoor.pos.X + (wall2 * flipX)), (RefCoor.pos.Y + (WallSize * (i + flipFactor_y2)) * flipY), (RefCoor.pos.Z + 0));
		SpawnCoor.rot = FRotator(0.0f, 270.0f, 0.0f);
		SpawnCoor.WallSide = 2;
		room->AddCoordinates(&SpawnCoor);

		SpawnCoor.pos.Set((RefCoor.pos.X + (wall4 * flipX)), (RefCoor.pos.Y + (WallSize * (i + flipFactor_y1)) * flipY), (RefCoor.pos.Z + 0));
		SpawnCoor.rot = FRotator(0.0f, 90.0f, 0.0f);
		SpawnCoor.WallSide = 4;
		room->AddCoordinates(&SpawnCoor);
	}

	if (rc > 0) { // Valid Room Check
		for (i = 0; i < rc; i++) {
			for (j = 0; j < room->coordinates.Num(); j++) {
				if (room->coordinates[j].pos.X < Rooms[i].MaxX && room->coordinates[j].pos.X > Rooms[i].MinX && room->coordinates[j].pos.Y < Rooms[i].MaxY && room->coordinates[j].pos.Y > Rooms[i].MinY) {
					delete room;
					failCount++;
					return false;
				}
			}
		}
	}

	roomPoints.Empty();

	for (l = 0; l < room->coordinates.Num(); l++) {
		roomPoints.Push(&room->coordinates[l]);
	}

	PerimeterSort(roomPoints, 0);
	sortedCoordinates.Empty();

	for (i = 0; i < SortedPerim.Num(); i++) {
		sortedCoordinates.Push(*SortedPerim[i]);
	}

	room->coordinates.Empty();

	for (i = 0; i < sortedCoordinates.Num(); i++) {
		room->coordinates.Push(sortedCoordinates[i]);
	}


	for (i = 0; i < room->coordinates.Num() - SortedPerim.Num(); i++) {
		room->coordinates.RemoveAt(i);
	}

	for (i = 0; i < room->coordinates.Num(); i++) {
		room->coordinates[i].next = &room->coordinates[((i + 1) % room->coordinates.Num())];
		if (i == 0) {
			room->coordinates[i].prev = &room->coordinates[room->coordinates.Num() - 1];
		}
		else {
			room->coordinates[i].prev = &room->coordinates[(i - 1)];
		}
	}

	if (rc > 0) {
		// Organize the rooms coordinates.
		m_placeholderN.Empty();
		m_placeholderP.Empty();
		m_NA.Empty();
		m_PA.Empty();

		for (i = 0; i < room->coordinates.Num(); i++) { // takes all repeating points
			for (j = 0; j < Perimeter.Num(); j++) {
				if (room->coordinates[i].pos == Perimeter[j]->pos) {
					m_placeholderN.Push(i);
					m_placeholderP.Push(j);
					break;
				}
			}
		}

		m_placeholderN.Sort();
		m_placeholderP.Sort();

		if (m_placeholderN.Num() == room->coordinates.Num() - 1) {
			delete room;
			failCount++;
			return false;
		}

		// check for multiple seperate room connections and sorts the matches in clockwise order
		i = 0;
		m_Scount = 0;
		do {
			if (i == 0 && m_Scount < 1) {
				if (room->coordinates[m_placeholderN[i]].prev == &room->coordinates[m_placeholderN[m_placeholderN.Num() - 1]]) {
					pon = 1;
					i = m_placeholderN.Num() - 1;
				}
				else {
					pon = 0;
					m_NA.Push(m_placeholderN[i]);
					i = (i + 1) % m_placeholderN.Num();
					m_Scount++;
				}
			}

			if (m_Scount == m_placeholderN.Num() - 1) {
				m_NA.Push(m_placeholderN[i]);
				m_Scount++;
			}
			else if (pon == 0) {
				j = (i + 1) % m_placeholderN.Num();
				if (room->coordinates[m_placeholderN[i]].next == &room->coordinates[m_placeholderN[j]]) {
					m_NA.Push(m_placeholderN[i]);
					i = (i + 1) % m_placeholderN.Num();
					m_Scount++;
				}
				else {
					delete room;
					failCount++;
					return false;
				}
			}
			else if (pon == 1) {
				if (room->coordinates[m_placeholderN[i]].prev == &room->coordinates[m_placeholderN[i - 1]]) {
					i--;
					m_Fcount++;
					if (m_Fcount >= m_placeholderN.Num()) {
						delete room;
						failCount++;
						return false;
					}
				}
				else {
					pon = 0;
					m_NA.Push(m_placeholderN[i]);
					i = (i + 1) % m_placeholderN.Num();
					m_Scount++;
				}
			}
		} while (!(m_Scount == m_placeholderN.Num()));

		i = 0;
		m_Scount = 0;
		do {
			if (i == 0 && m_Scount < 1) {
				if (Perimeter[m_placeholderP[i]]->prev->pos == Perimeter[m_placeholderP[m_placeholderP.Num() - 1]]->pos) {
					pon = 1;
					i = m_placeholderP.Num() - 1;
				}
				else {
					pon = 0;
					m_PA.Push(m_placeholderP[i]);
					i = (i + 1) % m_placeholderP.Num();
					m_Scount++;
				}
			}
			if (m_Scount == m_placeholderP.Num() - 1) {
				m_PA.Push(m_placeholderP[i]);
				m_Scount++;
			}
			else if (pon == 0) {
				j = (i + 1) % m_placeholderP.Num();
				if (Perimeter[m_placeholderP[i]]->next->pos == Perimeter[m_placeholderP[j]]->pos) {
					m_PA.Push(m_placeholderP[i]);
					i = (i + 1) % m_placeholderP.Num();
					m_Scount++;
				}
				else {
					delete room;
					failCount++;
					return false;
				}
			}
			else if (pon == 1) {
				if (Perimeter[m_placeholderP[i]]->prev->pos == Perimeter[m_placeholderP[i - 1]]->pos) {
					i--;
					m_Fcount++;
					if (m_Fcount >= m_placeholderN.Num()) {
						delete room;
						failCount++;
						return false;
					}
				}
				else {
					pon = 0;
					m_PA.Push(m_placeholderP[i]);
					i = (i + 1) % m_placeholderP.Num();
					m_Scount++;
				}
			}

		} while (!(m_Scount == m_placeholderP.Num()));

		switch (RefCoor.WallSide) {
		case 1:
			room->coordinates[m_NA[0]].next = NULL;

			for (i = 1; i < m_NA.Num(); i++) {
				room->coordinates[m_NA[i]].partOfPerimeter = false;
				room->coordinates[m_NA[i]].SpawnType = 2;
				removedPoints++;
			}

			for (i = 1; i < m_PA.Num(); i++) {
				removedPoints++;
				notPop.Add(Perimeter[m_PA[i]]);
				m_PA.RemoveAt(i);
				i--;
			}
			break;
		case 2:
			room->coordinates[m_NA[0]].next = NULL;

			for (i = 1; i < m_NA.Num(); i++) {
				room->coordinates[(m_NA[i] + 1) % room->coordinates.Num()].prev = NULL;
				room->coordinates[m_NA[i]].partOfPerimeter = false;
				room->coordinates[m_NA[i]].SpawnType = 2;
				removedPoints++;
			}
			for (i = 1; i < m_PA.Num(); i++) {
				notPop.Add(Perimeter[m_PA[i]]);
				removedPoints++;
				m_PA.RemoveAt(i);
				i--;
			}
			break;
		case 3:
			room->coordinates[m_NA[0]].next = NULL;

			for (i = 1; i < m_NA.Num(); i++) {
				room->coordinates[(m_NA[i] + 1) % room->coordinates.Num()].prev = NULL;
				room->coordinates[m_NA[i]].partOfPerimeter = false;
				room->coordinates[m_NA[i]].SpawnType = 2;
				removedPoints++;
			}
			for (i = 1; i < m_PA.Num(); i++) {
				notPop.Add(Perimeter[m_PA[i]]);
				removedPoints++;
				m_PA.RemoveAt(i);
				i--;
			}
			break;
		case 4:
			room->coordinates[m_NA[0]].next = NULL;

			for (i = 1; i < m_NA.Num(); i++) {
				room->coordinates[m_NA[i]].partOfPerimeter = false;
				room->coordinates[m_NA[i]].SpawnType = 2;
				removedPoints++;
			}
			for (i = 1; i < m_PA.Num(); i++) {
				notPop.Add(Perimeter[m_PA[i]]);
				removedPoints++;
				m_PA.RemoveAt(i);
				i--;
			}
			break;
		}

		for (i = 0; i < notPop.Num(); i++) {
			for (j = 0; j < rc; j++) {
				for (l = 0; l < Rooms[j].coordinates.Num(); l++) {

					if (notPop[i]->pos == Rooms[j].coordinates[l].pos && notPop[i]->rot == Rooms[j].coordinates[l].rot) {
						Rooms[j].coordinates[l].partOfPerimeter = false;
						if (l == 0) {
							Rooms[j].coordinates[Rooms[j].coordinates.Num() - 1].next = NULL;
						}
						else {
							Rooms[j].coordinates[l - 1].next = NULL;
						}
					}
				}
			}
		}
		notPop.Empty();
		for (j = 0; j < rc; j++) {
			for (l = 0; l < Rooms[j].coordinates.Num(); l++) {

				if (Rooms[j].coordinates[l].pos == Perimeter[m_PA[0]]->pos && Rooms[j].coordinates[l].rot == Perimeter[m_PA[0]]->rot) {
					switch (RefCoor.WallSide) {
					case 1:
						Rooms[j].coordinates[l].next = NULL;
						break;
					case 2:
						Rooms[j].coordinates[l].next = NULL;
						break;
					case 3:
						Rooms[j].coordinates[l].next = NULL;
						break;
					case 4:
						Rooms[j].coordinates[l].next = NULL;
						break;
					}
				}

				if (Rooms[j].coordinates[l].pos == RefCoor.pos && Rooms[j].coordinates[l].rot == RefCoor.rot) {
					Rooms[j].coordinates[l].SpawnType = 1; // this sets the spawning location start to be a door
				}
			}
		}

	}

	room->SetRange(&RefCoor);
	Rooms.Push(*room);
	RoomCount++;
	Perimeter.Empty();

	for (j = 0; j <= rc; j++) {  // this loop fills the perimeter with the coordinates from all rooms that are part of the perimeter.
		for (l = 0; l < Rooms[j].coordinates.Num(); l++) {
			if (Rooms[j].coordinates[l].partOfPerimeter) {
				Perimeter.Add(&Rooms[j].coordinates[l]);
			}
		}
	}

	PerimeterSort(Perimeter, 1);
	
	if (rc > 0) {
		if (!(SortedPerim == Perimeter)) {
			k += 20;
			return false;
		}
	}
	cantBeDoors.Empty();

	for (i = 0; i < Perimeter.Num(); i++) {
		if (Perimeter[i]->SpawnType == 1) {
			cantBeDoors.Add((i - 1) % Perimeter.Num());
			cantBeDoors.Add((i + 1) % Perimeter.Num());
		}
	}

	for (j = 0; j <= rc; j++) {  // this loop sets the new next and prev for points within the rooms
		for (l = 0; l < Rooms[j].coordinates.Num(); l++) {
			if (Rooms[j].coordinates[l].next == NULL) {
				for (i = 0; i < SortedPerim.Num(); i++) {
					if (SortedPerim[i]->pos == Rooms[j].coordinates[l].pos && SortedPerim[i]->rot == Rooms[j].coordinates[l].rot) {
						Rooms[j].coordinates[l].next = SortedPerim[i]->next;
					}
				}
			}
			else if (Rooms[j].coordinates[l].prev == NULL) {
				for (i = 0; i < SortedPerim.Num(); i++) {
					if (SortedPerim[i]->pos == Rooms[j].coordinates[l].pos && SortedPerim[i]->rot == Rooms[j].coordinates[l].rot) {
						Rooms[j].coordinates[l].prev = SortedPerim[i]->prev;
					}
				}
			}
			for (i = 0; i < cantBeDoors.Num(); i++) {
				if (Rooms[j].coordinates[l].pos == Perimeter[cantBeDoors[i]]->pos && Rooms[j].coordinates[l].rot == Perimeter[cantBeDoors[i]]->rot) {
					Rooms[j].coordinates[l].canBeDoor = false;
				}
			}
		}
	}

	if (rc > 0) {
		for (i = 0; i < Perimeter.Num(); i++) {
			if (Perimeter[i]->pos == RefCoor.pos) {
				PreviousLocation = i;
			}
		}
	}

	removedPoints = 0;
	spawnFactor = 0;
	failCount = 0;
	return true;
}
void ADungeonController::SpawnDungeon(UWorld* w, int rc)
{
	FActorSpawnParameters spawnParameters;
	spawnParameters.Owner = this;
	spawnParameters.Instigator = Instigator;

	for (i = 0; i < rc; i++) {

		for (j = 0; j < Rooms[i].coordinates.Num(); j++) { // Spawning
			switch (Rooms[i].coordinates[j].SpawnType) {
			case 0:
				w->SpawnActor<class AD_Wall>(dungeonWall, Rooms[i].coordinates[j].pos, Rooms[i].coordinates[j].rot, spawnParameters);
				w->SpawnActor<class AD_Pillar>(dungeonPillar, Rooms[i].coordinates[j].pos, Rooms[i].coordinates[j].rot, spawnParameters);
				
				x = math.RandRange(1, 1000);
				if (x > 618 && y < 618) {
					w->SpawnActor<class AD_torch>(dungeonTorch, Rooms[i].coordinates[j].pos, Rooms[i].coordinates[j].rot, spawnParameters);
				}
				y = x;

				break;
			case 1:
				w->SpawnActor<class AD_Door>(dungeonDoor, Rooms[i].coordinates[j].pos, Rooms[i].coordinates[j].rot, spawnParameters);
				break;
			}
		}

		for (x = Rooms[i].MinX; x < Rooms[i].MaxX; x+=500) {
			for (y = Rooms[i].MinY; y < Rooms[i].MaxY; y += 500) {

				if (!playerSpawned) {
					j = math.RandRange(1, 1000);
					if (j > 618) {
						w->SpawnActor<class AActor>(playerSpawn, FVector(x + 250, y + 250, 50), FRotator::ZeroRotator, spawnParameters);
						playerSpawned = true;
					}
					else {
						w->SpawnActor<class AD_Ceiling>(dungeonCeiling, FVector(x + 500, y + 500, 450), FRotator::ZeroRotator, spawnParameters);
						w->SpawnActor<class AD_Floor>(dungeonFloor, FVector(x + 500, y + 500, 0), FRotator::ZeroRotator, spawnParameters);
					}
				}
				else {
					w->SpawnActor<class AD_Ceiling>(dungeonCeiling, FVector(x + 500, y + 500, 450), FRotator::ZeroRotator, spawnParameters);
					w->SpawnActor<class AD_Floor>(dungeonFloor, FVector(x + 500, y + 500, 0), FRotator::ZeroRotator, spawnParameters);
				}
			}
		}
	}

}

void ADungeonController::PerimeterSort(TArray<Coordinate*> p, int roomOrDungeon) {
	bool completed = false, wallDone = false;
	int pointsLeft = 0;

	// room = 0 dungeon = 1
	SortedPerim.Empty();

	int direction;

	if (roomOrDungeon == 1) {
		if (p[0]->WallSide == p[1]->WallSide) {
			SortedPerim.Add(p[0]);
			p.RemoveAt(0);
			direction = SortedPerim[0]->WallSide;
		}
		else {
			SortedPerim.Add(p[1]);
			p.RemoveAt(1);
			direction = SortedPerim[0]->WallSide;
		}
		do {
			if (SortedPerim[SortedPerim.Num() - 1]->next != NULL) {
				for (i = 0; i < p.Num(); i++) {
					if (p[i]->pos == SortedPerim[SortedPerim.Num() - 1]->next->pos && p[i]->rot == SortedPerim[SortedPerim.Num() - 1]->next->rot) {
						SortedPerim.Add(p[i]);
						p.RemoveAt(i);
						break;
					}
				}
			}
			else {
				for (i = 0; i < p.Num(); i++) {
					if (p[i]->prev == NULL) {
						if ((p[i]->pos.Y - SortedPerim[SortedPerim.Num() - 1]->pos.Y == 500 || p[i]->pos.Y - SortedPerim[SortedPerim.Num() - 1]->pos.Y == -500) || (p[i]->pos.X - SortedPerim[SortedPerim.Num() - 1]->pos.X == 500 || p[i]->pos.X - SortedPerim[SortedPerim.Num() - 1]->pos.X == -500)) {
							SortedPerim[SortedPerim.Num() - 1]->next = p[i];
							SortedPerim.Add(p[i]);
							p.RemoveAt(i);
							SortedPerim[SortedPerim.Num() - 1]->prev = SortedPerim[SortedPerim.Num() - 2];
							break;
						}
					}
					else if (p[i]->prev->pos == SortedPerim[SortedPerim.Num() - 1]->pos) {
						SortedPerim[SortedPerim.Num() - 1]->next = p[i];
						SortedPerim.Add(p[i]);
						p.RemoveAt(i);
						SortedPerim[SortedPerim.Num() - 1]->prev = SortedPerim[SortedPerim.Num() - 2];
						break;
					}
				}
			}

			if (p.Num() == 0 && ((SortedPerim[0]->pos.X - SortedPerim[SortedPerim.Num() - 1]->pos.X == 500 || SortedPerim[0]->pos.X - SortedPerim[SortedPerim.Num() - 1]->pos.X == -500) || (SortedPerim[0]->pos.Y - SortedPerim[SortedPerim.Num() - 1]->pos.Y == 500 || SortedPerim[0]->pos.Y - SortedPerim[SortedPerim.Num() - 1]->pos.Y == -500))) {
				completed = true;
			}
			failCount++;
			if (failCount > 4000) {
				break;
			}
		} while (!completed);
		Perimeter = SortedPerim;
	}
	else {
		switch (RefCoor.WallSide) {
		case 1:
			SortedPerim.Add(p[1]);
			p.RemoveAt(1);
			direction = 3;
			break;
		case 2:
			SortedPerim.Add(p[0]);
			p.RemoveAt(0);
			direction = 4;
			break;
		case 3:
			SortedPerim.Add(p[0]);
			p.RemoveAt(0);
			direction = 1;
			break;
		case 4:
			SortedPerim.Add(p[1]);
			p.RemoveAt(1);
			direction = 2;
			break;
		}
		do { // the perimeter sorting loop
			wallDone = false;
			switch (direction) {// depending on the side of the wall the point is on. 
			case 1:
				do { //  the wall sorting loop
					for (i = 0; i < p.Num(); i++) {
						if (p[i]->pos.Y == SortedPerim[SortedPerim.Num() - 1]->pos.Y && p[i]->WallSide == 1) { //SortedPerim[SortedPerim.Num() - 1].WallSide) {
							if (p[i]->pos.X - SortedPerim[SortedPerim.Num() - 1]->pos.X == 500) { // || p[i].pos.X - SortedPerim[SortedPerim.Num() - 1].pos.X == -500) {
								SortedPerim.Add(p[i]); // adds the next point in order
								p.RemoveAt(i);
								i--;
							}
							else {
								pointsLeft++;
							}
						}
					}

					if (pointsLeft == 0) {
						wallDone = true;
						// here we need to find the next point and the direction for the following loop

						for (i = 0; i < p.Num(); i++) {
							if (p[i]->pos.X == SortedPerim[SortedPerim.Num() - 1]->pos.X) {
								if ((p[i]->pos.Y - SortedPerim[SortedPerim.Num() - 1]->pos.Y == 500 || p[i]->pos.Y - SortedPerim[SortedPerim.Num() - 1]->pos.Y == -500) && (p[i]->WallSide == 2 || p[i]->WallSide == 4)) {
									SortedPerim.Add(p[i]); // adds the next point in order
									p.RemoveAt(i);
									direction = SortedPerim[SortedPerim.Num() - 1]->WallSide; // this will be either 2 or 4;
									break;
								}
							}
						}

					}
					pointsLeft = 0;
				} while (!wallDone);
				break;
			case 2:
				do { //  the wall sorting loop
					for (i = 0; i < p.Num(); i++) {
						if (p[i]->pos.X == SortedPerim[SortedPerim.Num() - 1]->pos.X && p[i]->WallSide == 2) { //SortedPerim[SortedPerim.Num() - 1].WallSide) {
							if (p[i]->pos.Y - SortedPerim[SortedPerim.Num() - 1]->pos.Y == -500) { //|| p[i].pos.Y - SortedPerim[SortedPerim.Num() - 1].pos.Y == -500) {
								SortedPerim.Add(p[i]); // adds the next point in order
								p.RemoveAt(i);
								i--;
							}
							else {
								pointsLeft++;
							}
						}
					}

					if (pointsLeft == 0) {
						wallDone = true;
						// here we need to find the next point and the direction for the following loop

						for (i = 0; i < p.Num(); i++) {
							if (p[i]->pos.Y == SortedPerim[SortedPerim.Num() - 1]->pos.Y) {
								if ((p[i]->pos.X - SortedPerim[SortedPerim.Num() - 1]->pos.X == 500 || p[i]->pos.X - SortedPerim[SortedPerim.Num() - 1]->pos.X == -500) && (p[i]->WallSide == 1 || p[i]->WallSide == 3)) {
									SortedPerim.Add(p[i]); // adds the next point in order
									p.RemoveAt(i);
									direction = SortedPerim[SortedPerim.Num() - 1]->WallSide; // this will be either 2 or 4;
									break;
								}
							}
						}

					}

					pointsLeft = 0;
				} while (!wallDone);
				break;

			case 3:
				do { //  the wall sorting loop
					for (i = 0; i < p.Num(); i++) {
						if (p[i]->pos.Y == SortedPerim[SortedPerim.Num() - 1]->pos.Y && p[i]->WallSide == 3) { //SortedPerim[SortedPerim.Num() - 1].WallSide) {
							if (p[i]->pos.X - SortedPerim[SortedPerim.Num() - 1]->pos.X == -500) { // || p[i].pos.X - SortedPerim[SortedPerim.Num() - 1].pos.X == 500) {
								SortedPerim.Add(p[i]); // adds the next point in order
								p.RemoveAt(i);
								i--;
							}
							else {
								pointsLeft++;
							}
						}
					}

					if (pointsLeft == 0) {
						wallDone = true;
						// here we need to find the next point and the direction for the following loop

						for (i = 0; i < p.Num(); i++) {
							if (p[i]->pos.X == SortedPerim[SortedPerim.Num() - 1]->pos.X) {
								if ((p[i]->pos.Y - SortedPerim[SortedPerim.Num() - 1]->pos.Y == 500 || p[i]->pos.Y - SortedPerim[SortedPerim.Num() - 1]->pos.Y == -500) && (p[i]->WallSide == 2 || p[i]->WallSide == 4)) {
									SortedPerim.Add(p[i]); // adds the next point in order
									p.RemoveAt(i);
									direction = SortedPerim[SortedPerim.Num() - 1]->WallSide; // this will be either 2 or 4;
									break;
								}
							}
						}
					}
					pointsLeft = 0;
				} while (!wallDone);
				break;

			case 4:
				do { //  the wall sorting loop
					for (i = 0; i < p.Num(); i++) {
						if (p[i]->pos.X == SortedPerim[SortedPerim.Num() - 1]->pos.X && p[i]->WallSide == 4) { //SortedPerim[SortedPerim.Num() - 1].WallSide) {
							if (p[i]->pos.Y - SortedPerim[SortedPerim.Num() - 1]->pos.Y == 500) { // || p[i].pos.Y - SortedPerim[SortedPerim.Num() - 1].pos.Y == -500) {
								SortedPerim.Add(p[i]); // adds the next point in order
								p.RemoveAt(i);
								i--;
							}
							else {
								pointsLeft++;
							}
						}
					}

					if (pointsLeft == 0) {
						wallDone = true;
						// here we need to find the next point and the direction for the following loop

						for (i = 0; i < p.Num(); i++) {
							if (p[i]->pos.Y == SortedPerim[SortedPerim.Num() - 1]->pos.Y) {
								if ((p[i]->pos.X - SortedPerim[SortedPerim.Num() - 1]->pos.X == 500 || p[i]->pos.X - SortedPerim[SortedPerim.Num() - 1]->pos.X == -500) && (p[i]->WallSide == 1 || p[i]->WallSide == 3)) {
									SortedPerim.Add(p[i]); // adds the next point in order
									p.RemoveAt(i);
									direction = SortedPerim[SortedPerim.Num() - 1]->WallSide; // this will be either 2 or 4;
									break;
								}
							}
						}

					}
					pointsLeft = 0;
				} while (!wallDone);
				break;
			}

			if (p.Num() == 0 && ((SortedPerim[0]->pos.X - SortedPerim[SortedPerim.Num() - 1]->pos.X == 500 || SortedPerim[0]->pos.X - SortedPerim[SortedPerim.Num() - 1]->pos.X == -500) || (SortedPerim[0]->pos.Y - SortedPerim[SortedPerim.Num() - 1]->pos.Y == 500 || SortedPerim[0]->pos.Y - SortedPerim[SortedPerim.Num() - 1]->pos.Y == -500))) {
				completed = true;
			}
		} while (!completed);
	}


}

// Called every frame
void ADungeonController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

