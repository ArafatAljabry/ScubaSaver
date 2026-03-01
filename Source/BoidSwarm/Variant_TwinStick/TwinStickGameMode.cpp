// Copyright Epic Games, Inc. All Rights Reserved.


#include "TwinStickGameMode.h"
#include "TwinStickUI.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

ATwinStickGameMode::ATwinStickGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATwinStickGameMode::BeginPlay()
{
	

	// create the UI widget and add it to the viewport
	UIWidget = CreateWidget<UPlayerWidget>(UGameplayStatics::GetPlayerController(GetWorld(), 0), UIWidgetClass);
	UIWidget->AddToViewport(0);
	Player =  Cast<ACharacter >(UGameplayStatics::GetPlayerCharacter(this, 0));



	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(this, ATwinStickSpawner::StaticClass(), actors);

	for (auto& actor : actors)
	{
		if (ATwinStickSpawner* Spawner = Cast<ATwinStickSpawner>(actor))
		{
			Spawners.Add(Spawner);
		}

	}

	UpdateFishNumber();

}

void ATwinStickGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//Player fishes =< 0 game over
	//if (expression)
	//{
	//	GameOver();
	//}


	FindClosestSpawners();
}

void ATwinStickGameMode::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	// clear the combo timer
	GetWorld()->GetTimerManager().ClearTimer(ComboTimer);
}

void ATwinStickGameMode::ItemUsed(int32 Value)
{ 
	// update the UI
	//UIWidget->UpdateItems(Value);
}

void ATwinStickGameMode::ScoreUpdate(int32 Value)
{
	//// multiply the base score by the combo multiplier and add it to the score
	//Score += Value * Combo;

	//// update the UI
	//UIWidget->UpdateScore(Score);

	//// update the combo multiplier
	//ComboUpdate();
}

void ATwinStickGameMode::ComboUpdate()
{
	//// return
	//if (Combo > ComboCap)
	//{
	//	return;
	//}

	//// update the combo increment
	//++ComboIncrement;

	//// is it time to increase the multiplier?
	//if (ComboIncrement > ComboIncrementMax)
	//{
	//	// reset the combo increment
	//	ComboIncrement = 0;

	//	// increase the combo multiplier
	//	++Combo;

	//	// update the UI
	//	UIWidget->UpdateCombo(Combo);

	//}

	//// reset the cooldown timer
	//ResetComboCooldown();
}

void ATwinStickGameMode::ResetComboCooldown()
{
	// reset the combo cooldown timer
	//GetWorld()->GetTimerManager().SetTimer(ComboTimer, this, &ATwinStickGameMode::ResetCombo, ComboCooldown, false);
}

void ATwinStickGameMode::ResetCombo()
{
	//// is the combo multiplier above min?
	//if (Combo > 1)
	//{
	//	// reset the combo increment
	//	ComboIncrement = 0;

	//	// tick down the multiplier
	//	--Combo;

	//	// update the UI
	//	UIWidget->UpdateCombo(Combo);

	//	// reset the cooldown timer
	//	ResetComboCooldown();
	//}
}

void ATwinStickGameMode::UpdateFishNumber()
{

	//UIWidget->UpdateFishAmount(/*Add here the fish number*/);
}

void ATwinStickGameMode::FindClosestSpawners()
{
	for (auto& spawner : Spawners)
	{
		if (Player)
		{
			float distance = FVector::Dist(Player->GetActorLocation(), spawner->GetActorLocation());
			if (distance > range)
		{
			spawner->bCanSpawn = false;
		}
		else
		{
			spawner->bCanSpawn = true;
		}
		}
	

	}
}

bool ATwinStickGameMode::CanSpawnNPCs()
{
	// is the NPC counter under the cap?
	return NPCCount < NPCCap;
}

void ATwinStickGameMode::IncreaseNPCs()
{
	// increase the NPC counter
	++NPCCount;
}

void ATwinStickGameMode::DecreaseNPCs()
{
	// decrease the NPC counter
	--NPCCount;
}

void ATwinStickGameMode::GotFish()
{
	UIWidget->GotNewFish();
	//Create the component
	SoundComponent = UGameplayStatics::SpawnSound2D(
		this,
		SoundBase,
		1.0f,      // Volume
		1.0f,      // Pitch
		0.0f,      // Start time
		nullptr,   // Concurrency
		true,      // Persist
		false      // Don't auto destroy
	);
	if (SoundComponent)
		SoundComponent->Play();

	//Player u can say here add 1 fish

	UpdateFishNumber();
}

void ATwinStickGameMode::GameOver()
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("GameOver"));
}
