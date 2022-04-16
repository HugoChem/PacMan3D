// Fill out your copyright notice in the Description page of Project Settings.


#include "Blinky.h"

#include "PacMan.h"


void ABlinky::BeginPlay()
{
	Super::BeginPlay();

	GhostMaterial->SetVectorParameterValue("FloorColor", FLinearColor::Red);
	GhostMaterial->SetVectorParameterValue("GrooveColor", FLinearColor::Red);
	GhostMaterial->SetVectorParameterValue("TrimColor", FLinearColor::Red);

	GhostMaterial->SetVectorParameterValue("GlowColor", FLinearColor::Red * 100000.0);
}

const UMazeTile* ABlinky::GetChaseTile()
{
	return PacMan->CurrentTile;
}
