// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/BagCharacter.h"

#include "EnemyAIController.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ABagCharacter::ABagCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxCollider = CreateDefaultSubobject<UBoxComponent>("BoxCollider");
	BoxCollider->IgnoreActorWhenMoving(this, true);
	

}

// Called when the game starts or when spawned
void ABagCharacter::BeginPlay()
{
	Super::BeginPlay();
	BoxCollider->OnComponentBeginOverlap.AddDynamic(this, &ABagCharacter::OnOverlapBegin);
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

void ABagCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag("Player"))
	{
		//Damage
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Damage player"));


	}



}