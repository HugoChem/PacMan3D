// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GhostBase.h"
#include "Inky.generated.h"


UCLASS()
class PACMAN3D_API AInky : public AGhostBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	
	virtual const UMazeTile* GetChaseTile(const UMazeTile* currentTile) override;
};
