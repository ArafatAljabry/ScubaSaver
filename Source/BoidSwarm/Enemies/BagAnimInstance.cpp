// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/BagAnimInstance.h"

void UBagAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Bag = Cast<ABagCharacter>(TryGetPawnOwner());
}

void UBagAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!Bag)
	{
		Bag = Cast<ABagCharacter>(TryGetPawnOwner());
	}
}
