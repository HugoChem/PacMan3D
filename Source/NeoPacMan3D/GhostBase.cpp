// Fill out your copyright notice in the Description page of Project Settings.


#include "GhostBase.h"

#include "PacMan.h"
#include "GameFramework/RotatingMovementComponent.h"

// Sets default values
AGhostBase::AGhostBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>("GhostRoot");
	SetRootComponent(Root);

	
	PyramidMesh = CreateDefaultSubobject<UStaticMeshComponent>("GhostlyPyramid");
	PyramidMesh->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);
	PyramidMesh->SetRelativeLocation(FVector::ZeroVector);
	PyramidMesh->SetRelativeScale3D(FVector(0.5f));
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> mesh(TEXT("/Engine/VREditor/BasicMeshes/SM_Pyramid_01.SM_Pyramid_01"));
	if (mesh.Succeeded())
		PyramidMesh->SetStaticMesh(mesh.Object);

	
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> coreMaterial(TEXT("/Game/Ghosts/GhostMaterial.GhostMaterial"));
	if (coreMaterial.Succeeded())
	{
		OriginalMaterial = coreMaterial.Object;
		PyramidMesh->SetMaterial(0, coreMaterial.Object);
	}
	
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> frightenedMaterial(TEXT("/Engine/EngineDebugMaterials/WireframeMaterial.WireframeMaterial"));
	if (frightenedMaterial.Succeeded())
		FrightenedMaterial = frightenedMaterial.Object;

	
	Rotator = CreateDefaultSubobject<URotatingMovementComponent>("Rotator");
	Rotator->bRotationInLocalSpace = false;
}


bool AGhostBase::CountDownSpawnTimer(const float deltaTime)
{
	if (CurrentState == GhostState::Waiting)
	{
		SpawnWaitingTime -= deltaTime;
		
		if (SpawnWaitingTime <= 0)
		{
			CurrentState = GhostState::Exiting;
			return true;
		}

		return false;
	}

	return true;
}


const UMazeTile* AGhostBase::SnapToClosestTile()
{
	const UMazeTile* currentTile = MazeManager->GetNearestTile(GetActorLocation());
	SetActorLocation(**currentTile);

	TileProgress = 0;

	check(currentTile)
	CurrentTile = currentTile;
	return currentTile;
}

const UMazeTile* AGhostBase::ProcessStatusTile()
{
	switch (CurrentState)
	{
	case GhostState::Waiting: return SpawnTile;

	case GhostState::Exiting:
		if (CurrentTile == ExitTile)
		{
			CurrentState = GhostState::Chase;
			return GetChaseTile();
		}
		else return ExitTile;

		
	case GhostState::Chase:			return GetChaseTile();
	case GhostState::Scatter:		return ScatterTile;

	case GhostState::Frightened:	checkf(false, TEXT("Shouldn't be here..")) return nullptr;


	case GhostState::Eaten:
		if (CurrentTile == SpawnTile)
		{
			CurrentState = GhostState::Exiting;
			Speed /= 2;
			SetGhostProperties(true);
			return ExitTile;
		}
		else return SpawnTile;

		
	default: return nullptr;
	}
}


Direction::CardinalDirection AGhostBase::DetermineStartingDirection()
{
	const UMazeTile* currentTile = MazeManager->GetNearestTile(GetActorLocation());
	const UMazeTile* target = ProcessStatusTile();

	float closestDirDistToTarget = MAX_FLT;
	Direction::CardinalDirection closestDirToTarget = !CurrentDir;


	const UMazeTile* upTile		= MazeManager->GetNeighborTile(currentTile, Direction::Up);
	const UMazeTile* rightTile	= MazeManager->GetNeighborTile(currentTile, Direction::Right);
	const UMazeTile* downTile	= MazeManager->GetNeighborTile(currentTile, Direction::Down);
	const UMazeTile* leftTile	= MazeManager->GetNeighborTile(currentTile, Direction::Left);


	const int tilesToAvoid = MazeNode::Wall | MazeNode::MagicWall |
		((CurrentState == GhostState::Exiting || CurrentState == GhostState::Eaten) ? 0 : MazeNode::SpawnerExit);
	
	
	if ( (upTile->Node().Interior & tilesToAvoid) == 0 )
	{
		closestDirDistToTarget = FVector::Dist(**upTile, **target);
		closestDirToTarget = Direction::Up;
	}

	if ( (rightTile->Node().Interior & tilesToAvoid) == 0 )
	{
		const float rightDist = FVector::Dist(**rightTile, **target);

		if (rightDist < closestDirDistToTarget)
		{
			closestDirDistToTarget = rightDist;
			closestDirToTarget = Direction::Right;	
		}
	}

	if ( (downTile->Node().Interior & tilesToAvoid) == 0 )
	{
		const float downDist = FVector::Dist(**rightTile, **target);

		if (downDist < closestDirDistToTarget)
		{
			closestDirDistToTarget = downDist;
			closestDirToTarget = Direction::Down;	
		}
	}

	if ( (leftTile->Node().Interior & tilesToAvoid) == 0 )
	{
		const float leftDist = FVector::Dist(**leftTile, **target);

		if (leftDist < closestDirDistToTarget)
		{
			closestDirDistToTarget = leftDist;
			closestDirToTarget = Direction::Left;	
		}
	}

	return closestDirToTarget;
}

Direction::CardinalDirection AGhostBase::DetermineNewDirection()
{
	const UMazeTile* target = ProcessStatusTile();

	if (!target)
	{
		WhiteFrighten();
		return RandomDirection();
	}
	
	float closestDirDistToTarget = MAX_FLT;
	Direction::CardinalDirection closestDirToTarget = !CurrentDir;


	const UMazeTile* frontTile	= MazeManager->GetNeighborTile(CurrentTile, CurrentDir);
	const UMazeTile* rightTile	= MazeManager->GetNeighborTile(CurrentTile, CurrentDir++);
	const UMazeTile* leftTile	= MazeManager->GetNeighborTile(CurrentTile, --CurrentDir);
	
	const int tilesToAvoid = MazeNode::Wall | MazeNode::MagicWall |
		((CurrentState == GhostState::Exiting || CurrentState == GhostState::Eaten) ? 0 : MazeNode::SpawnerExit);

	
	if ( (frontTile->Node().Interior & tilesToAvoid) == 0 )
	{
		closestDirDistToTarget = FVector::Dist(**frontTile, **target);
		closestDirToTarget = CurrentDir;
	}

	if ( (rightTile->Node().Interior & tilesToAvoid) == 0 )
	{
		const float rightDist = FVector::Dist(**rightTile, **target);

		if (rightDist < closestDirDistToTarget)
		{
			closestDirDistToTarget = rightDist;
			closestDirToTarget = CurrentDir++;
		}
	}

	if ( (leftTile->Node().Interior & tilesToAvoid) == 0 )
	{
		const float leftDist = FVector::Dist(**leftTile, **target);

		if (leftDist < closestDirDistToTarget)
		{
			closestDirDistToTarget = leftDist;
			closestDirToTarget = --CurrentDir;	
		}
	}

	return closestDirToTarget;
}

Direction::CardinalDirection AGhostBase::RandomDirection()
{
	const UMazeTile* frontTile	= MazeManager->GetNeighborTile(CurrentTile, CurrentDir);
	const UMazeTile* rightTile	= MazeManager->GetNeighborTile(CurrentTile, CurrentDir++);
	const UMazeTile* leftTile	= MazeManager->GetNeighborTile(CurrentTile, --CurrentDir);

	TArray<Direction::CardinalDirection> validEscapes;
	
	constexpr int tilesToAvoid = MazeNode::Wall | MazeNode::MagicWall | MazeNode::SpawnerExit; 

	
	if ( (frontTile->Node().Interior & tilesToAvoid) == 0 )
		validEscapes.Add(CurrentDir);

	if ( (rightTile->Node().Interior & tilesToAvoid) == 0 )
		validEscapes.Add(CurrentDir++);

	if ( (leftTile->Node().Interior & tilesToAvoid) == 0 )
		validEscapes.Add(--CurrentDir);


	// TL;DR : If 1, return the first index, if more, return a random, if 0 go back.
	return
	validEscapes.Num() > 0 ?
		(validEscapes.Num() > 1 ? validEscapes[FMath::RandRange(0, validEscapes.Num() -1)] : validEscapes[0])
		: !CurrentDir;
}


// Called when the game starts or when spawned
void AGhostBase::BeginPlay()
{
	Super::BeginPlay();

	SetGhostProperties(true);
}

// Called every frame
void AGhostBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
	switch (CurrentDir)
	{
	case Direction::Up:		AddActorWorldOffset({-Speed * 100.f * DeltaTime, 0, 0}); break;
	case Direction::Right:	AddActorWorldOffset({0, -Speed * 100.f * DeltaTime, 0}); break;
	case Direction::Down:	AddActorWorldOffset({ Speed * 100.f * DeltaTime, 0, 0}); break;
	case Direction::Left:	AddActorWorldOffset({0,  Speed * 100.f * DeltaTime, 0}); break;
		
	case Direction::None:	if (CountDownSpawnTimer(DeltaTime)) { CurrentDir = DetermineStartingDirection(); break; } else { return; }
	}

	
	if (CurrentDir != Direction::None)
	{
		TileProgress += Speed * DeltaTime;

		if (TileProgress >= 1.f)
		{
			SnapToClosestTile();
			CurrentDir = CurrentState != GhostState::Frightened ? DetermineNewDirection() : RandomDirection();
		}
	}

	if (CurrentState == GhostState::Frightened &&
		FVector::Dist(GetActorLocation(), PacMan->GetActorLocation()) < APacMan::EatingDistance)
	{ Eat(); }
}

void AGhostBase::Frighten(const float duration)
{
	if ( (CurrentState == GhostState::Chase || CurrentState == GhostState::Scatter) && (CurrentTile->Node().Interior != MazeNode::SpawnerExit) )
	{
		CurrentState = GhostState::Frightened;

		CurrentDir = !CurrentDir;
		TileProgress = 1 - TileProgress;
		Speed /= 2;

		BlueFrighten();
		
		Rotator->RotationRate = { 0, -30.f, 0 };
		
		GetWorldTimerManager().SetTimer(FrightenedHandle, this, &AGhostBase::EndFrighten, duration, false);
		GetWorldTimerManager().SetTimer(ColorBlinkHandle, this, &AGhostBase::WhiteFrighten, duration / 2.f, false);
	}	
}

void AGhostBase::BlueFrighten()
{
	GhostMaterial->SetVectorParameterValue("FloorColor", FLinearColor::Blue);
	GhostMaterial->SetVectorParameterValue("GrooveColor", FLinearColor::Blue);
	GhostMaterial->SetVectorParameterValue("TrimColor", FLinearColor::Blue);
	
	GhostMaterial->SetVectorParameterValue("GlowColor", FLinearColor::Blue * 3000);

	GetWorldTimerManager().SetTimer(ColorBlinkHandle, this, &AGhostBase::WhiteFrighten, 1.f, false);
}

void AGhostBase::WhiteFrighten()
{
	GhostMaterial->SetVectorParameterValue("FloorColor", FLinearColor::White);
	GhostMaterial->SetVectorParameterValue("GrooveColor", FLinearColor::White);
	GhostMaterial->SetVectorParameterValue("TrimColor", FLinearColor::White);
	
	GhostMaterial->SetVectorParameterValue("GlowColor", FLinearColor::White * 3000);

	GetWorldTimerManager().SetTimer(ColorBlinkHandle, this, &AGhostBase::BlueFrighten, 1.f, false);
}

void AGhostBase::Eat()
{
	CurrentState = GhostState::Eaten;
	Speed *= 4;
	
	GhostMaterial = PyramidMesh->CreateDynamicMaterialInstance(0, FrightenedMaterial);

	GetWorldTimerManager().ClearTimer(FrightenedHandle);
	GetWorldTimerManager().ClearTimer(ColorBlinkHandle);
}

void AGhostBase::EndFrighten()
{
	CurrentState = GhostState::Chase;
	Speed *= 2;
	
	GetWorldTimerManager().ClearTimer(ColorBlinkHandle);
	SetGhostProperties(false);
}

