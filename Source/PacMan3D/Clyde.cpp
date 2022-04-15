// Fill out your copyright notice in the Description page of Project Settings.


#include "Clyde.h"

#include "PacMan.h"

void AClyde::BeginPlay()
{
	Super::BeginPlay();

	
	GhostMaterial->SetVectorParameterValue("FloorColor", Orange);
	GhostMaterial->SetVectorParameterValue("GrooveColor", Orange);
	GhostMaterial->SetVectorParameterValue("TrimColor", Orange);

	GhostMaterial->SetVectorParameterValue("GlowColor", Orange * 1000);
}

const UMazeTile* AClyde::GetChaseTile()
{
	if (FVector::Dist(**CurrentTile, **PacMan->CurrentTile) < ScaredDistance)
	{
		GhostMaterial->SetVectorParameterValue("GlowColor", FLinearColor::White * 5000);

		return ScatterTile;
	}
	else
	{
		GhostMaterial->SetVectorParameterValue("GlowColor", Orange * 1000);

		return PacMan->CurrentTile;
	}
}
