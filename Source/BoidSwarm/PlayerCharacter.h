// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"


USTRUCT()
struct FBoidData
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

	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	 TObjectPtr<class UCameraComponent> m_TopDownCameraComponent{};

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> m_CameraBoom{};

	UPROPERTY(EditAnywhere, Category = "Flocking")
	TObjectPtr<USkeletalMesh> m_FishMesh;

	UPROPERTY(EditAnywhere, Category = "Flocking")
	TObjectPtr<UAnimationAsset> m_SwimAnimation{};
	
	UPROPERTY()
	TArray<TObjectPtr<USkeletalMeshComponent>> m_FishComponents{};

	UPROPERTY(EditAnywhere, Category ="Flocking")
	int32 m_FishCount{ 10 };

	UPROPERTY(EditAnywhere, Category = "Flocking")
	float m_MouseSensitivity{ 500.0f };

private:
	/** Boid movement */
	TArray<FBoidData> Boids;

	/**On Mouse input*/
	void OnMouseX(float value);
	void OnMouseY(float value);

	float m_MouseXInput{ 0.0f };
	float m_MouseYInput{ 0.0f };
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flocking")
	float m_MaxSpeed{ 200.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flocking")
	float m_NeighborRadius{ 200.0f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flocking")
	float m_SeparationWeight{ 1.5f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flocking")
	float m_AlignmentWeight{1.0f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flocking")
	float m_CohesionWeight{ 1.0f };

	
	/** Returns the camera component **/
	FORCEINLINE TObjectPtr<class UCameraComponent> GetTopDownCameraComponent() const { return m_TopDownCameraComponent; }

	/** Returns the Camera Boom component **/
	FORCEINLINE TObjectPtr<class USpringArmComponent> GetCameraBoom() const { return m_CameraBoom; }

	
};
