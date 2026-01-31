// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TwinStickCharacter.h"
#include "Containers/Set.h"
#include "UObject/WeakObjectPtr.h"
#include "Boid.generated.h"

UCLASS()
class BOIDSWARM_API ABoid : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABoid();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	bool isTurning = false;

	UPROPERTY(EditAnywhere, Category = "Boids")
	float speed = 100;
	UPROPERTY(EditAnywhere, Category = "Boids")
	float turn = 0.45f; //test this instead of 0,01


	UPROPERTY(EditAnywhere, Category = "Boids")
	float minspeed = 300;
	UPROPERTY(EditAnywhere, Category = "Boids")
	float maxspeed = 750;

public:
	//Player health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boids | Health")
	float maxHealth = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boids | Health")
	float health = 100.0f;

	// Stay away
	UPROPERTY(EditAnywhere, Category = "Boids")
	float SeparationStrength = 100.f;

	UPROPERTY(EditAnywhere, Category = "Boids")
	float SeparationRadius = 20.f;

	UPROPERTY(EditAnywhere, Category = "Boids")
	float SeparationDamping = 6.f;
	// Stay close
	UPROPERTY(EditAnywhere, Category = "Boids")
	float NeighbourRadius = 45.f;


	//Weighting behavior, can be used for different emotions/boid states

	//for Boids:
	UPROPERTY(EditAnywhere, Category = "Boid AI Weight")
	float SeparationWeight = 1.5f;

	UPROPERTY(EditAnywhere, Category = "Boid AI Weight")
	float AlignmentWeight = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Boid AI Weight")
	float CohesionWeight = 1.0f;

	//for target:
	UPROPERTY(EditAnywhere, Category = "Boid AI Weight")
	float OrbitWeight = 0.8f;          

	UPROPERTY(EditAnywhere, Category = "Boid AI Weight")
	float CenterPullWeight = 0.15f;    

	UPROPERTY(EditAnywhere, Category = "Boid AI Weight")
	float ReturnToCenterWeight = 3.0f;

	//Around player/target : 

	//TODO : make variables that shouldn't be stored in every Boid individualy static

	ATwinStickCharacter* spawner;
	FVector Center;
	FVector Radius;

	static TArray<TWeakObjectPtr<ABoid>> AllBoids;


	void UpdateTextureColor(const FLinearColor& NewColor);

protected:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterial;

private:
	FVector GetSeparation(const TArray<ABoid*>& Neighbours) const;
	FVector GetAlignment(const TArray<ABoid*>& Neighbours) const;
	FVector GetCohesion(const TArray<ABoid*>& Neighbours) const;

	void GetNeighbours(TArray<ABoid*>& OutNeighbours) const;
	void updateHealth(float num);


	TSet<TWeakObjectPtr<class ABoid>> OverlappingBoids;

	FVector SeparationForce = FVector::ZeroVector;

	// Addition to the individual swarming
	FVector PersonalOffset = FVector::ZeroVector; // Offset around the target
	float OrbitSign = 1.f; // With or Against the clock.                      
	float OrbitPhase = 0.f; // Desync

protected:

	// Overlap event handler for CapsuleComponent - Unreal Engine assistant
	UFUNCTION()
	void OnCapsuleBeginOverlap(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnCapsuleEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};
