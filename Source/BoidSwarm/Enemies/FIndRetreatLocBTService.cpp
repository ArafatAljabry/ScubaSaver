// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/FIndRetreatLocBTService.h"

#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

void UFIndRetreatLocBTService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AIController = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());

	UNavigationSystemV1* Nav = UNavigationSystemV1::GetCurrent(OwnerComp.GetWorld());
	AActor* Player = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(PlayerKey.SelectedKeyName));

	

	FNavLocation RandomLoc;
	if (!Nav->GetRandomReachablePointInRadius(AIController->GetPawn()->GetActorLocation(), 400.0f, RandomLoc))
	{
		return;
	}

if (Player)
	{
		FVector Dir = (RandomLoc.Location - Player->GetActorLocation()).GetSafeNormal();

	FVector Location = RandomLoc.Location + Dir * 500.0f;

	OwnerComp.GetBlackboardComponent()->SetValueAsVector(RetreatLocKey.SelectedKeyName, Location);
	}
	
}
