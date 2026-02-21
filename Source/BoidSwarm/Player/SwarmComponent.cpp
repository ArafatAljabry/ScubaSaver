// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/SwarmComponent.h"

// Sets default values for this component's properties
USwarmComponent::USwarmComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USwarmComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	Owner = GetOwner();
	for (int i = 0; i < MaxFish; ++i)
	{
		AddFish();
	}
	
}


// Called every frame
void USwarmComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

	Orbit(DeltaTime);
}

void USwarmComponent::AddFish()
{
	if (!FishClass)
	{
		return;
	}

	
	
	FActorSpawnParameters Params;
	AActor* newFish = GetWorld()->SpawnActor<AActor>(FishClass, Owner->GetActorLocation(), FRotator::ZeroRotator,Params);

	FishList.Add(newFish);

}

void USwarmComponent::KillFish()
{
	if (FishList.Num() == 0)
	{
		return;
	}

	AActor* Fish = FishList.Pop();
	Fish->Destroy();
}

void USwarmComponent::Orbit(float Deltatime)
{
	if (FishList.Num() == 0)
	{
		return;
	}

	FVector Center = Owner->GetActorLocation();

	yawOffset += swarmSpeed*Deltatime; 
	float angleStep = 2 * PI / FishList.Num();


	for (int i = 0; i < FishList.Num(); ++i)
	{
		AActor* fish = FishList[i];

		float yaw = i * angleStep  + yawOffset;
		float pitch = FMath::Sin(i * 2.3f + yawOffset * 0.7f) * (PI / 2.0f);

		FVector Offset = FVector(FMath::Cos(pitch) * FMath::Cos(yaw) * OrbitRadius,FMath::Cos(pitch) * FMath::Sin(yaw) * OrbitRadius, FMath::Sin(pitch) * OrbitRadius);

		
		FVector target = Center + Offset;

		fish->SetActorLocation(target);
		fish->SetActorRotation((Center - target).Rotation());
	}


}

