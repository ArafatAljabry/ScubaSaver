// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyActorProjectile.h"
#include "GameFramework/Character.h"
#include "Shootercharacter.generated.h"

UCLASS()
class BOIDSWARM_API AShootercharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShootercharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "variables")
	float speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "variables")
	float FireRate = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* ShootLoc;
	UPROPERTY(EditAnywhere, Category = "Pickup")
	float FishSpawnPercentage = 0;
	ACharacter* Player = nullptr;

	FTimerHandle timerShoot;
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AMyActorProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* SoundBase;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAudioComponent* SoundComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NPC")
	bool bHit = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NPC")
	bool bCanShoot = true;


	UFUNCTION()
	void Killed();

	UFUNCTION()
	void ProjectileImpact(const FVector& ForwardVector);

	UFUNCTION()
	void Shoot();

	UFUNCTION()
	void ResetFire();

	UFUNCTION()
	void FireProjectile();

	void FacePlayer();

};
