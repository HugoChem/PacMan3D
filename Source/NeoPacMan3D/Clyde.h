// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GhostBase.h"
#include "Clyde.generated.h"

/**
 * 
 */
UCLASS()
class NEOPACMAN3D_API AClyde : public AGhostBase
{
	GENERATED_BODY()

	static constexpr FLinearColor Orange  = {1.f, 0.5f, 0.0f};
	//static constexpr FLinearColor Yellish = {1.f, 1.0f, 0.5f};

	static constexpr float ScaredDistance = 400.f;

private:
	virtual void SetGhostProperties(bool reinstanceMaterial) override;
	
protected:
	virtual const UMazeTile* GetChaseTile() override;
};
