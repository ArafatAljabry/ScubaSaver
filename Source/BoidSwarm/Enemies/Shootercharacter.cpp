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

	ShootLoc = CreateDefaultSubobject<USceneComponent>(TEXT("shootloc"));
	ShootLoc->SetupAttachment(GetMesh());

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
	GEngine->AddOnScreenDebugMessage(
		-1,                      // Key (-1 = new line)
		5.f,                     // Display time in seconds
		FColor::Yellow,          // Text color
		TEXT("outch!")  // Message
	);

	Destroy();
}

void AShootercharacter::ProjectileImpact()
{
	ATwinStickGameMode* GM = Cast<ATwinStickGameMode>(GetWorld()->GetAuthGameMode());


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
		TEXT("outch1!")  // Message
	);
	// deactivate character movement
	GetCharacterMovement()->Deactivate();



	// randomly spawn a pickup
	if (FMath::RandRange(0, 100) <= FishSpawnPercentage)
	{

		
		GM->GotFish();
			
		
		
	}
	GM->DecreaseNPCs();
	FTimerHandle Timer;
	// defer destruction
	GetWorld()->GetTimerManager().SetTimer(Timer, this, &AShootercharacter::Killed, 0.1, false);
}

void AShootercharacter::Shoot()
{
	if (bHit)
	{
		return;
	}
	FacePlayer();
	if (!bCanShoot)
	{
		return;
	}

	bCanShoot = false;

	FireProjectile();

	GetWorld()->GetTimerManager().SetTimer(timerShoot, this, &AShootercharacter::ResetFire, FireRate, false);

}

void AShootercharacter::ResetFire()
{
	//UE_LOG(LogTemp, Warning, TEXT("ReadyToFire"));
	bCanShoot = true;
	GetWorld()->GetTimerManager().ClearTimer(timerShoot);
}

void AShootercharacter::FireProjectile()
{
	//UE_LOG(LogTemp, Warning, TEXT("Fire"));
	FVector Start = ShootLoc->GetComponentLocation();
	FVector Target = Player->GetActorLocation();

	FVector Direction = (Target - Start).GetSafeNormal();

	FActorSpawnParameters Params;
	Params.Owner = this;
	

	AMyActorProjectile* bullet = GetWorld()->SpawnActor<AMyActorProjectile>(ProjectileClass, Start, Direction.Rotation(), Params);

	if (bullet)
	{
		bullet->collider->IgnoreActorWhenMoving(this, true);
	}
}

void AShootercharacter::FacePlayer()
{
	FVector Dir = Player->GetActorLocation() - GetActorLocation();
	Dir.Z = 0;

	if (!Dir.IsNearlyZero())
	{
		SetActorRotation(Dir.Rotation());
	}


}



