// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ScubaSaverPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BOIDSWARM_API AScubaSaverPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AScubaSaverPlayerController();

protected:
	/** Initialize input bindings */
	//virtual void SetupInputComponent() override;
private:

	//void OnMouseX(float AxisValue);
	//void OnMouseY(float AxisValue);
};
