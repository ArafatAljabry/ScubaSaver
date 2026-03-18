// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/FIndRetreatLocBTService.h"

#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

void UFIndRetreatLocBTService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AIController = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());

	UNavigationSystemV1* Nav = UNavigationSystemV1::GetCurrent(OwnerComp.GetWorld());

	FNavLocation RandomLoc;
	if (!Nav->GetRandomReachablePointInRadius(AIController->GetPawn()->GetActorLocation(), 400.0f, RandomLoc))
	{
		return;
	}

	OwnerComp.GetBlackboardComponent()->SetValueAsVector(RetreatLocKey.SelectedKeyName, RandomLoc.Location);
}
