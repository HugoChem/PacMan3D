// Fill out your copyright notice in the Description page of Project Settings.


#include "Inky.h"

#include "PacMan.h"

void AInky::BeginPlay()
{
	Super::BeginPlay();

	constexpr FLinearColor cyan(0.f, 1.f, 1.f);
	
	GhostMaterial->SetVectorParameterValue("FloorColor", cyan);
	GhostMaterial->SetVectorParameterValue("GrooveColor", cyan);
	GhostMaterial->SetVectorParameterValue("TrimColor", cyan);

	GhostMaterial->SetVectorParameterValue("GlowColor", cyan * 1000);
}

const UMazeTile* AInky::GetChaseTile()
{
	const UMazeTile* pivotTile = MazeManager->GetNeighborTile(PacMan->CurrentTile, PacMan->GetFacingDir(), 2);

	return MazeManager->GetNearestTile(**pivotTile + (**pivotTile - **LeaderGhost->CurrentTile));
}