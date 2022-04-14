// Fill out your copyright notice in the Description page of Project Settings.


#include "Pinky.h"

#include "PacMan.h"


void APinky::BeginPlay()
{
	Super::BeginPlay();
	
	GhostMaterial->SetVectorParameterValue("FloorColor", Pink);
	GhostMaterial->SetVectorParameterValue("GrooveColor", Pink);
	GhostMaterial->SetVectorParameterValue("TrimColor", Pink);

	GhostMaterial->SetVectorParameterValue("GlowColor", Pink * 1000);
}


const UMazeTile* APinky::GetChaseTile(const UMazeTile* currentTile)
{
	return MazeManager->GetNeighborTile(PacMan->PacTile, PacMan->GetFacingDir(), 2);
}

