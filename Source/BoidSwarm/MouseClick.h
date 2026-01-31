// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MouseClick.generated.h"

UCLASS()
class BOIDSWARM_API AMouseClick : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMouseClick();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void BindToInput();

	void LeftMouseClickFunction();

	void RightMouseClickFunction();

	float mouseX;
	float mouseY;

};
