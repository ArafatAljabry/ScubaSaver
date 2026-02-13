// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/BagCharacter.h"

#include "EnemyAIController.h"
#include "TwinStickGameMode.h"
#include "TwinStickNPCDestruction.h"
#include "TwinStickPickup.h"
#include "Components/AudioComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABagCharacter::ABagCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	

}

// Called when the game starts or when spawned
void ABagCharacter::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = speed;
	
}

// Called every frame
void ABagCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABagCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


void ABagCharacter::Killed()
{
	if (!SoundBase)
	{
		return;
	}
	//Create the component
	SoundComponent = UGameplayStatics::SpawnSound2D(
		this,
		SoundBase,
		1.0f,      // Volume
		1.0f,      // Pitch
		0.0f,      // Start time
		nullptr,   // Concurrency
		true,      // Persist
		false      // Don't auto destroy
	);
	if (SoundComponent)
		SoundComponent->Play();
}
void ABagCharacter::DestroyActor()
{
	Killed();
	Destroy();
}

void ABagCharacter::NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp,
	bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{

	if (ATwinStickCharacter* PlayerCharacter = Cast<ATwinStickCharacter>(Other))
	{
		// apply damage to the character
		PlayerCharacter->HandleDamage(1.0f, GetActorForwardVector());
	}

}

void ABagCharacter::ProjectileImpact(const FVector& ForwardVector)
{

	// only handle damage if we haven't been hit yet
	if (bHit)
	{
		return;
	}

	// raise the hit flag
	bHit = true;
	GEngine->AddOnScreenDebugMessage(
		-1,                      // Key (-1 = new line)
		5.f,                     // Display time in seconds
		FColor::Yellow,          // Text color
		TEXT("outch!")  // Message
	);

	// deactivate character movement
	GetCharacterMovement()->Deactivate();

	// award points
	if (ATwinStickGameMode* GM = Cast<ATwinStickGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GM->ScoreUpdate(Score);
	}

	// randomly spawn a pickup
	if (FMath::RandRange(0, 100) <= FishSpawnPercentage)
	{
		//
		GEngine->AddOnScreenDebugMessage(
			-1,                      // Key (-1 = new line)
			5.f,                     // Display time in seconds
			FColor::Yellow,          // Text color
			TEXT("im busting")  // Message
		);

		ATwinStickPickup* Pickup = GetWorld()->SpawnActor<ATwinStickPickup>(PickupClass, GetActorTransform());
	}

	// spawn the NPC destruction proxy
	ATwinStickNPCDestruction* DestructionProxy = GetWorld()->SpawnActor<ATwinStickNPCDestruction>(DestructionProxyClass, GetActorTransform());

	DestroyActor();

	// defer destruction
	GetWorld()->GetTimerManager().SetTimer(DestructionTimer, this, &ABagCharacter::Killed, 0.1, false);
}
