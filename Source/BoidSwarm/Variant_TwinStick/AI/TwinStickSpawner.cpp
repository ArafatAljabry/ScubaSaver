// Copyright Epic Games, Inc. All Rights Reserved.


#include "TwinStickSpawner.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "NavigationSystem.h"
#include "NavMesh/RecastNavMesh.h"
#include "Kismet/GameplayStatics.h"
#include "TwinStickNPC.h"
#include "TwinStickGameMode.h"

ATwinStickSpawner::ATwinStickSpawner()
{
 	PrimaryActorTick.bCanEverTick = true;

}

void ATwinStickSpawner::BeginPlay()
{
	Super::BeginPlay();
	
	// find the recast navmesh actor on the level
	TArray<AActor*> ActorList;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARecastNavMesh::StaticClass(), ActorList);

	if (ActorList.Num() > 0)
	{
		NavData = Cast<ARecastNavMesh>(ActorList[0]);
	} else {

		UE_LOG(LogTemp, Log, TEXT("Could not find recast navmesh"));

	}

	// set up the spawn timer
	GetWorld()->GetTimerManager().SetTimer(SpawnGroupTimer, this, &ATwinStickSpawner::SpawnNPCGroup, SpawnGroupDelay, true);

	GetWorld()->GetTimerManager().SetTimer(ShooterStarts, this, &ATwinStickSpawner::EnableShooter, 10, false);

	

	// spawn the first group of NPCs
	SpawnNPCGroup();
}

void ATwinStickSpawner::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// clear the spawn timers
	GetWorld()->GetTimerManager().ClearTimer(SpawnGroupTimer);
	GetWorld()->GetTimerManager().ClearTimer(SpawnNPCTimer);
	GetWorld()->GetTimerManager().ClearTimer(ShooterStarts);
}

void ATwinStickSpawner::SpawnNPCGroup()
{
	// check if we're still under the max NPC cap

	SpawnCount = 0;
	GM = Cast<ATwinStickGameMode>(GetWorld()->GetAuthGameMode());
	
		if (GM->CanSpawnNPCs())
		{
			SpawnNPC();
		
		}
	
}

void ATwinStickSpawner::SpawnNPC()
{
	TSubclassOf<ACharacter> ClassToSpawn = nullptr;

	if (bCanSpawn)
	{
		FTransform SpawnTransform;

		// find a random point around the spawner
		FVector SpawnLoc;
		if (UNavigationSystemV1::K2_GetRandomReachablePointInRadius(GetWorld(), GetActorLocation(), SpawnLoc, SpawnRadius, NavData))
		{
			SpawnTransform.SetLocation(SpawnLoc);

			// spawn the NPC
			//ATwinStickNPC* NPC = GetWorld()->SpawnActor<ATwinStickNPC>(NPCClass, SpawnTransform);
			if (isBag)
			{
				ClassToSpawn = BagNPCClass;
			}
			if (BCanSpawnShooter && bShooter)
			{
				ClassToSpawn = ShooterClass;
			}
		
			
			ACharacter* NewCharacter = GetWorld()->SpawnActor<ACharacter>(ClassToSpawn, SpawnTransform);
			if (NewCharacter)
			{
				GM->IncreaseNPCs();
			}

			if(isTrawler) { ATrolleyNetActor* Trawler = GetWorld()->SpawnActor<ATrolleyNetActor>(TrawlerNPCClass, SpawnTransform); }
		}
		
		// increase the spawn counter
		SpawnCount++;

		
	}
	
	if (SpawnCount < SpawnGroupSize)
	{
		GetWorld()->GetTimerManager().SetTimer(
			SpawnNPCTimer,
			this,
			&ATwinStickSpawner::SpawnNPC,
			FMath::RandRange(MinSpawnDelay, MaxSpawnDelay),
			false
		);
	}
}

void ATwinStickSpawner::EnableShooter()
{
	UE_LOG(LogTemp, Warning, TEXT("Shooters"));
	BCanSpawnShooter = true;
}
