// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter.h"

#include "TwinStickGameMode.h"
#include "Animation/AnimSingleNodeInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimationAsset.h"
	

#include "Components/SphereComponent.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);


	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create the camera boom component
	m_CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));

	m_CameraBoom->SetupAttachment(GetRootComponent());
	m_CameraBoom->SetUsingAbsoluteRotation(true);
	m_CameraBoom->TargetArmLength = 800.f;
	m_CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	m_CameraBoom->bDoCollisionTest = false;

	// Create the camera component
	m_TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));

	m_TopDownCameraComponent->SetupAttachment(m_CameraBoom, USpringArmComponent::SocketName);
	m_TopDownCameraComponent->bUsePawnControlRotation = false;

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	for (int32 i = 0; i < m_FishCount; i++)
	{
		FString name = FString::Printf(TEXT("Fish_%d"), i);
		USkeletalMeshComponent* Fish = NewObject<USkeletalMeshComponent>(this, *name);

		
		if (m_FishMesh)
		{
			Fish->SetSkeletalMesh(m_FishMesh);
			Fish->SetMaterial(0, m_Material);
			Fish->SetupAttachment(GetRootComponent());
			Fish->SetMobility(EComponentMobility::Movable);
			Fish->SetRelativeScale3D(FVector(0.1f)); //Scale the fish, they be too big
			Fish->RegisterComponent();
			Fish->SetVisibility(true);
		}
	
		

		if (m_SwimAnimation)
		{
			Fish->SetAnimationMode(EAnimationMode::AnimationSingleNode);
			Fish->SetAnimation(m_SwimAnimation);

			if (UAnimSingleNodeInstance* SI = Fish->GetSingleNodeInstance())
			{
				Fish->Play(true); // Loop animation
				SI->SetPlayRate(FMath::FRandRange(0.1f, 0.4f)); // slow, varied
			}
			
		}

		// Random initial location
		Fish->SetRelativeLocation(FVector(FMath::FRandRange(-300.0f, 300.0f),
										  FMath::FRandRange(-300.0f, 300.0f),
										  FMath::FRandRange(-100.0f, 100.0f)));

		m_FishComponents.Add(Fish);

		//Creating a boid implicitly linked to this mesh since it is created at the same time,
		//and has the same index in the array
		FBoidData boid;
		boid.Direction	  = FVector(FMath::FRandRange(0.f,1.f),0.f,0.f);
		boid.Velocity	  = boid.Direction * (m_MaxSpeed * 0.5f); //Start at half speed
		boid.targetOffset = FVector(FMath::FRandRange(0.0f, 30.0f), 
									FMath::FRandRange(0.0f, 30.0f),
									0.0f);
		Boids.Add(boid);
	}
	UE_LOG(LogTemp, Warning, TEXT("Fish created: %d"), m_FishComponents.Num());
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	calculateSwarmForce(DeltaTime);
	for (int i = 0; i < m_FishCount; ++i)
	{
		//Move each fish forward, direction is updated in calculateSwarmForce, and speed is constant for now
		m_FishComponents[i]->AddWorldOffset(Boids[i].Velocity * DeltaTime);
	}
	
	//here fix this
	FVector target;
	if (GetCursorWorldTarget(target))
	{
		FVector direction = target - GetActorLocation();
		

		direction.Normalize();
		AddMovementInput(direction, 1.0);
	}
		
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Turn and LookUp is default axis mapping from project settings
	InputComponent->BindAxis("Turn", this, &APlayerCharacter::OnMouseX);
	InputComponent->BindAxis("LookUp", this, &APlayerCharacter::OnMouseY);

}

void APlayerCharacter::OnMouseX(float value)
{
	m_MouseXInput = value;
	AddMovementInput(GetActorForwardVector(), m_MouseXInput);
	
}

void APlayerCharacter::OnMouseY(float value)
{
	m_MouseYInput = value;
	AddMovementInput(GetActorRightVector(), m_MouseYInput);
}

bool APlayerCharacter::GetCursorWorldTarget(FVector& OutTarget) const
{
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		FHitResult Hit;
		if (PC->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, Hit))
		{
			OutTarget = Hit.Location;
			return true;
		}

		// Fallback: intersect mouse ray with plane Z = actor Z
		FVector WorldOrigin, WorldDir;
		if (PC->DeprojectMousePositionToWorld(WorldOrigin, WorldDir))
		{
			const float PlaneZ = GetActorLocation().Z;
			const float denom = WorldDir.Z;
			if (FMath::Abs(denom) > KINDA_SMALL_NUMBER)
			{
				const float T = (PlaneZ - WorldOrigin.Z) / denom;
				if (T > 0.f)
				{
					OutTarget = WorldOrigin + T * WorldDir;
					return true;
				}
			}
		}
	}
	return false;
}

void APlayerCharacter::calculateSwarmForce( float dt)
{
	for (int a = 0; a < m_FishCount; a++)
	{
		FBoidData& i = Boids[a]; // Ref to boid A
		FVector boid_a_position = m_FishComponents[a]->GetComponentLocation();
		
		//Resetting values at start
		FVector separationForce		= FVector::ZeroVector;
		FVector alignmentForce		= FVector::ZeroVector;
		FVector cohesionForce		= FVector::ZeroVector;
		FVector seekForce			= FVector::ZeroVector;

		i.neighbourCount = 0;

		//Computing forces
		for (int b = 0; b < m_FishCount; b++)
		{
			if (a == b) continue;
			FVector boid_b_position = m_FishComponents[b]->GetComponentLocation();
			FBoidData& j = Boids[b];
			float distanceFromNeighbour = FVector::Distance(boid_a_position, boid_b_position);
			if (distanceFromNeighbour < m_NeighborRadius && distanceFromNeighbour > 0)
			{
				i.neighbourCount++;

				//Alignment
				alignmentForce += j.Velocity;

				//Cohesion
				cohesionForce += boid_b_position;

				//seperation
				FVector away = boid_a_position - boid_b_position;
				float denom = FMath::Max(distanceFromNeighbour * distanceFromNeighbour, 1.0f);
				separationForce += away / denom;
			}
		}
		separationForce = separationForce.GetSafeNormal();

		FVector seekTarget{};
		bool bHasTarget = GetCursorWorldTarget(seekTarget);
		seekTarget = seekTarget + i.targetOffset;
		alignmentForce /= i.neighbourCount > 0 ? i.neighbourCount : 1; // Avoid division by zero
		alignmentForce = alignmentForce.GetSafeNormal(); // Avoid division by zero

		cohesionForce /= i.neighbourCount > 0 ? i.neighbourCount : 1; // Avoid division by zero
		cohesionForce = (cohesionForce - boid_a_position).GetSafeNormal(); // Direction towards the center of mass of the neighbors

		if (bHasTarget)
		{
			FVector desired = (seekTarget - boid_a_position); 
			desired.Z += 200; // Keep movement in the XY plane
			FVector desiredVelocity = desired.GetSafeNormal() * m_MaxSpeed;
			seekForce = (desiredVelocity - i.Velocity).GetSafeNormal();
			
			if (desired.Length() < m_OrbitRadius)
			{
				FVector radialDirection = desired.GetSafeNormal();
				FVector TangentDirection = FVector::CrossProduct(radialDirection, GetActorUpVector());
				
				// Blend: Apparently this should keep it obiting properly
				float radiusError = desired.Length() - m_OrbitRadius;  // negative = too close
				FVector radialCorrection = radialDirection * radiusError * 0.01f; // tune multiplier

				seekForce = (TangentDirection + radialCorrection).GetSafeNormal();
			}
		}
		FVector totalForce =
							separationForce * m_SeparationWeight + // acc = w*separation + w*alignment + w*cohesion
							alignmentForce	* m_AlignmentWeight	 +
							cohesionForce	* m_CohesionWeight   +
							seekForce		* m_SeekWeight;

		/*
		 *  After all the forces are computed, we apply them to the velocity and position of the boid.
		 */
		FVector acc = totalForce;
		i.Velocity += acc;
		i.Velocity = i.Velocity.GetClampedToMaxSize(m_MaxSpeed);
		
		//Direction is normalized velocity
		i.Direction = i.Velocity.GetSafeNormal();
		//Rotate fish to face direction
		if (!i.Direction.IsNearlyZero())
		{
			FRotator TargetRotation = i.Direction.ToOrientationRotator();
			FRotator CurrentRotation = m_FishComponents[a]->GetRelativeRotation();
			FRotator SmoothedRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, dt, 100.0f);
			m_FishComponents[a]->SetRelativeRotation(SmoothedRotation);
		} 
	}
}

void APlayerCharacter::AdjustFishVolume()
{
	SphereComp->InitSphereRadius(m_FishCount * m_SizeForOneFish);

}


int32 APlayerCharacter::CreateOneFish()
{
	if (!m_FishMesh) // Require a mesh to create a fish
	{
		UE_LOG(LogTemp, Warning, TEXT("CreateOneFish: No m_FishMesh set."));
		return INDEX_NONE;
	}

	const int32 Index = m_FishComponents.Num();

	// Unique component name
	USkeletalMeshComponent* Fish = NewObject<USkeletalMeshComponent>(
		this,
		*FString::Printf(TEXT("Fish_%d"), Index)
	);

	if (!Fish)
	{
		UE_LOG(LogTemp, Warning, TEXT("CreateOneFish: Failed to allocate skeletal mesh component."));
		return INDEX_NONE;
	}

	// Basic setup
	Fish->SetMobility(EComponentMobility::Movable);
	Fish->SetupAttachment(GetRootComponent());
	Fish->SetRelativeScale3D(FVector(0.1f));
	Fish->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Fish->SetCastShadow(false);
	Fish->bReceivesDecals = false;

	Fish->SetSkeletalMesh(m_FishMesh);

	// Register before accessing AnimSingleNodeInstance
	Fish->RegisterComponent();
	Fish->SetVisibility(true);

	// Animation (single-node)
	if (m_SwimAnimation)
	{
		Fish->SetAnimationMode(EAnimationMode::AnimationSingleNode);
		Fish->SetAnimation(m_SwimAnimation);

		if (UAnimSingleNodeInstance* SI = Fish->GetSingleNodeInstance())
		{
			SI->SetPlaying(true);
			SI->SetPlayRate(FMath::FRandRange(0.6f, 1.2f));
		}
	}

	// Initial world placement near actor
	const FVector RandOffset(
		FMath::FRandRange(-300.f, 300.f),
		FMath::FRandRange(-300.f, 300.f),
		FMath::FRandRange(-100.f, 100.f)
	);
	Fish->SetWorldLocation(GetActorLocation() + RandOffset);

	// Push into arrays
	m_FishComponents.Add(Fish);

	FBoidData NewBoid{};
	NewBoid.Direction = FVector(FMath::FRandRange(0.f, 1.f), 0.f, 0.f).GetSafeNormal();
	NewBoid.Velocity = NewBoid.Direction * (m_MaxSpeed * 0.5f);
	NewBoid.targetOffset = FVector(FMath::FRandRange(0.0f, 30.0f),
		FMath::FRandRange(0.0f, 30.0f),
		0.0f);
	Boids.Add(NewBoid);

	// Keep count in sync, and update sphere
	m_FishCount = m_FishComponents.Num();
	UpdateFishVolume();

	return Index;
}

void APlayerCharacter::DestroyFishAt(int32 Index)
{
	if (!m_FishComponents.IsValidIndex(Index) || !Boids.IsValidIndex(Index))
	{
		return;
	}

	// Destroy the component
	if (USkeletalMeshComponent* Fish = m_FishComponents[Index])
	{
		Fish->DestroyComponent();
	}

	// Swap-remove from arrays (O(1)) to keep them in sync and compact
	m_FishComponents.RemoveAtSwap(Index);
	Boids.RemoveAtSwap(Index);

	// If we swapped in the last element, its component name/Anim is still valid.
	// No need to rename, indices are internal.

	// Keep count in sync, and update sphere
	m_FishCount = m_FishComponents.Num();
	UpdateFishVolume();
}

void APlayerCharacter::UpdateFishVolume()
{
	// Keep your existing logic, or call AdjustFishVolume()
	if (SphereComp)
	{
		GetCapsuleComponent()->SetCapsuleRadius(m_FishCount * m_SizeForOneFish);
	}
	// Or:
	// AdjustFishVolume();
}

void APlayerCharacter::AddFish(int32 Count /*=1*/)
{
	Count = FMath::Max(0, Count);
	for (int32 i = 0; i < Count; ++i)
	{
		CreateOneFish();
	}

	UE_LOG(LogTemp, Log, TEXT("AddFish: now have %d fish."), m_FishComponents.Num());
}

void APlayerCharacter::RemoveFish(int32 Count)
{
	Count = FMath::Max(0, Count);

	// Remove up to Count, clamped to current size
	const int32 ToRemove = FMath::Min(Count, m_FishComponents.Num());

	for (int32 i = 0; i < ToRemove; ++i)
	{
		// Remove last element for O(1) (or choose a random index)
		const int32 LastIndex = m_FishComponents.Num() - 1;
		DestroyFishAt(LastIndex);
	}
	if (ATwinStickGameMode* GM = Cast<ATwinStickGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GM->LostFish();
	}

	UE_LOG(LogTemp, Log, TEXT("RemoveFish: now have %d fish."), m_FishComponents.Num());
}

int32 APlayerCharacter::GetFishAmount() const
{
	return m_FishComponents.Num();
}
