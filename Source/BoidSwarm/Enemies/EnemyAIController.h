// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/PawnSensingComponent.h"
#include "EnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class BOIDSWARM_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

	AEnemyAIController();

protected:
	virtual void BeginPlay() override;
public:

	virtual void OnPossess(APawn* InPawn) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior Tree")
	UBehaviorTree* BagBT;

	UPROPERTY()
	UBlackboardComponent* BagBbComp;

	



	
};
