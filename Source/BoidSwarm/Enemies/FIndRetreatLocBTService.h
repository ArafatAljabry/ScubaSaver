// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyAIController.h"
#include "BehaviorTree/BTService.h"
#include "FIndRetreatLocBTService.generated.h"

/**
 * 
 */
UCLASS()
class BOIDSWARM_API UFIndRetreatLocBTService : public UBTService
{
	GENERATED_BODY()


	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	
protected:

	UPROPERTY()
	AEnemyAIController* AIController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key")
	FBlackboardKeySelector RetreatLocKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlackBoard")
	FBlackboardKeySelector PlayerKey;



};
