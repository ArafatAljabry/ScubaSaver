// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter.h"

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


	//Create capsule component for keeping the fishes in a certain area, and for future collision with the player
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComp->InitSphereRadius(m_FishCount * m_SizeForOneFish);

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
			Fish->Play(true); // Loop animation
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

	FVector Delta = FVector(m_MouseYInput, m_MouseXInput, 0.f) * m_CameraSpeed * DeltaTime;
	AddActorWorldOffset(Delta, false);

	calculateSwarmForce(DeltaTime);
	for (int i = 0; i < m_FishCount; ++i)
	{
		//Move each fish forward, direction is updated in calculateSwarmForce, and speed is constant for now
		m_FishComponents[i]->AddWorldOffset(Boids[i].Velocity * DeltaTime);
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
}

void APlayerCharacter::OnMouseY(float value)
{
	m_MouseYInput = value;
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
							seekForce		* m_SeekWeight ;

		/*
		 *  After all the forces are computed, we apply them to the velocity and position of the boid.
		 */
		FVector acc = totalForce;
		i.Velocity += acc * seekTarget.GetSafeNormal().Length();
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