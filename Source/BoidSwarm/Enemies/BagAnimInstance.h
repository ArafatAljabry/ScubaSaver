// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BagCharacter.h"
#include "Animation/AnimInstance.h"
#include "BagAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class BOIDSWARM_API UBagAnimInstance : public UAnimInstance
{
	GENERATED_BODY()


public:
	virtual void NativeInitializeAnimation() override;

	UFUNCTION(BlueprintCallable)
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:

	
	UPROPERTY()
	ABagCharacter* Bag;

};
