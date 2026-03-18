// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/EnemyAIController.h"

#include "BagCharacter.h"
#include "Shootercharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

AEnemyAIController::AEnemyAIController()
{

	BagBbComp = nullptr;
	BagBT = nullptr;
	ShooterBT = nullptr;
	ShooterBbComp = nullptr;
	
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);


    GetWorldTimerManager().SetTimerForNextTick([this, InPawn]()
        {
            ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
            if (!PlayerCharacter) return;

            if (InPawn->IsA(ABagCharacter::StaticClass()) && BagBT)
            {
                if (UseBlackboard(BagBT->GetBlackboardAsset(), BagBbComp))
                {
                    GetBlackboardComponent()->SetValueAsObject("Player", PlayerCharacter);
                    RunBehaviorTree(BagBT);
                }
            }
            else if (InPawn->IsA(AShootercharacter::StaticClass()) && ShooterBT)
            {
                if (UseBlackboard(ShooterBT->GetBlackboardAsset(), BagBbComp))
                {
                    GetBlackboardComponent()->SetValueAsObject("Player", PlayerCharacter);
                    RunBehaviorTree(ShooterBT);
                }
            }
        });

	
	



}


