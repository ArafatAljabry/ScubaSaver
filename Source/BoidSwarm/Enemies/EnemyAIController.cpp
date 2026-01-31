// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/EnemyAIController.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

AEnemyAIController::AEnemyAIController()
{

	BagBbComp = nullptr;
	BagBT = nullptr;
	
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	//BagBbComp->SetValueAsObject("Player", UGameplayStatics::GetPlayerCharacter(this, 0));
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);


	if (!BagBT)
	{return;}
	UseBlackboard(BagBT->GetBlackboardAsset(), BagBbComp);
	BagBbComp->SetValueAsObject("Player", UGameplayStatics::GetPlayerCharacter(this, 0));
	RunBehaviorTree(BagBT);
	
	



}


