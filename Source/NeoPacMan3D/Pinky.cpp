// Fill out your copyright notice in the Description page of Project Settings.


#include "Pinky.h"

#include "PacMan.h"


void APinky::BeginPlay()
{
	Super::BeginPlay();
	
	GhostMaterial->SetVectorParameterValue("FloorColor", Pink);
	GhostMaterial->SetVectorParameterValue("GrooveColor", Pink);
	GhostMaterial->SetVectorParameterValue("TrimColor", Pink);

	GhostMaterial->SetVectorParameterValue("GlowColor", Pink * 3000);
}


const UMazeTile* APinky::GetChaseTile()
{
	return MazeManager->GetNeighborTile(PacMan->CurrentTile, PacMan->GetFacingDir(), PinkyAmbushDistance);
}

