// Fill out your copyright notice in the Description page of Project Settings.


#include "Variant_TwinStick/AI/TrolleyNetActor.h"


// Sets default values
ATrolleyNetActor::ATrolleyNetActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponent"));
    RootComponent = CollisionComponent;

    CollisionComponent->SetBoxExtent(FVector(50.f, 500.f, 100.f));

    CollisionComponent->SetCollisionProfileName(TEXT("OverlapAll"));

    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

}

// Called when the game starts or when spawned
void ATrolleyNetActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATrolleyNetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AddActorWorldOffset(GetActorForwardVector() * speed * DeltaTime);

}

