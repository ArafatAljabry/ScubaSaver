// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/DistanceBTService.h"

#include "BehaviorTree/BlackboardComponent.h"

void UDistanceBTService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);


	AActor* SelfActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(SelfActorKey.SelectedKeyName));

	AActor* PlayerCharacter = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(PlayerKey.SelectedKeyName));

	if (SelfActor && PlayerCharacter)
	{
		float Distance = FVector::Dist(SelfActor->GetActorLocation(), PlayerCharacter->GetActorLocation());
		OwnerComp.GetBlackboardComponent()->SetValueAsFloat(DistanceToPlayer.SelectedKeyName, Distance);

	}
}
