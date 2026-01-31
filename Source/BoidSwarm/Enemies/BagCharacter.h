// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TwinStickNPCDestruction.h"
#include "TwinStickPickup.h"
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

	UPROPERTY(EditAnywhere, Category = "Score", meta = (ClampMin = 0, ClampMax = 100))
	int32 Score = 1;

	/** Type of pickup to spawn on death */
	UPROPERTY(EditAnywhere, Category = "Pickup")
	TSubclassOf<ATwinStickPickup> PickupClass;

	/** Type of destruction proxy to spawn on death */
	UPROPERTY(EditAnywhere, Category = "Destruction")
	TSubclassOf<ATwinStickNPCDestruction> DestructionProxyClass;

	/** Deferred destruction timer */
	FTimerHandle DestructionTimer;

public:

	/** If true, this NPC has already been hit by a projectile and is being destroyed. Exposed to BP so it can be read by StateTree */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NPC")
	bool bHit = false;


	/** Tells the NPC to process a projectile impact */
	void ProjectileImpact(const FVector& ForwardVector);

	void Killed();

	/** Collision handling */
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;


};
