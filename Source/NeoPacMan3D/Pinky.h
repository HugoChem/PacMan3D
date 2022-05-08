// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GhostBase.h"
#include "Pinky.generated.h"


UCLASS()
class NEOPACMAN3D_API APinky : public AGhostBase
{
	GENERATED_BODY()

	static constexpr FLinearColor Pink {1.f, 0.f, 0.5f};
	
	static constexpr int PinkyAmbushDistance = 4;

private:
	virtual void SetGhostProperties(bool reinstanceMaterial) override;

protected:
	virtual const UMazeTile* GetChaseTile() override;
};
