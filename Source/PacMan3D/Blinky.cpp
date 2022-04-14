// Fill out your copyright notice in the Description page of Project Settings.


#include "Blinky.h"

#include "PacMan.h"


void ABlinky::BeginPlay()
{
	Super::BeginPlay();

	GhostMaterial->SetVectorParameterValue("FloorColor", FLinearColor::Red);
	GhostMaterial->SetVectorParameterValue("GrooveColor", FLinearColor::Red);
	GhostMaterial->SetVectorParameterValue("TrimColor", FLinearColor::Red);

	GhostMaterial->SetVectorParameterValue("GlowColor", FLinearColor(1000.f, 0.f, 0.f));
}

const UMazeTile* ABlinky::GetChaseTile(const UMazeTile* currentTile)
{
	return PacMan->PacTile;
}
