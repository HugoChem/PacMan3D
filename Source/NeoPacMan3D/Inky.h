// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GhostBase.h"
#include "Inky.generated.h"


UCLASS()
class NEOPACMAN3D_API AInky : public AGhostBase
{
	GENERATED_BODY()

	UPROPERTY()
	const AGhostBase* LeaderGhost;

	static constexpr FLinearColor Cyan {0.f, 1.f, 1.f};

private:
	virtual void SetGhostProperties(bool reinstanceMaterial) override;

protected:
	virtual const UMazeTile* GetChaseTile() override;

public:
	void FollowGhost(const AGhostBase* leaderGhost)
	{ LeaderGhost = leaderGhost; }
};
