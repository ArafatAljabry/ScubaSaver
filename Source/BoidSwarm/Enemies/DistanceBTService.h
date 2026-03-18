// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "DistanceBTService.generated.h"

/**
 * 
 */
UCLASS()
class BOIDSWARM_API UDistanceBTService : public UBTService
{
	GENERATED_BODY()

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackBoard")
	FBlackboardKeySelector SelfActorKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackBoard")
	FBlackboardKeySelector PlayerKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackBoard")
	FBlackboardKeySelector DistanceToPlayer;

};
