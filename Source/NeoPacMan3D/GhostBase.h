// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "MazeManager.h"

#include "GhostBase.generated.h"


enum class GhostState { Waiting, Exiting, Chase, Scatter, Frightened, Eaten };


UCLASS(Abstract)
class NEOPACMAN3D_API AGhostBase : public AActor
{
	GENERATED_BODY()

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	USceneComponent* Root;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess))
	float SpawnWaitingTime = 10;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess, ClampMin = 0, UIMax = 5))
	// Consider this as a "how many tiles per seconds".
	float Speed = 3.f;
	
	// When this reaches 1, it is set back to 0 and the ghost is snapped to the nearest tile.
	// It is at this point the ghost's core logic determines the new path to take.
	float TileProgress = 0;

	// Handle for the frightened state.
	FTimerHandle FrightenedHandle;

	// Separate handle for the color blinking.
	FTimerHandle ColorBlinkHandle;
	

	GhostState CurrentState = GhostState::Waiting;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* PyramidMesh;
	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UMaterialInterface* OriginalMaterial;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UMaterialInterface* FrightenedMaterial;

	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	UMaterialInstanceDynamic* GhostMaterial;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class URotatingMovementComponent* Rotator;

	
	
	UPROPERTY(BlueprintReadOnly, meta=(ExposeOnSpawn="true"))
	AMazeManager* MazeManager;


	UPROPERTY()
	const UMazeTile* SpawnTile	= nullptr;

	UPROPERTY()
	const UMazeTile* ExitTile	= nullptr;

	UPROPERTY()
	const UMazeTile* ScatterTile = nullptr;

	
	UPROPERTY(BlueprintReadOnly, meta=(ExposeOnSpawn="true"))
	const class APacMan* PacMan;

	Direction::CardinalDirection CurrentDir = Direction::None;

public:
	UPROPERTY()
	const UMazeTile* CurrentTile;
	
	void SetGhostWaitingTime(const float waitFor)
	{ SpawnWaitingTime = waitFor; }
	
	void SetGhostInfos(	const int spawnWait,
						const UMazeTile* spawnTile,
						const UMazeTile* exitTile,
						const UMazeTile* scatterTile)
	{
		SpawnWaitingTime = spawnWait;

		check(!SpawnTile || !ExitTile || !ScatterTile)
		SpawnTile = spawnTile, ExitTile = exitTile, ScatterTile = scatterTile;
		CurrentTile = SpawnTile;
	}

	
	// Sets default values for this actor's properties
	AGhostBase();

	
private:
	// Unique to each ghosts, and each overload!
	virtual void SetGhostProperties(bool reinstanceMaterial)
	{ checkf(false, TEXT("Should not reach here!")); };
	
	// Returns TRUE as soon as the ghost is free to go.
	bool CountDownSpawnTimer(const float deltaTime);


	const UMazeTile* SnapToClosestTile();

	virtual const UMazeTile* ProcessStatusTile();
	
	virtual const UMazeTile* GetChaseTile()
	{ checkf(false, TEXT("Should not reach here!")); return nullptr; }
	
	
	// Stuck or starting
	Direction::CardinalDirection DetermineStartingDirection();
	
	// Already in motion
	Direction::CardinalDirection DetermineNewDirection();

	// Frightened
	Direction::CardinalDirection RandomDirection();


	void BlueFrighten();
	void WhiteFrighten();

	void EndFrighten();

protected:
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	// Frightens the ghost for a set period of time.
	void Frighten(float duration = 10.f);

	// Eats the ghost.
	void Eat();
};
