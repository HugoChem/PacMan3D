// Fill out your copyright notice in the Description page of Project Settings.


#include "GhostBase.h"
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

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> material(TEXT("/Game/Ghosts/GhostMaterial.GhostMaterial"));
	if (material.Succeeded())
		PyramidMesh->SetMaterial(0, material.Object);

	
	Rotator = CreateDefaultSubobject<URotatingMovementComponent>("Rotator");
	Rotator->RotationRate = { 0, 60, 0 };
	Rotator->bRotationInLocalSpace = false;

}

const UMazeTile* AGhostBase::SnapToClosestTile()
{
	const UMazeTile* currentTile = MazeManager->GetNearestTile(GetActorLocation());
	SetActorLocation(**currentTile);

	TileProgress = 0;

	check(currentTile)
	GhostTile = currentTile;
	return currentTile;
}

const UMazeTile* AGhostBase::ProcessStatusTile(const UMazeTile* currentTile)
{
	check(currentTile != nullptr)
	
	switch (CurrentState)
	{
	case GhostState::Waiting: return SpawnTile;

	case GhostState::Exiting:
		if (currentTile == ExitTile)
		{
			CurrentState = GhostState::Chase;
			return GetChaseTile(currentTile);
		}
		else return ExitTile;

		
	case GhostState::Chase:			return GetChaseTile(currentTile);
	case GhostState::Scatter:		return ScatterTile;

	case GhostState::Frightened:	return nullptr;


	case GhostState::Eaten:
		if (currentTile == SpawnTile)
		{
			CurrentState = GhostState::Exiting;
			CurrentDir = Direction::None;
			return ExitTile;
		}
		else return SpawnTile;

		
	default: return nullptr;
	}
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


Direction::CardinalDirection AGhostBase::DetermineNewDirection()
{
	const UMazeTile* target = ProcessStatusTile(GhostTile);
	

	float closestDirDistToTarget = MAX_FLT;
	Direction::CardinalDirection closestDirToTarget = !CurrentDir;


	const UMazeTile* frontTile	= MazeManager->GetNeighborTile(GhostTile, CurrentDir);
	const UMazeTile* rightTile	= MazeManager->GetNeighborTile(GhostTile, CurrentDir++);
	const UMazeTile* leftTile	= MazeManager->GetNeighborTile(GhostTile, --CurrentDir);
	
	const int tilesToAvoid = MazeNode::Wall | MazeNode::MagicWall | (CurrentState == GhostState::Exiting ? 0 : MazeNode::SpawnerExit); 

	
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

Direction::CardinalDirection AGhostBase::DetermineStartingDirection()
{
	const UMazeTile* currentTile = MazeManager->GetNearestTile(GetActorLocation());
	const UMazeTile* target = ProcessStatusTile(currentTile);

	float closestDirDistToTarget = MAX_FLT;
	Direction::CardinalDirection closestDirToTarget = !CurrentDir;


	const UMazeTile* upTile		= MazeManager->GetNeighborTile(currentTile, Direction::Up);
	const UMazeTile* rightTile	= MazeManager->GetNeighborTile(currentTile, Direction::Right);
	const UMazeTile* downTile	= MazeManager->GetNeighborTile(currentTile, Direction::Down);
	const UMazeTile* leftTile	= MazeManager->GetNeighborTile(currentTile, Direction::Left);

	
	if ( (upTile->Node().Interior & (MazeNode::Wall | MazeNode::MagicWall)) == 0 )
	{
		closestDirDistToTarget = FVector::Dist(**upTile, **target);
		closestDirToTarget = Direction::Up;
	}

	if ( (rightTile->Node().Interior & (MazeNode::Wall | MazeNode::MagicWall)) == 0 )
	{
		const float rightDist = FVector::Dist(**rightTile, **target);

		if (rightDist < closestDirDistToTarget)
		{
			closestDirDistToTarget = rightDist;
			closestDirToTarget = Direction::Right;	
		}
	}

	if ( (downTile->Node().Interior & (MazeNode::Wall | MazeNode::MagicWall)) == 0 )
	{
		const float downDist = FVector::Dist(**rightTile, **target);

		if (downDist < closestDirDistToTarget)
		{
			closestDirDistToTarget = downDist;
			closestDirToTarget = Direction::Down;	
		}
	}

	if ( (leftTile->Node().Interior & (MazeNode::Wall | MazeNode::MagicWall)) == 0 )
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


// Called when the game starts or when spawned
void AGhostBase::BeginPlay()
{
	Super::BeginPlay();

	GhostMaterial = PyramidMesh->CreateDynamicMaterialInstance(0);
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
			CurrentDir = DetermineNewDirection();

		}
	}
}

