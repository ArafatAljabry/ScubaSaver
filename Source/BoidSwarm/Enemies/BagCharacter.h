// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TwinStickNPCDestruction.h"
#include "TwinStickPickup.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Perception/PawnSensingComponent.h"
#include "BagCharacter.generated.h"

UCLASS()
class BOIDSWARM_API ABagCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABagCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


protected:

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="variables")
	float speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "variables")
	float damage;

	UPROPERTY(EditAnywhere, Category = "Destruction")
	TSubclassOf<ATwinStickNPCDestruction> DestructionProxyClass;

	FTimerHandle DestructionTimer;

	UPROPERTY(EditAnywhere, Category = "Score", meta = (ClampMin = 0, ClampMax = 100))
	int32 Score = 1;

	UPROPERTY(EditAnywhere, Category = "Pickup")
	TSubclassOf<ATwinStickPickup> PickupClass;

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NPC")
	bool bHit = false;

	
	UFUNCTION()
	void Killed();

	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;


	void ProjectileImpact(const FVector& ForwardVector);
};
