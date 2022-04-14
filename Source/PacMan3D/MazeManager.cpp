// Fill out your copyright notice in the Description page of Project Settings.


#include "MazeManager.h"

#include "Blinky.h"
#include "PacMan.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMazeManager::AMazeManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MazeManagerRoot = CreateDefaultSubobject<USceneComponent>("Maze manager root");
	SetRootComponent(MazeManagerRoot);
}

void AMazeManager::ConstructPacMap(TArray<UMazeTile*>& ghostSpawns, APacMan*& pacMan)
{
	TArray<FString> mazeStrings;

	FFileHelper::LoadFileToStringArray(mazeStrings, *(FPaths::ProjectConfigDir() + "PacMap.ini"));

	MazeRows		= mazeStrings.Num();
	MazeCollumns	= mazeStrings[0].Len();
	
	Tiles = MakeUnique<TUniquePtr<UMazeTile*[]>[]>(MazeRows);
	
	for (int i = 0; i < MazeRows; ++i)
	{
		Tiles[i] = MakeUnique<UMazeTile*[]>(MazeCollumns);
		
		TArray<TCHAR>& mazeLine = mazeStrings[i].GetCharArray();
		
		for (int j = 0; j < MazeCollumns; ++j)
		{
			switch (mazeLine[j])
			{
			case '@':
				Tiles[i][j] = NewObject<UMazeTile>()->Node(MazeNode(CreatePacWall(i, j), i, j, MazeNode::Wall)); break;

			case 'X':
				Tiles[i][j] = NewObject<UMazeTile>()->Node(MazeNode(i, j, MazeNode::Wall)); break;

			case '~':
				Tiles[i][j] = NewObject<UMazeTile>()->Node(MazeNode(i, j, MazeNode::GhostWall)); break;

			case '?':
				Tiles[i][j] = NewObject<UMazeTile>()->Node(MazeNode(CreateGhostWall(i, j), i, j, MazeNode::GhostWall)); break;

				
			case '.':
				Tiles[i][j] = NewObject<UMazeTile>()->Node(MazeNode(CreatePacDot (i, j), i, j, MazeNode::PacDot)); break;

				
			case 'P':
				Tiles[i][j] = NewObject<UMazeTile>()->Node(MazeNode(i, j, MazeNode::PlayerSpawn));
				pacMan = PlacePacMan(i, j);
				pacMan->PacTile = Tiles[i][j];
				break;

				
			case 'G':
				Tiles[i][j] = NewObject<UMazeTile>()->Node(MazeNode(CreateGhostHouse(i, j), i, j, MazeNode::GhostSpawn));
				ghostSpawns.Add(Tiles[i][j]); break;

			case 'g':
				Tiles[i][j] = NewObject<UMazeTile>()->Node(MazeNode(i, j, MazeNode::GhostSpawn));
				ghostSpawns.Add(Tiles[i][j]); break;

			case 'E': Tiles[i][j] = NewObject<UMazeTile>()->Node(MazeNode(i, j, MazeNode::SpawnerExit)); break;
				
				
			default: Tiles[i][j] =  NewObject<UMazeTile>()->Node(MazeNode(i, j, MazeNode::Empty)); break;
			}
		}
	}
}

void AMazeManager::PlaceGhosts(const TArray<UMazeTile*>& ghostSpawns, const APacMan* pacMan)
{
	const TArray<const UMazeTile*> corners
	{ Tiles[0][0], Tiles[0][MazeCollumns -1], Tiles[MazeRows -1][MazeCollumns -1], Tiles[MazeRows -1][0] };
	
	TArray<FString> ghosts;
	FFileHelper::LoadFileToStringArray(ghosts, *(FPaths::ProjectConfigDir() + "Ghosts.ini"));

	int scatterCorner = 0; // Max 3 for lower left.
	int nextGhostSpawn = 0; // Cycles through the ghostSpawns.

	AGhostBase* newGhost;
	

	for (auto it = ghosts.CreateConstIterator(); it; ++it)
	{
		const UMazeTile* ghostSpawn = ghostSpawns[nextGhostSpawn];

		
		if		(*it == "Blinky")	newGhost = PlaceAGhost(EGhostType::Blinky,	**ghostSpawn, pacMan);
		else if (*it == "Pinky")	newGhost = PlaceAGhost(EGhostType::Pinky,	**ghostSpawn, pacMan);
		//else if (*it == "Inky")		newGhost = PlaceAGhost(EGhostType::Inky,	**ghostSpawn, pacMan);
		else if (*it == "Clyde")	newGhost = PlaceAGhost(EGhostType::Clyde,	**ghostSpawn, pacMan);
		
		else continue;

		
		newGhost->SetGhostInfos(
			FCString::Atoi(**(++it)),

			ghostSpawns[nextGhostSpawn],
			GetNearestTileOfType(**ghostSpawn, MazeNode::SpawnerExit),
			corners[scatterCorner]
			);

		if (scatterCorner < 3)	scatterCorner++;
		else					scatterCorner = 0;

		if (nextGhostSpawn < ghostSpawns.Num() -1)	nextGhostSpawn++;
		else										nextGhostSpawn = 0;
	}
}

// Called when the game starts or when spawned
void AMazeManager::BeginPlay()
{
	Super::BeginPlay();

	TArray<UMazeTile*> ghostSpawns;
	APacMan* pacMan;

	ConstructPacMap(ghostSpawns, pacMan);

	PlaceGhosts(ghostSpawns, pacMan);
}

const UMazeTile* AMazeManager::GetNearestTile(const FVector& nearestTo) const
{
	const UMazeTile* nearestTile = nullptr;
	float shortestDist = MAX_FLT;
	
	for (int i = 0; i < MazeRows; ++i)
	{
		for (int j = 0; j < MazeCollumns; ++j)
		{
			const float distanceToTile = FVector::Distance(nearestTo, **Tiles[i][j]);
		
			if (distanceToTile <= shortestDist)
			{
				shortestDist = distanceToTile;

				nearestTile = Tiles[i][j];
			}
		}
	}

	check(nearestTile)
	return nearestTile;
}

const UMazeTile* AMazeManager::GetNearestTileOfType(const FVector& nearestTo, const MazeNode::MazeNodeComposition targetType) const
{
	const UMazeTile* nearestTile = nullptr;
	float shortestDist = MAX_FLT;
	
	for (int i = 0; i < MazeRows; ++i)
	{
		for (int j = 0; j < MazeCollumns; ++j)
		{
			if (Tiles[i][j]->Node().Interior != targetType) continue;

			const float distanceToTile = FVector::Distance(nearestTo, **Tiles[i][j]);
			
			if (distanceToTile <= shortestDist)
			{
				shortestDist = distanceToTile;

				nearestTile = Tiles[i][j];
			}
		}
	}

	check(nearestTile)
	return nearestTile;
}

const UMazeTile* AMazeManager::GetNeighborTile (const UMazeTile* origin, const Direction::CardinalDirection& direction, const int stepsInDir) const
{
	switch (direction)
	{
	case Direction::Up:
		if (origin->Node().RowIndex - stepsInDir < 0)
			return Tiles[0									 ][origin->Node().CollumnIndex];
		else
			return Tiles[origin->Node().RowIndex - stepsInDir][origin->Node().CollumnIndex];
	
	case Direction::Right:
		if (origin->Node().CollumnIndex + stepsInDir >= MazeCollumns)
			return Tiles[origin->Node().RowIndex][MazeCollumns -1						  ];
		else
			return Tiles[origin->Node().RowIndex][origin->Node().CollumnIndex + stepsInDir];
	
	case Direction::Down:
		if (origin->Node().RowIndex + stepsInDir >= MazeRows)
			return Tiles[MazeRows -1						  ][origin->Node().CollumnIndex];
		else
			return Tiles[origin->Node().RowIndex + stepsInDir][origin->Node().CollumnIndex];
	
	case Direction::Left:
		if (origin->Node().CollumnIndex - stepsInDir < 0)
			return Tiles[origin->Node().RowIndex][0										  ];
		else
			return Tiles[origin->Node().RowIndex][origin->Node().CollumnIndex - stepsInDir];

	default: return nullptr;
	}
}

