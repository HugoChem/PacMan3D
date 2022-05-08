// Fill out your copyright notice in the Description page of Project Settings.


#include "Clyde.h"

#include "PacMan.h"

#include "GameFramework/RotatingMovementComponent.h"


void AClyde::SetGhostProperties(const bool reinstanceMaterial)
{
	if (reinstanceMaterial)
		GhostMaterial = GhostMaterial = PyramidMesh->CreateDynamicMaterialInstance(0, OriginalMaterial);

	GhostMaterial->SetVectorParameterValue("FloorColor", Orange);
	GhostMaterial->SetVectorParameterValue("GrooveColor", Orange);
	GhostMaterial->SetVectorParameterValue("TrimColor", Orange);

	GhostMaterial->SetVectorParameterValue("GlowColor", Orange * 3000);

	Rotator->RotationRate = { 0, 60, 0 };
}

const UMazeTile* AClyde::GetChaseTile()
{
	if (FVector::Dist(**CurrentTile, **PacMan->GetCurrentTile()) < ScaredDistance)
	{
		GhostMaterial->SetVectorParameterValue("GlowColor", FLinearColor::White * 5000);

		return ScatterTile;
	}
	else
	{
		GhostMaterial->SetVectorParameterValue("GlowColor", Orange * 1000);

		return PacMan->GetCurrentTile();
	}
}
