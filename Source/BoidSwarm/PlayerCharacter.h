// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"


USTRUCT()
struct FBoid
{
	GENERATED_BODY()

	FVector Position{ FVector::ZeroVector };
	FVector Velocity{ FVector::ZeroVector };

	int neighbourCount{ 0 };
};

UCLASS()
class BOIDSWARM_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Scenecomponent */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* SceneComponent;

	UPROPERTY(EditAnywhere)
	USkeletalMesh* FishMesh;

	UPROPERTY(EditAnywhere)
	UAnimationAsset* SwimAnimation;
	
	TArray<USkeletalMeshComponent*> FishComponents;

	UPROPERTY(EditAnywhere)
	int32 FishCount{ 10 };

	/** Boid movement */
	TArray<FBoid> Boids;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxSpeed = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float NeighborRadius = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SeparationWeight = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AlignmentWeight = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CohesionWeight = 1.0f;
	
public: 
	
	/** Returns the camera component **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }

	/** Returns the Camera Boom component **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
};
