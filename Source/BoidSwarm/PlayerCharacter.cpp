// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimationAsset.h"
	
#include "PlayerCharacter.h"

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

		
		if (!m_FishMesh)
		{
			UE_LOG(LogTemp, Warning, TEXT("Fish mesh not set"));
		}
		Fish->SetupAttachment(GetRootComponent());
		Fish->SetMobility(EComponentMobility::Movable);
		Fish->SetRelativeScale3D(FVector(1.0f)); //Scale the fish, they be too big
		Fish->RegisterComponent();
		Fish->SetVisibility(true);
		

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
		boid.Position = Fish->GetRelativeLocation();
		boid.Velocity = FMath::VRand() * 100.f;
		Boids.Add(boid);
	}
	UE_LOG(LogTemp, Warning, TEXT("Fish created: %d"), m_FishComponents.Num());
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	for (int a = 0; a < m_FishCount; a++)
	{
		FBoidData& i = Boids[a];
		
		//Resetting values at start
		FVector separationForce = FVector::ZeroVector;
		FVector alignmentForce  = FVector::ZeroVector;
		FVector cohesionForce	= FVector::ZeroVector;

		i.neighbourCount = 0;

		//Computing forces
		for (int b = 0; b < m_FishCount; b++)
		{
			FBoidData& j = Boids[b];
			float distanceFromNeighbour = FVector::Distance(i.Position, j.Position);
			if(distanceFromNeighbour < m_NeighborRadius && distanceFromNeighbour > 0)
			{
				i.neighbourCount++;
				
				//Seperation
				FVector awayVec = i.Position - j.Position;
				
				//In case of small numbers, we want to avoid the force to be too strong, so we check if the vector is not nearly zero before normalizing it
				if (!awayVec.IsNearlyZero())
				{
					separationForce += awayVec.GetSafeNormal() / distanceFromNeighbour;
				}

				//Alignment
				alignmentForce += j.Velocity;

				//Cohesion
				cohesionForce += j.Position;
			}
		}
		//Get mouse input
		FVector right = GetActorRightVector();
		FVector up = GetActorUpVector();

		FVector flockTarget = right * m_MouseXInput * m_MouseSensitivity * DeltaTime;
		flockTarget += up * -m_MouseYInput * m_MouseSensitivity * DeltaTime;
		flockTarget = flockTarget.GetSafeNormal();

		separationForce /= i.neighbourCount > 0 ? i.neighbourCount : 1; // Avoid division by zero

		alignmentForce /= i.neighbourCount > 0 ? i.neighbourCount : 1; // Avoid division by zero
		alignmentForce = alignmentForce.Length() > 0 ? alignmentForce.GetSafeNormal() : FVector(1.0f); // Avoid division by zero

		cohesionForce /= i.neighbourCount > 0 ? i.neighbourCount : 1; // Avoid division by zero
		cohesionForce = (cohesionForce - i.Position).GetSafeNormal(); // Direction towards the center of mass of the neighbors

		FVector totalForce = separationForce * m_SeparationWeight + // acc = w*separation + w*alignment + w*cohesion
			alignmentForce * m_AlignmentWeight +
			cohesionForce * m_CohesionWeight;   //+flockTarget;


		/*
		 *  After all the forces are computed, we apply them to the velocity and position of the boid.
		 */
		i.Velocity += totalForce;
		i.Velocity = i.Velocity.GetClampedToMaxSize(m_MaxSpeed); // Limit the velocity to MaxSpeed
		
		/* NOTE:Shorthand to limit speed: 
		 * if/else --> condition ? expression_if_true : expression_if_false
		 */
		i.Position += i.Velocity * DeltaTime;

		m_FishComponents[a]->SetRelativeLocation(i.Position);
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
