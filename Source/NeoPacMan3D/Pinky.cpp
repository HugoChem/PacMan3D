// Fill out your copyright notice in the Description page of Project Settings.


#include "Pinky.h"

#include "PacMan.h"

#include "GameFramework/RotatingMovementComponent.h"


void APinky::SetGhostProperties(const bool reinstanceMaterial)
{
	if (reinstanceMaterial)
		GhostMaterial = GhostMaterial = PyramidMesh->CreateDynamicMaterialInstance(0, OriginalMaterial);

	GhostMaterial->SetVectorParameterValue("FloorColor", Pink);
	GhostMaterial->SetVectorParameterValue("GrooveColor", Pink);
	GhostMaterial->SetVectorParameterValue("TrimColor", Pink);

	GhostMaterial->SetVectorParameterValue("GlowColor", Pink * 3000);

	Rotator->RotationRate = { 0, 60, 0 };
}

const UMazeTile* APinky::GetChaseTile()
{
	return MazeManager->GetNeighborTile(PacMan->GetCurrentTile(), PacMan->GetFacingDir(), PinkyAmbushDistance);
}

