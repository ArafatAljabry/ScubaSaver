// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/MyActorProjectile.h"

#include "PlayerCharacter.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AMyActorProjectile::AMyActorProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	collider = CreateDefaultSubobject<USphereComponent>(TEXT("Collider"));
	RootComponent =collider;

	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MEsh"));
	mesh->SetupAttachment(RootComponent);;

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


	
}

// Called every frame
void AMyActorProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMyActorProjectile::DestroySelf()
{
	Destroy();
}

void AMyActorProjectile::NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other,
	class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal,
	FVector NormalImpulse, const FHitResult& Hit)
{
	
	// apply damage to the character
	if (TObjectPtr<APlayerCharacter> FishCharacter = Cast<APlayerCharacter>(Other))
	{

			UE_LOG(LogTemp, Warning, TEXT("[Bag] NotifyHit fired: Other=%s, MyComp=%s, OtherComp=%s"),
				*GetNameSafe(Other), *GetNameSafe(MyComp), *GetNameSafe(OtherComp));

			FishCharacter->RemoveFish();
	}

	

}






