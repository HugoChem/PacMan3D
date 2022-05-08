// Fill out your copyright notice in the Description page of Project Settings.


#include "MazeManager.h"

#include "Blinky.h"
#include "Pinky.h"
#include "Inky.h"
#include "Clyde.h"

#include "PacMan.h"

#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"


void UMazeTile::Empty()
{
	Intern.Interior = MazeNode::Empty;
	Intern.MeshActor->Destroy();
}

// Sets default values
AMazeManager::AMazeManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MazeManagerRoot = CreateDefaultSubobject<USceneComponent>("Maze manager root");
	SetRootComponent(MazeManagerRoot);

	RootComponent->SetMobility(EComponentMobility::Movable);
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
			const FVector mazePos = IndexToVector(i, j);
			
			switch (mazeLine[j])
			{
			case '@':
				Tiles[i][j] = NewObject<UMazeTile>()
				->Node(MazeNode(i, j, MazeNode::Wall, CreatePacWall(mazePos)));
				
				break;

			case 'X':
				Tiles[i][j] = NewObject<UMazeTile>()
				->Node(MazeNode(i, j, MazeNode::Wall));
				
				break;

			case '~':
				Tiles[i][j] = NewObject<UMazeTile>()
				->Node(MazeNode(i, j, MazeNode::GhostWall));

				break;

			case '?':
				Tiles[i][j] = NewObject<UMazeTile>()
				->Node(MazeNode(i, j, MazeNode::GhostWall, CreateGhostWall(mazePos)));

				break;

				
			case '.':
				Tiles[i][j] = NewObject<UMazeTile>()
				->Node(MazeNode(i, j, MazeNode::PacDot, CreatePacDot(mazePos)));

				break;

			case 'o':
				Tiles[i][j] = NewObject<UMazeTile>()
				->Node(MazeNode(i, j, MazeNode::PowerPellet, CreatePowerPellet(mazePos)));

				break;
			

			case 'p':
				Tiles[i][j] = NewObject<UMazeTile>()
				->Node(MazeNode(i, j, MazeNode::PlayerSpawn));

				pacMan = PlacePacMan(mazePos, false);

				break;

				
			case 'P':
				Tiles[i][j] = NewObject<UMazeTile>()
				->Node(MazeNode(i, j, MazeNode::PlayerSpawn));

				pacMan = PlacePacMan(mazePos, true);

				break;

				
			case 'G':
				Tiles[i][j] = NewObject<UMazeTile>()
				->Node(MazeNode(i, j, MazeNode::GhostSpawn));

				ghostSpawns.Add(Tiles[i][j]);

				break;

				
			case 'h':
				Tiles[i][j] = NewObject<UMazeTile>()
				->Node(MazeNode(i, j, MazeNode::Empty));

				CreateLittleGhostHouse(mazePos);
				ghostSpawns.Add(Tiles[i][j]);

				break;

			case 'H':
				Tiles[i][j] = NewObject<UMazeTile>()
				->Node(MazeNode(i, j, MazeNode::Empty));

				CreateBigGhostHouse(mazePos);

				break;

				
			case 'E':
				Tiles[i][j] = NewObject<UMazeTile>()
				->Node(MazeNode(i, j, MazeNode::SpawnerExit));

				break;
				
				
			default: Tiles[i][j] =  NewObject<UMazeTile>()->Node(MazeNode(i, j, MazeNode::Empty)); break;
			}
		}
	}

	CreateMazeBackground();
}

void AMazeManager::PlaceGhosts(const TArray<UMazeTile*>& ghostSpawns, const APacMan* pacMan)
{
	const TArray<const UMazeTile*> corners
	{ Tiles[0][0], Tiles[0][MazeCollumns -1], Tiles[MazeRows -1][MazeCollumns -1], Tiles[MazeRows -1][0] };
	
	TArray<FString> ghostStrings;
	FFileHelper::LoadFileToStringArray(ghostStrings, *(FPaths::ProjectConfigDir() + "Ghosts.ini"));

	int scatterCorner = 0; // Max 3 for lower left.
	int nextGhostSpawn = 0; // Cycles through the ghostSpawns.

	AGhostBase* newGhost = nullptr;
	

	for (auto it = ghostStrings.CreateConstIterator(); it; ++it)
	{
		const UMazeTile* ghostSpawn = ghostSpawns[nextGhostSpawn];

		if		(*it == "Blinky")
			newGhost = PlaceAGhost(EGhostType::Blinky,	**ghostSpawn, pacMan);

		else if (*it == "Pinky")
			newGhost = PlaceAGhost(EGhostType::Pinky,	**ghostSpawn, pacMan);

		else if (*it == "Inky")
		{
			newGhost = PlaceAGhost(EGhostType::Inky,	**ghostSpawn, pacMan);
			Cast<AInky>(newGhost)->FollowGhost(Ghosts.Last());			
		}

		else if (*it ==  "Clyde")
			newGhost = PlaceAGhost(EGhostType::Clyde,	**ghostSpawn, pacMan);

		
		newGhost->SetGhostInfos(
			FCString::Atoi(**(++it)),

			ghostSpawns[nextGhostSpawn],
			GetNearestTileOfType(**ghostSpawn, MazeNode::SpawnerExit),
			corners[scatterCorner]
			);

		
		Ghosts.Add(newGhost);

		
		if (scatterCorner < 3)	scatterCorner++;
		else					scatterCorner = 0;

		if (nextGhostSpawn < ghostSpawns.Num() -1)	nextGhostSpawn++;
		else										nextGhostSpawn = 0;
	}
}

void AMazeManager::RefineWalls()
{
	for (int i = 0; i < MazeRows; ++i)
	{
		for (int j = 0; j < MazeCollumns; ++j)
		{
			if (Tiles[i][j]->Node().Interior == MazeNode::Wall)
			{
				FVector displacement = {0, 0, 0};

				if (GetNeighborTile(Tiles[i][j], Direction::Up)		->Node().Interior == MazeNode::Wall)
					displacement += FVector(-50.f, 0, 0);

				if (GetNeighborTile(Tiles[i][j], Direction::Right)	->Node().Interior == MazeNode::Wall)
					displacement += FVector(0, -50.f, 0);

				if (GetNeighborTile(Tiles[i][j], Direction::Down)	->Node().Interior == MazeNode::Wall)
					displacement += FVector(50.f, 0, 0);

				if (GetNeighborTile(Tiles[i][j], Direction::Left)	->Node().Interior == MazeNode::Wall)
					displacement += FVector(0, 50.f, 0);


				if (AStaticMeshActor* wallMesh = Tiles[i][j]->Node().MeshActor)
					wallMesh->AddActorWorldOffset(displacement);
			}
		}
	}
}

// Called when the game starts or when spawned
void AMazeManager::BeginPlay()
{
	Super::BeginPlay();

	TArray<UMazeTile*> ghostSpawns;
	APacMan* pacMan;

	ConstructPacMap(ghostSpawns, pacMan);

	pacMan->UpdateCurrentTile();
	
	PlaceGhosts(ghostSpawns, pacMan);

	//RefineWalls();
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

	default: return origin;
	}
}

void AMazeManager::EmptyTile(const UMazeTile* targetTile) const
{
	if (targetTile->Node().Interior == MazeNode::PowerPellet)
	{
		for (AGhostBase* ghost : Ghosts)
		{
			ghost->Frighten();
		}
	}
	
	Tiles[targetTile->Node().RowIndex][targetTile->Node().CollumnIndex]->Empty();
}

