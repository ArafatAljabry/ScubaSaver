// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SkeletalMeshComponent.h"

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

	//Create scene component and set it as root
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;
	// Create the camera boom component
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));

	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false;

	// Create the camera component
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));

	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false;

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	for (int32 i = 0; i < FishCount; i++)
	{
		FString name = FString::Printf(TEXT("Fish_%d"), i);
		USkeletalMeshComponent* Fish = NewObject<USkeletalMeshComponent>(this, *name);

		Fish->RegisterComponent();
		Fish->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

		if (FishMesh)
		{
			Fish->SetSkeletalMesh(FishMesh);
		}

		if (SwimAnimation)
		{
			Fish->SetAnimationMode(EAnimationMode::AnimationSingleNode);
			Fish->SetAnimation(SwimAnimation);
			Fish->Play(true); // Loop animation
		}

		// Random initial location
		Fish->SetRelativeLocation(FVector(FMath::FRandRange(-300.0f, 300.0f),
			FMath::FRandRange(-300.0f, 300.0f),
			FMath::FRandRange(-100.0f, 100.0f)));

		FishComponents.Add(Fish);

		//Creating a boid implicitly linked to this mesh since it is created at the same time,
		//and has the same index in the array
		FBoid boid;
		boid.Position = Fish->GetRelativeLocation();
		boid.Velocity = FMath::VRand() * 100.f;
		Boids.Add(boid);
	}
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);



	for (FBoid i : Boids)
	{
		//Reseting values at start
		FVector separationForce = FVector::ZeroVector;
		FVector alignmentForce = FVector::ZeroVector;
		FVector cohesionForce = FVector::ZeroVector;

		i.neighbourCount = 0;
		//Computing forces
		for (FBoid j : Boids)
		{
			//Seperation
			float distanceFromNeighbour = FVector::Distance(i.Position, j.Position);
			if(distanceFromNeighbour < NeighborRadius && distanceFromNeighbour > 0)
			{
				i.neighbourCount++;
				FVector awayVec = i.Position - j.Position;
				
				//In case of small numbers, we want to avoid the force to be too strong, so we check if the vector is not nearly zero before normalizing it
				if (!awayVec.IsNearlyZero())
				{
					separationForce += awayVec.GetSafeNormal() / distanceFromNeighbour;
				}
			}
		}
		separationForce /= i.neighbourCount > 0 ? i.neighbourCount : 1; // Avoid division by zero
		separationForce *= SeparationWeight;

		FVector totalForce =	separationForce * SeparationWeight + // acc = w*seperation + w*alignment + w*cohesion
								alignmentForce  * AlignmentWeight + 
								cohesionForce	* CohesionWeight;


		/*
		 *  After all the forces are computed, we apply them to the velocity and position of the boid.
		 */
		i.Velocity += totalForce * DeltaTime;

		/* NOTE:Shorthand to limit speed: 
		 * if/else --> condition ? expression_if_true : expression_if_false
		 */
		i.Position += i.Velocity.Length() > MaxSpeed ? i.Velocity.GetSafeNormal() * MaxSpeed : i.Velocity; 
	}
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
