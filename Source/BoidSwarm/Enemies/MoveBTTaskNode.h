// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "EnemyAIController.h"
#include "MoveBTTaskNode.generated.h"

/**
 * 
 */
UCLASS()
class BOIDSWARM_API UMoveBTTaskNode : public UBTTaskNode
{
	GENERATED_BODY()

	UMoveBTTaskNode();

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:

	UPROPERTY()
	AEnemyAIController* AIController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Key")
	FBlackboardKeySelector RetreatLocKey;

	
};
