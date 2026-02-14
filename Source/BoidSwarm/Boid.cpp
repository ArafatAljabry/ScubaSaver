// Fill out your copyright notice in the Description page of Project Settings.


#include "Boid.h"
#include "TwinStickNPC.h"
#include "TrolleyNetActor.h"
#include <Kismet/KismetMathLibrary.h>

#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "Enemies/BagCharacter.h"
#include "Engine/Engine.h" //debug messages
#include "Kismet/GameplayStatics.h"


TArray<TWeakObjectPtr<ABoid>> ABoid::AllBoids;


FQuat LookAt(const FVector& lookAt, const FVector& upDirection) {
	return FRotationMatrix::MakeFromXZ(lookAt, upDirection).ToQuat();
}

// Sets default values
ABoid::ABoid()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;	    
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("OverlapAll"));
	/*
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(GetCapsuleComponent());
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);*/
	/*
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;*/
}


void ABoid::UpdateTextureColor(const FLinearColor& NewColor)
{
	if (!DynamicMaterial)
	{
		UMaterialInterface* BaseMaterial = MeshComponent->GetMaterial(0);
		DynamicMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this);
		MeshComponent->SetMaterial(0, DynamicMaterial);
	}

	if (DynamicMaterial)
	{
		// "Color" must match your Vector Parameter in the material
		DynamicMaterial->SetVectorParameterValue(FName("Color"), NewColor);
	}
}


FLinearColor ABoid::UpdateTextureColorF(const FLinearColor& NewColor)
{
	if (!DynamicMaterial)
	{
		UMaterialInterface* BaseMaterial = MeshComponent->GetMaterial(0);
		DynamicMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this);
		MeshComponent->SetMaterial(0, DynamicMaterial);
	}

	if (DynamicMaterial)
	{
		DynamicMaterial->SetVectorParameterValue(FName("Color"), NewColor);
	}
	return NewColor;
}

void ABoid::updateHealth(float num)
{
	float newHealth = health + num;
	health = FMath::Clamp(newHealth, 0.f, maxHealth);
	if (health == 0)
	{

		//Play sound
		if (!DeathSound)
		{
			return;
		}
		//Create the component
		DeathAudioComp = UGameplayStatics::SpawnSound2D(
			this,
			DeathSound,
			1.0f,      // Volume
			1.0f,      // Pitch
			0.0f,      // Start time
			nullptr,   // Concurrency
			true,      // Persist
			false      // Don't auto destroy
		);
		if (DeathAudioComp)
			DeathAudioComp->Play();
		spawner->DeleteBoidFromArray(this);
		Destroy();
	}

	
	// Fish die when HP equals 0, duh.
	//else {
	//	float RedTint = (1-((maxHealth*health)/100)/100);
	//	RedTint = FMath::Clamp(RedTint, 0.0f, 1.0f);
	//	FLinearColor NewColor = FLinearColor(RedTint, 0, 0, 1);
	//	UpdateTextureColor(NewColor);
	//	/*MyColor = NewColor;*/
	//}

}

// Called when the game starts or when spawned
void ABoid::BeginPlay()
{
	Super::BeginPlay();
	AllBoids.Add(this);
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ABoid::OnCapsuleBeginOverlap);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &ABoid::OnCapsuleEndOverlap);
	if (spawner == nullptr) return;
	Radius = FVector(spawner->radius, spawner->radius, spawner->radius / 2.0f);
	Center = spawner->spawnOrigin;

GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ABoid::OnCapsuleBeginOverlap);
}



// DANGER ZONE

//I need to have one central boid with an index 0, and the rest of the boids to spawn at a 137.5 degree angle separation from the previous one.

void ABoid::AssignPosition(const TArray<AActor*>& Actors, const FVector& Center, float DistanceIncrement)
{
	constexpr float GoldenAngleDegrees = 137.5f;
	constexpr float GoldenAngleRadians = FMath::DegreesToRadians(GoldenAngleDegrees);

	for (int32 Index = 0; Index < Actors.Num(); ++Index)
	{
		AActor* Actor = Actors[Index];
		if (!IsValid(Actor))
		{
			continue; // Skip if null or eliminated
		}

		// Calculate angle and radius for this index
		float Angle = Index * GoldenAngleRadians;
		float Radius = DistanceIncrement * Index;

		// Calculate new location on XY plane
		FVector NewLocation;
		NewLocation.X = Center.X + Radius * FMath::Cos(Angle);
		NewLocation.Y = Center.Y + Radius * FMath::Sin(Angle);
		NewLocation.Z = Center.Z;

		// Set actor location without physics sweep
		Actor->SetActorLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);
	}
}

// DANGER ZONE end


// Called every frame
void ABoid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!spawner) return;

	
	if (Radius == FVector::ZeroVector || Center == FVector::ZeroVector) {
		if (spawner == nullptr) return;
		Radius = FVector(spawner->radius, spawner->radius, spawner->radius / 2.0f);
	}


	const FVector MyPos = GetActorLocation();
	const FVector MyFowardVector = GetActorForwardVector();
	const FVector SwarmCenter = Center + PersonalOffset;


	const bool bInside = UKismetMathLibrary::IsPointInBox(MyPos, SwarmCenter, Radius);


	if (bInside == false)
	{
		isTurning = true;
		/*	 To player :
		FVector direction = spawner->GetActorLocation() - GetActorLocation();

		FQuat rotationWhileTurning = FQuat::Slerp(
			GetActorRotation().Quaternion(), 
			LookAt(direction, GetActorUpVector()),
			turn);
		SetActorRotation(rotationWhileTurning);
		*/

		const FVector ToSwarmCenter = (SwarmCenter - MyPos).GetSafeNormal();
		if (!ToSwarmCenter.IsNearlyZero())
		{
			const FQuat TargetQ = FRotationMatrix::MakeFromXZ(ToSwarmCenter, GetActorUpVector()).ToQuat();
			const FQuat NewQ = FQuat::Slerp(GetActorQuat(), TargetQ, FMath::Clamp(ReturnToCenterWeight * DeltaTime, 0.f, 1.f));
			SetActorRotation(NewQ);
		}

		AddActorWorldOffset(MyFowardVector * speed * DeltaTime, true);
		return;
	}
	else
	{
		isTurning = false;
	}

	if(!isTurning){
		if(FMath::RandRange(0,10)<2){speed = FMath::RandRange(minspeed, maxspeed);}
	}

	// DANGER ZONE !!!


	// !!!

	FVector Sep = FVector::ZeroVector;
	FVector Ali = FVector::ZeroVector;
	FVector Coh = FVector::ZeroVector;


	const FVector ToCenter = (Center - MyPos);
	FVector Orbit = FVector::ZeroVector;

	if (!ToCenter.IsNearlyZero())
	{
		const FVector Radial = ToCenter.GetSafeNormal();
		Orbit = FVector::CrossProduct(GetActorUpVector(), Radial).GetSafeNormal(); 
	}

	
	FVector CenterPull = FVector::ZeroVector;
	if (!ToCenter.IsNearlyZero())
	{
		CenterPull = ToCenter.GetSafeNormal();
	}


	FVector DesiredDir;

	DesiredDir = DesiredDir.GetSafeNormal();

	//try FQuat LookAt() then Slerp


	//Check if DesiredDir is used at all

	if (!DesiredDir.IsNearlyZero())
	{
		//const FQuat TargetQ = FRotationMatrix::MakeFromXZ(DesiredDir, GetActorUpVector()).ToQuat();
		const float Alpha = FMath::Clamp(turn, 0.f, 1.f);
		FQuat TargetQ = LookAt(DesiredDir, FVector(0,0,1));
		const FQuat NewQ = FQuat::Slerp(GetActorQuat(), TargetQ, 1-pow(Alpha, DeltaTime)); //Instead of Alpha; may need to use Godot version
		SetActorRotation(NewQ);
	}

	// DANGER ZONE end!!!

	AddActorWorldOffset(MyFowardVector * speed * DeltaTime);

}

// Called to bind functionality to input
void ABoid::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABoid::OnCapsuleBeginOverlap(UPrimitiveComponent* OverlappedComp,
	AActor* OverlapActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	
	if (OverlapActor != this)
	{
		// Example: Show debug message when overlap occurs
		if (GEngine)
		{
			
			if (ATwinStickNPC* NPC = Cast<ATwinStickNPC>(OverlapActor))
			{
				updateHealth(-25); //Deal damage to boids :<
				NPC->ProjectileImpact(GetActorForwardVector()); //Make enemies die :>
			}


			if (ABagCharacter* BagNpc = Cast<ABagCharacter>(OverlapActor))
			{
				updateHealth(-25); //Deal damage to boids :<
				BagNpc->ProjectileImpact(GetActorForwardVector());
			}

			if (ABoid* OtherBoid = Cast<ABoid>(OverlapActor))
			{
				OverlappingBoids.Add(OtherBoid);
			}
			/*
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan,
				FString::Printf(TEXT("Overlap with: %s"), *OverlapActor->GetName()));*/

			if (ATrolleyNetActor* Trolley = Cast<ATrolleyNetActor>(OverlapActor)) {
				updateHealth(Trolley->damage); //Deal damage to boids :<
			}


		}

		// TODO: Add your custom trigger logic here (e.g., elimination, gameplay events)
	}
}


void ABoid::OnCapsuleEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor || OtherActor == this) return;

	if (ABoid* OtherBoid = Cast<ABoid>(OtherActor))
	{
		OverlappingBoids.Remove(OtherBoid);
	}
}

