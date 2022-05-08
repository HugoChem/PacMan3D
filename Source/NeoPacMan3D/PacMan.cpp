// Fill out your copyright notice in the Description page of Project Settings.


#include "PacMan.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
APacMan::APacMan()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<USoundWave> munch1(TEXT("/Game/Sounds/munch_1.munch_1"));
	if (munch1.Succeeded()) Munch1 = munch1.Object;
	
	static ConstructorHelpers::FObjectFinder<USoundWave> munch2(TEXT("/Game/Sounds/munch_2.munch_2"));
	if (munch2.Succeeded()) Munch2 = munch2.Object;

}

// Called when the game starts or when spawned
void APacMan::BeginPlay()
{
	Super::BeginPlay();
}

const UMazeTile* APacMan::SnapAndMove()
{
	const UMazeTile* currentTile = MazeManager->GetNearestTile(GetActorLocation());
	SetActorLocation(**currentTile);

	CurrentTile = currentTile;
	TileProgress = 0;

	if (CurrentTile->Node().Interior & (MazeNode::PacDot | MazeNode::PowerPellet))
	{
		MazeManager->EmptyTile(CurrentTile);

		
		if (MunchFlip)
			UGameplayStatics::PlaySound2D(this, Munch2);
		else
			UGameplayStatics::PlaySound2D(this, Munch1);

		MunchFlip = !MunchFlip;
	}

	
	constexpr int impassableNodes = MazeNode::Wall | MazeNode::GhostWall | MazeNode::SpawnerExit;
	
	if		( (MazeManager->GetNeighborTile(currentTile, NextDir)->Node().Interior & impassableNodes ) == 0 )
		CurrentDir = NextDir;

	else if ( (MazeManager->GetNeighborTile(currentTile, CurrentDir)->Node().Interior & impassableNodes) != 0 )
		CurrentDir = Direction::None;

	return currentTile;
}

void APacMan::ApplyMoveCheck(const Direction::CardinalDirection& dir)
{
	NextDir = dir;

	if (CurrentDir == Direction::None)
	{
		const UMazeTile* currentTile = MazeManager->GetNearestTile(GetActorLocation());

		if ( (MazeManager->GetNeighborTile(currentTile, dir)->Node().Interior & (MazeNode::Wall | MazeNode::GhostWall | MazeNode::SpawnerExit)) == 0)
			CurrentDir = dir;
	}

	else if (CurrentDir == !dir)
	{
		CurrentDir = dir;

		TileProgress = 1 - TileProgress;
	}
}

void APacMan::MoveUp()
{
	ApplyMoveCheck(Direction::Up);
}

void APacMan::MoveRight()
{
	ApplyMoveCheck(Direction::Right);
}

void APacMan::MoveDown()
{
	ApplyMoveCheck(Direction::Down);
}

void APacMan::MoveLeft()
{
	ApplyMoveCheck(Direction::Left);
}

// Called every frame
void APacMan::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (CurrentDir)
	{
	case Direction::Up:		AddActorWorldOffset({-Speed * 100.f * DeltaTime, 0, 0}); break;
	case Direction::Right:	AddActorWorldOffset({0, -Speed * 100.f * DeltaTime, 0}); break;
	case Direction::Down:	AddActorWorldOffset({ Speed * 100.f * DeltaTime, 0, 0}); break;
	case Direction::Left:	AddActorWorldOffset({0,  Speed * 100.f * DeltaTime, 0}); break;
	case Direction::None:	break;
	}

	if (CurrentDir != Direction::None)
	{
		TileProgress += Speed * DeltaTime;

		if (TileProgress >= 1.f)
			SnapAndMove();
	}
}

// Called to bind functionality to input
void APacMan::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("UpDir",		EInputEvent::IE_Pressed, this, &APacMan::MoveUp);
	PlayerInputComponent->BindAction("RightDir",	EInputEvent::IE_Pressed, this, &APacMan::MoveRight);
	PlayerInputComponent->BindAction("DownDir",		EInputEvent::IE_Pressed, this, &APacMan::MoveDown);
	PlayerInputComponent->BindAction("LeftDir",		EInputEvent::IE_Pressed, this, &APacMan::MoveLeft);
}

