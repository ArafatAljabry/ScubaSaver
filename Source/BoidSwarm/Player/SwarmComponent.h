// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SwarmComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BOIDSWARM_API USwarmComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USwarmComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:

	AActor* Owner;

	TArray<float>Phase;
	TArray<float>PitchOffset;
	

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Swarm")
	int MaxFish;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swarm")
	float OrbitRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swarm")
	float swarmSpeed;

	UPROPERTY()
	TArray<AActor*>FishList;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Swarm")
	TSubclassOf<AActor> FishClass;

	float yawOffset = 0.0;

	//Health Functions
	UFUNCTION()
	void AddFish();

	UFUNCTION()
	void KillFish();

	//Behavior

	UFUNCTION()
	void Orbit(float Deltatime);

	UFUNCTION()
	FQuat LookAt(const FVector& lookAt, const FVector& upDirection);


};
