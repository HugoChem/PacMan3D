// Fill out your copyright notice in the Description page of Project Settings.


#include "Inky.h"

#include "PacMan.h"

#include "GameFramework/RotatingMovementComponent.h"


void AInky::SetGhostProperties(const bool reinstanceMaterial)
{
	if (reinstanceMaterial)
		GhostMaterial = GhostMaterial = PyramidMesh->CreateDynamicMaterialInstance(0, OriginalMaterial);

	GhostMaterial->SetVectorParameterValue("FloorColor", Cyan);
	GhostMaterial->SetVectorParameterValue("GrooveColor", Cyan);
	GhostMaterial->SetVectorParameterValue("TrimColor", Cyan);

	GhostMaterial->SetVectorParameterValue("GlowColor", Cyan * 3000.f);

	Rotator->RotationRate = { 0, 60, 0 };
}

const UMazeTile* AInky::GetChaseTile()
{
	const UMazeTile* pivotTile = MazeManager->GetNeighborTile(PacMan->GetCurrentTile(), PacMan->GetFacingDir(), 2);

	return MazeManager->GetNearestTile(**pivotTile + (**pivotTile - **LeaderGhost->CurrentTile));
}