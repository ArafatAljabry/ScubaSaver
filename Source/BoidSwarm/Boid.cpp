// Fill out your copyright notice in the Description page of Project Settings.


#include "Boid.h"
#include "TwinStickNPC.h"
#include "TrolleyNetActor.h"
#include <Kismet/KismetMathLibrary.h>

#include "Components/CapsuleComponent.h"
#include "Enemies/BagCharacter.h"
#include "Engine/Engine.h" //debug messages


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
	if (health == 0) { Destroy(); } // Fish die when HP equals 0, duh.
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

	PersonalOffset = FVector(
		FMath::FRandRange(-Radius.X * 0.3f, Radius.X * 0.3f),
		FMath::FRandRange(-Radius.Y * 0.3f, Radius.Y * 0.3f),
		0.f
	);

	OrbitSign = (FMath::RandBool() ? 1.f : -1.f);

	OrbitPhase = FMath::FRandRange(0.f, 1000.f);

}

// DANGER ZONE

void ABoid::GetNeighbours(TArray<ABoid*>& OutNeighbours) const
{
	OutNeighbours.Reset();

	const FVector MyPos = GetActorLocation();
	const float R2 = NeighbourRadius * NeighbourRadius;

	// Clean stale pointers occasionally
	// (You can do this less often if perf matters)
	for (int32 i = AllBoids.Num() - 1; i >= 0; --i)
	{
		ABoid* B = AllBoids[i].Get();
		if (!B)
		{
			AllBoids.RemoveAtSwap(i);
			continue;
		}
		if (B == this) continue;

		const float DistSq = FVector::DistSquared(MyPos, B->GetActorLocation());
		if (DistSq <= R2)
		{
			OutNeighbours.Add(B);
		}
	}
}

FVector ABoid::GetSeparation(const TArray<ABoid*>& Neighbours) const
{
	FVector Force = FVector::ZeroVector;
	const FVector MyPos = GetActorLocation();
	const float SepR2 = SeparationRadius * SeparationRadius;

	for (ABoid* N : Neighbours)
	{
		if (!N) continue;

		const FVector Delta = MyPos - N->GetActorLocation();
		const float DistSq = Delta.SizeSquared();
		if (DistSq < KINDA_SMALL_NUMBER) continue;

		// optional: apply only inside separation radius
		if (DistSq > SepR2) continue;

		// normalize(direction) / |direction|  == direction / |direction|^2
		Force += Delta / DistSq;
	}

	return Force; // not normalized yet; caller may normalize
}

FVector ABoid::GetAlignment(const TArray<ABoid*>& Neighbours) const
{
	if (Neighbours.Num() == 0) return FVector::ZeroVector;

	FVector AvgHeading = FVector::ZeroVector;
	for (ABoid* N : Neighbours)
	{
		if (!N) continue;
		AvgHeading += N->GetActorForwardVector();
	}

	AvgHeading /= FMath::Max(1, Neighbours.Num());

	// average - heading
	return (AvgHeading - GetActorForwardVector());
}

FVector ABoid::GetCohesion(const TArray<ABoid*>& Neighbours) const
{
	if (Neighbours.Num() == 0) return FVector::ZeroVector;

	FVector localCenter = FVector::ZeroVector;
	for (ABoid* N : Neighbours)
	{
		if (!N) continue;
		localCenter += N->GetActorLocation();
	}

	localCenter /= FMath::Max(1, Neighbours.Num());

	// seek(center): desired direction to center (not full seek with velocity, just direction)
	return (localCenter - GetActorLocation());
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
		GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ABoid::OnCapsuleBeginOverlap);
	}


	const FVector MyPos = GetActorLocation();
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

		AddActorWorldOffset(GetActorForwardVector() * speed * DeltaTime, true);
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

	TArray<ABoid*> Neighbours;
	Neighbours.Reserve(16);

	const float NeighR2 = NeighbourRadius * NeighbourRadius;

	for (int32 i = AllBoids.Num() - 1; i >= 0; --i)
	{
		ABoid* B = AllBoids[i].Get();
		if (!B)
		{
			AllBoids.RemoveAtSwap(i);
			continue;
		}
		if (B == this) continue;

		if (FVector::DistSquared(MyPos, B->GetActorLocation()) <= NeighR2)
		{
			Neighbours.Add(B);
		}
	}

	// !!!

	FVector Sep = FVector::ZeroVector;
	FVector Ali = FVector::ZeroVector;
	FVector Coh = FVector::ZeroVector;

	if (Neighbours.Num() > 0)
	{
		// separation
		const float SepR2 = SeparationRadius * SeparationRadius;
		for (ABoid* N : Neighbours)
		{
			const FVector Delta = MyPos - N->GetActorLocation();
			const float DistSq = Delta.SizeSquared();
			if (DistSq < KINDA_SMALL_NUMBER) continue;
			if (DistSq > SepR2) continue;

			Sep += Delta / DistSq; 
		}

		// alignment
		FVector AvgHeading = FVector::ZeroVector;
		FVector AvgPos = FVector::ZeroVector;

		for (ABoid* N : Neighbours)
		{
			AvgHeading += N->GetActorForwardVector();
			AvgPos += N->GetActorLocation();
		}

		AvgHeading /= Neighbours.Num();
		AvgPos /= Neighbours.Num();

		Ali = (AvgHeading - GetActorForwardVector());
		Coh = (AvgPos - MyPos); 
	}

	//if (!Sep.IsZero()) Sep = Sep.GetSafeNormal();
	//if (!Ali.IsZero()) Ali = Ali.GetSafeNormal();
	//if (!Coh.IsZero()) Coh = Coh.GetSafeNormal();

	const FVector FlockForce =
		Sep * SeparationWeight +
		Ali * AlignmentWeight +
		Coh * CohesionWeight;


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


	FVector DesiredDir =
		GetActorForwardVector() +
		FlockForce +
		Orbit * OrbitWeight +
		CenterPull * CenterPullWeight;

	//DesiredDir = DesiredDir.GetSafeNormal();

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

	AddActorWorldOffset(GetActorForwardVector() * speed * DeltaTime);

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

