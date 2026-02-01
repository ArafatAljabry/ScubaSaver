// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "TrolleyNetActor.generated.h"

UCLASS()
class BOIDSWARM_API ATrolleyNetActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATrolleyNetActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Enemy")
	float speed = 20.0f;
	UPROPERTY(EditAnywhere, Category = "Enemy")
	float damage = -1000.0f;


protected:
	UPROPERTY(VisibleAnywhere, Category = "Collision")
	UBoxComponent* CollisionComponent;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* trolleySound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	UAudioComponent* trolleySoundComponent;

};
