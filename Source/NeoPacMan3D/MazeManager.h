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

	
	class AStaticMeshActor* MeshActor;

	
	MazeNode()
		: Position		(FVector::ZeroVector),
		  RowIndex		(0),
		  CollumnIndex	(0),
		  Interior		(Empty),
		  MeshActor		(nullptr)
	{}

	MazeNode(const int rI, const int cI, const MazeNodeComposition& interior)
		: Position		(rI * 100.f, cI * -100.f, 0),
		  RowIndex		(rI),
		  CollumnIndex	(cI),
		  Interior		(interior),
		  MeshActor		(nullptr)
	{}

	MazeNode(const int rI, const int cI, const MazeNodeComposition& interior, class AStaticMeshActor* meshActor)
		: Position		(rI * 100.f, cI * -100.f, 0),
		  RowIndex		(rI),
		  CollumnIndex	(cI),
		  Interior		(interior),
		  MeshActor		(meshActor)
	{}

	MazeNode(const MazeNode& nodeOrigin)
		: Position		(nodeOrigin.Position),
		  RowIndex		(nodeOrigin.RowIndex),
		  CollumnIndex	(nodeOrigin.CollumnIndex),
		  Interior		(nodeOrigin.Interior),
		  MeshActor		(nodeOrigin.MeshActor)
	{}

	MazeNode(MazeNode&& nodeOrigin) noexcept
		: Position		(nodeOrigin.Position),
		  RowIndex		(nodeOrigin.RowIndex),
		  CollumnIndex	(nodeOrigin.CollumnIndex),
		  Interior		(nodeOrigin.Interior),
		  MeshActor		(nodeOrigin.MeshActor)
	{}

	
	MazeNode& operator= (const MazeNode& nodeCopy)
	{
		Position		= nodeCopy.Position;
		RowIndex		= nodeCopy.RowIndex;
		CollumnIndex	= nodeCopy.CollumnIndex;
		Interior		= nodeCopy.Interior;
		MeshActor		= nodeCopy.MeshActor;

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


	// Clears any static mesh associated and sets the interior to nothing.
	void Empty();
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

	void RefineWalls();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Utilitary")
	static FVector IndexToVector(int row, int collumn)
	{ return {row * 100.f, collumn * -100.f, 0.f}; }

	UFUNCTION(BlueprintImplementableEvent, Category = "Walls")
	class AStaticMeshActor* CreatePacWall(const FVector& pos);

	UFUNCTION(BlueprintImplementableEvent, Category = "Walls")
	class AStaticMeshActor* CreateGhostWall(const FVector& pos);

	
	UFUNCTION(BlueprintImplementableEvent, Category = "Utilitary")
	// To be called after the all the tiles were placed.
	void CreateMazeBackground();
	

	UFUNCTION(BlueprintImplementableEvent, Category = "Ghosts")
	void CreateLittleGhostHouse(const FVector& pos);

	UFUNCTION(BlueprintImplementableEvent, Category = "Ghosts")
	void CreateBigGhostHouse(const FVector& pos);

	
	UFUNCTION(BlueprintImplementableEvent, Category = "Dots")
	class AStaticMeshActor* CreatePacDot(const FVector& pos);

	UFUNCTION(BlueprintImplementableEvent, Category = "Dots")
	class AStaticMeshActor* CreatePowerPellet(const FVector& pos);
	

	UFUNCTION(BlueprintImplementableEvent, Category = "PacMan")
	class APacMan* PlacePacMan(const FVector& pos, bool offset);

	UFUNCTION(BlueprintImplementableEvent, Category = "Ghosts")
	class AGhostBase* PlaceAGhost(EGhostType newGhostClass, const FVector& pos, const class APacMan* pacMan);

public:
	const UMazeTile* GetNearestTile			(const FVector& nearestTo) const;

	const UMazeTile* GetNearestTileOfType	(const FVector& nearestTo, MazeNode::MazeNodeComposition targetType) const;

	const UMazeTile* GetNeighborTile		(const UMazeTile* origin, const Direction::CardinalDirection& direction, const int stepsInDir = 1) const;


	void EmptyTile(const UMazeTile* targetTile) const;
};

