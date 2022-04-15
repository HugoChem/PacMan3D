// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GhostBase.h"
#include "Blinky.generated.h"


UCLASS()
class PACMAN3D_API ABlinky : public AGhostBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	
	virtual const UMazeTile* GetChaseTile() override;
	
};
