// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "MyActorProjectile.generated.h"

UCLASS()
class BOIDSWARM_API AMyActorProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyActorProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Life = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "variables")
	USphereComponent* collider;

	UFUNCTION()
	void DestroySelf();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProjectileMovement")
	class UProjectileMovementComponent* ProjectileMovement;

	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;



};
