// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/MoveBTTaskNode.h"


#include "NavigationSystem.h"
#include "Shootercharacter.h"
#include "BehaviorTree/BlackboardComponent.h"


UMoveBTTaskNode::UMoveBTTaskNode()
{
	NodeName = "SHoot";
}

EBTNodeResult::Type UMoveBTTaskNode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AIController = Cast<AEnemyAIController>(OwnerComp.GetAIOwner());

	AShootercharacter* shooter = Cast<AShootercharacter>(AIController->GetPawn());

	if (!shooter)
	{
		return EBTNodeResult::Failed;
	}

	shooter->Shoot();

	return  EBTNodeResult::Succeeded;
}
