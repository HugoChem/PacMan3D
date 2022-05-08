// Fill out your copyright notice in the Description page of Project Settings.


#include "Blinky.h"

#include "PacMan.h"

#include "GameFramework/RotatingMovementComponent.h"


void ABlinky::SetGhostProperties(const bool reinstanceMaterial)
{
	if (reinstanceMaterial)
		GhostMaterial = GhostMaterial = PyramidMesh->CreateDynamicMaterialInstance(0, OriginalMaterial);
	
	GhostMaterial->SetVectorParameterValue("FloorColor", FLinearColor::Red);
	GhostMaterial->SetVectorParameterValue("GrooveColor", FLinearColor::Red);
	GhostMaterial->SetVectorParameterValue("TrimColor", FLinearColor::Red);

	GhostMaterial->SetVectorParameterValue("GlowColor", FLinearColor::Red * 3000.f);

	Rotator->RotationRate = { 0, 60, 0 };
}

const UMazeTile* ABlinky::GetChaseTile()
{
	return PacMan->GetCurrentTile();
}
