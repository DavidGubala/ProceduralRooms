// Fill out your copyright notice in the Description page of Project Settings.


#include "D_Floor.h"

// Sets default values
AD_Floor::AD_Floor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AD_Floor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AD_Floor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

