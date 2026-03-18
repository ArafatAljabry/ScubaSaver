// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/MyActorProjectile.h"

// Sets default values
AMyActorProjectile::AMyActorProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MEsh"));
	RootComponent = mesh;

	collider = CreateDefaultSubobject<USphereComponent>(TEXT("Collider"));
	collider->SetupAttachment(RootComponent);

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


