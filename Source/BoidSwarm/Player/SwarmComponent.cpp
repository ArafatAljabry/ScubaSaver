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

	Phase.Add(FMath::RandRange(0.0f, 2 * PI));
	PitchOffset.Add(FMath::RandRange(-1.5, 1.5));

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
	const int32 Fishlist = FishList.Num();
	if (Fishlist == 0)
	{
		return;
	}

	yawOffset += swarmSpeed*Deltatime; 

	const FTransform OwnerTransform = Owner->GetActorTransform();
	const FVector Center = OwnerTransform.GetLocation();
	const FVector Foward = OwnerTransform.GetUnitAxis(EAxis::X);
	const FVector Up = OwnerTransform.GetUnitAxis(EAxis::Z);
	const FVector Right = OwnerTransform.GetUnitAxis(EAxis::Y);

	
	for (int32 i = 0; i < Fishlist; ++i)
	{

		AActor* fish = FishList[i];
		
		const float Yaw = Phase[i] + yawOffset ;
		const float Pitch = FMath::Sin(i*2.3f + yawOffset * 0.7) * (PI / 6.0);

		
		float YawCos, YawSin;
		FMath::SinCos(&YawSin,&YawCos,Yaw);
		float PitchSin, PitchCos;
		FMath::SinCos(&PitchSin,&PitchCos,Pitch);

		
		FVector direction = Right * (YawCos * PitchCos) * OrbitRadius + Foward * (YawSin * PitchCos) * OrbitRadius + Up * PitchSin * OrbitRadius;
		const FVector Target = direction + Center;
		const FVector newPos = FMath::VInterpTo(fish->GetActorLocation(), Target, Deltatime, 12.0f);

		fish->SetActorLocation(newPos , false);

		const FVector MovementDir = (Target-fish->GetActorLocation()).GetSafeNormal();
		const FQuat rotationWhileTurning = FQuat::Slerp(fish->GetActorQuat(), LookAt(MovementDir, Up), 0.12f);

		
		fish->SetActorRotation(rotationWhileTurning);

	}
	

}




FQuat USwarmComponent::LookAt(const FVector& lookAt, const FVector& upDirection)
{


	return FRotationMatrix::MakeFromXZ(lookAt,upDirection).ToQuat();

}

