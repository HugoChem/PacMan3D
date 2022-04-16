// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MazeManager.generated.h"


namespace Direction
{
	enum CardinalDirection		{ Up, Right, Down, Left, None };

	inline CardinalDirection operator! (const CardinalDirection& dir)
	{
		switch (dir)
		{
		case Up:	return Down;
		case Right: return Left;
		case Down:	return Up;
		case Left:	return Right;

		default:	return None;
		}
	}

	// Clockwise
	inline CardinalDirection operator++ (const CardinalDirection& dir, int)
	{
		switch (dir)
		{
		case Up:	return Right;
		case Right: return Down;
		case Down:	return Left;
		case Left:	return Up;

		default:	return None;
		}
	}

	// Counter clockwise
	inline CardinalDirection operator-- (const CardinalDirection& dir)
	{
		switch (dir)
		{
		case Up:	return Left;
		case Right: return Up;
		case Down:	return Right;
		case Left:	return Down;

		default:	return None;
		}
	}
}


UENUM()
enum class EGhostType : uint8 { Blinky, Pinky, Inky, Clyde };


struct NEOPACMAN3D_API MazeNode
{
	FVector Position;

	int RowIndex, CollumnIndex;


	enum MazeNodeComposition
	{
		Empty		= 1 << 0,

		Wall		= 1 << 1,
		GhostWall	= 1 << 2, // PacMan can't through these ones
		MagicWall	= 1 << 3, // Magic bloc against the ghosts
		
		PacDot		= 1 << 4,
		PowerPellet	= 1 << 5,

		PlayerSpawn	= 1 << 6,
		GhostSpawn	= 1 << 7,
		SpawnerExit = 1 << 8
	};

	MazeNodeComposition Interior;

	
	MazeNode()
		: Position		(0),
		  RowIndex		(0),
		  CollumnIndex	(0),
		  Interior		(MazeNodeComposition::Empty)
	{}
	
	MazeNode(const FVector& position, const int rowIndex, const int collumnIndex, const MazeNodeComposition& interior)
		: Position		(position.X, position.Y, 0),
		  RowIndex		(rowIndex),
		  CollumnIndex	(collumnIndex),
		  Interior		(interior)
	{}

	MazeNode(const int rowIndex, const int collumnIndex, const MazeNodeComposition& interior)
		: Position		(rowIndex * 100.f, collumnIndex * -100.f, 0),
		  RowIndex		(rowIndex),
		  CollumnIndex	(collumnIndex),
		  Interior		(interior)
	{}

	MazeNode(const MazeNode& nodeOrigin)
		: Position		(nodeOrigin.Position),
		  RowIndex		(nodeOrigin.RowIndex),
		  CollumnIndex	(nodeOrigin.CollumnIndex),
		  Interior		(nodeOrigin.Interior)
	{}

	MazeNode(MazeNode&& nodeOrigin) noexcept
		: Position		(nodeOrigin.Position),
		  RowIndex		(nodeOrigin.RowIndex),
		  CollumnIndex	(nodeOrigin.CollumnIndex),
		  Interior		(nodeOrigin.Interior)
	{}

	
	MazeNode& operator= (const MazeNode& nodeCopy)
	{
		Position		= nodeCopy.Position;
		RowIndex		= nodeCopy.RowIndex;
		CollumnIndex	= nodeCopy.CollumnIndex;
		Interior		= nodeCopy.Interior;

		return *this;
	}
};

UCLASS()
class NEOPACMAN3D_API UMazeTile : public UClass
{
	GENERATED_BODY()
	
	MazeNode Intern;

public:
	UMazeTile() {}
	
	// Sets the internal node. Returns the full Tile.
	UMazeTile* Node(const MazeNode& newIntern)
	{ Intern = newIntern; return this; }

	// Returns the internal node as a const.
	const MazeNode& Node() const { return Intern; }
	
	// Quick access to the position.
	const FVector& operator* () const { return Intern.Position; }
	
	const UMazeTile& operator= (MazeNode&& nodeAssign)
	{ Intern = nodeAssign; return *this; }

	
	bool operator== (const UMazeTile& other) const
	{
		return	Intern.RowIndex		== other.Intern.RowIndex
		&&		Intern.CollumnIndex == other.Intern.CollumnIndex;
	}
};


UCLASS()
class NEOPACMAN3D_API AMazeManager : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	USceneComponent* MazeManagerRoot;

	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	int MazeRows = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	int MazeCollumns = 0;

	
	TUniquePtr<TUniquePtr<UMazeTile*[]>[]> Tiles;

	TArray<class AGhostBase*> Ghosts;

public:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	TArray<UStaticMeshComponent*> Walls;
	
	// Sets default values for this actor's properties
	AMazeManager();

private:
	void ConstructPacMap(TArray<UMazeTile*>& ghostSpawns, class APacMan*& pacMan);

	void PlaceGhosts(const TArray<UMazeTile*>& ghostSpawns, const class APacMan* pacMan);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
	FVector CreatePacWall(int row, int collumn);

	UFUNCTION(BlueprintImplementableEvent)
	// To be called after the all the tiles were placed.
	void CreateMazeBackground();

	UFUNCTION(BlueprintImplementableEvent)
	FVector CreateGhostWall(int row, int collumn);


	UFUNCTION(BlueprintImplementableEvent)
	FVector CreateGhostHouse(int row, int collumn);

	
	UFUNCTION(BlueprintImplementableEvent)
	FVector CreatePacDot(int row, int collumn);

	UFUNCTION(BlueprintImplementableEvent)
	FVector CreatePowerPellet(int row, int collumn);
	

	UFUNCTION(BlueprintImplementableEvent)
	class APacMan* PlacePacMan(int row, int collumn);

	UFUNCTION(BlueprintImplementableEvent)
	class AGhostBase* PlaceAGhost(EGhostType newGhostClass, FVector spawnPlace, const class APacMan* pacMan);

public:
	const UMazeTile* GetNearestTile			(const FVector& nearestTo) const;

	const UMazeTile* GetNearestTileOfType	(const FVector& nearestTo, MazeNode::MazeNodeComposition targetType) const;

	const UMazeTile* GetNeighborTile		(const UMazeTile* origin, const Direction::CardinalDirection& direction, const int stepsInDir = 1) const;
};

