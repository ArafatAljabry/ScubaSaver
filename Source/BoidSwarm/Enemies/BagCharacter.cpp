// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/BagCharacter.h"

#include "EnemyAIController.h"

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

