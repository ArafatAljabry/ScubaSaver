// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/MyActorProjectile.h"

#include "Boid.h"
#include "PlayerCharacter.h"
#include "Shootercharacter.h"
#include "Components/AudioComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMyActorProjectile::AMyActorProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	collider = CreateDefaultSubobject<USphereComponent>(TEXT("Collider"));
	RootComponent =collider;

	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MEsh"));
	mesh->SetupAttachment(RootComponent);;

	collider->SetNotifyRigidBodyCollision(true);
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
	ProjectileMovement->UpdatedComponent = collider;
	ProjectileMovement->InitialSpeed = 0.0f;
	ProjectileMovement->MaxSpeed = 0.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;

}

// Called when the game starts or when spawned
void AMyActorProjectile::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle Timer;

	GetWorld()->GetTimerManager().SetTimer(Timer, this, &AMyActorProjectile::DestroySelf, Life, false);

	collider->OnComponentBeginOverlap.AddDynamic(this, &AMyActorProjectile::OnBeginOverlap);
	
}

// Called every frame
void AMyActorProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (hit)
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AMyActorProjectile::DestroySelf, 0.1, false);
	}

}

void AMyActorProjectile::DestroySelf()
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

void AMyActorProjectile::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this)
		return;

	// Ignore the shooter
	if (OtherActor == GetOwner())
		return;

	// Ignore boids
	if (OtherActor->IsA(ABoid::StaticClass()))
		return;

	// Ignore other shooters
	if (OtherActor->IsA(AShootercharacter::StaticClass()))
		return;

	Destroy();
}








