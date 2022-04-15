// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

#include "MazeManager.h"

#include "PacMan.generated.h"


UCLASS()
class PACMAN3D_API APacMan : public APawn
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess, ExposeOnSpawn="true"))
	AMazeManager* MazeManager;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess, ClampMin = 0, UIMax = 5))
	// Consider this as a "how many tiles per seconds".
	float Speed = 1;
	
	
	// When this reaches 1, it is set back to 0 and pac-man is snapped to the nearest tile.
	// It is at this point the next direction is applied (if valid).
	float TileProgress = 0;

	Direction::CardinalDirection CurrentDir = Direction::None;

	Direction::CardinalDirection NextDir = Direction::None;
	
public:
	// Sets default values for this pawn's properties
	APacMan();
	
	UPROPERTY()
	// Last known tile PacMan was on.
	const UMazeTile* CurrentTile;
	
	//The direction the player is facing
	Direction::CardinalDirection GetFacingDir() const
	{ return CurrentDir != Direction::None ? CurrentDir : NextDir; }
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	const class UMazeTile* SnapAndMove();

	// Logical decision that makes pac man move immediately if possible.
	void ApplyMoveCheck(const Direction::CardinalDirection& dir);
	
	void MoveUp();
	void MoveRight();
	void MoveDown();
	void MoveLeft();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
