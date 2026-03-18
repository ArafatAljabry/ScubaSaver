// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Shootercharacter.h"

#include "MyActorProjectile.h"
#include "TwinStickGameMode.h"
#include "Components/AudioComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AShootercharacter::AShootercharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AShootercharacter::BeginPlay()
{
	Super::BeginPlay();

	if (!Player)
	{
		Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	}

}

// Called every frame
void AShootercharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!Player)
	{
		Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	}

}

// Called to bind functionality to input
void AShootercharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AShootercharacter::Killed()
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

	Destroy();
}

void AShootercharacter::ProjectileImpact(const FVector& ForwardVector)
{
	if (bHit)
	{
		return;
	}

	// raise the hit flag
	bHit = true;
	//GEngine->AddOnScreenDebugMessage(
	//	-1,                      // Key (-1 = new line)
	//	5.f,                     // Display time in seconds
	//	FColor::Yellow,          // Text color
	//	TEXT("outch!")  // Message
	//);

	// deactivate character movement
	GetCharacterMovement()->Deactivate();



	// randomly spawn a pickup
	if (FMath::RandRange(0, 100) <= FishSpawnPercentage)
	{

		if (ATwinStickGameMode* GM = Cast<ATwinStickGameMode>(GetWorld()->GetAuthGameMode()))
		{
			GM->GotFish();
		}
		
	}

	FTimerHandle Timer;
	// defer destruction
	GetWorld()->GetTimerManager().SetTimer(Timer, this, &AShootercharacter::Killed, 0.1, false);
}

void AShootercharacter::Shoot()
{
	if (!bCanShoot)
	{
		return;
	}

	bCanShoot = false;

	FireProjectile();

	FTimerHandle timer;
	GetWorld()->GetTimerManager().SetTimer(timer, this, &AShootercharacter::ResetFire, FireRate, false);

}

void AShootercharacter::ResetFire()
{
	bCanShoot = true;
}

void AShootercharacter::FireProjectile()
{

	FVector Start = ShootLoc->GetComponentLocation() + GetActorForwardVector();
	FVector Target = Player->GetActorLocation();

	FVector Direction = (Target - Start).GetSafeNormal();

	FActorSpawnParameters Params;

	AMyActorProjectile* bullet = GetWorld()->SpawnActor<AMyActorProjectile>(AMyActorProjectile::StaticClass(), Start, FRotator::ZeroRotator, Params);

	
}



