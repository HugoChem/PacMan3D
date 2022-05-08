// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GhostBase.h"
#include "Blinky.generated.h"


UCLASS()
class NEOPACMAN3D_API ABlinky : public AGhostBase
{
	GENERATED_BODY()

private:
	virtual void SetGhostProperties(bool reinstanceMaterial) override;

protected:
	virtual const UMazeTile* GetChaseTile() override;
	
};
