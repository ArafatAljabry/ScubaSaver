// Copyright Epic Games, Inc. All Rights Reserved.


#include "TwinStickCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "TwinStickGameMode.h"
#include "TwinStickAoEAttack.h"
#include "Kismet/KismetMathLibrary.h"
#include "TwinStickProjectile.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Boid.h"
#include "Kismet/KismetMathLibrary.h"
#include "Blueprint/UserWidget.h"
#include "Components/AudioComponent.h"
#include "GameFramework/PlayerController.h"
#include "Components/PrimitiveComponent.h" // For the HitResult's component
#include "Engine/EngineTypes.h" // Contains FHitResult definition
#include "Kismet/GameplayStatics.h"

ATwinStickCharacter::ATwinStickCharacter()
{
 	PrimaryActorTick.bCanEverTick = true;

	// create the spring arm
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(RootComponent);

	SpringArm->SetRelativeRotation(FRotator(-50.0f, 0.0f, 0.0f));

	SpringArm->TargetArmLength = 2200.0f;
	SpringArm->bDoCollisionTest = false;
	SpringArm->bInheritYaw = false;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 0.5f;

	// create the camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	Camera->SetFieldOfView(75.0f);

	// configure the character movement
	GetCharacterMovement()->GravityScale = 1.5f;
	GetCharacterMovement()->MaxAcceleration = 1000.0f;
	GetCharacterMovement()->BrakingFrictionFactor = 1.0f;
	GetCharacterMovement()->bCanWalkOffLedges = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 640.0f, 0.0f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	bIsLeftMouseDown = false;

	/*Pause menu widget class*/
	static ConstructorHelpers::FClassFinder<UUserWidget> PauseMenuBPClass(TEXT("/Game/UI/WBP_PauseMenu"));
	if (PauseMenuBPClass.Class)
	{
		PauseMenuWidgetClass = PauseMenuBPClass.Class;
	}
}

/*
void ATwinStickCharacter::OnLeftMousePressed()
{
	bIsLeftMouseDown = true;
}

void ATwinStickCharacter::OnLeftMouseReleased()
{
	bIsLeftMouseDown = false;
}
*/

void ATwinStickCharacter::OnRightMousePressed() {
	bIsRightMouseDown = true;
};
void ATwinStickCharacter::OnRightMouseReleased()
{
	bIsRightMouseDown = false;
	SwarmCenter = spawnOrigin; // fallback
	for (ABoid* B : Boids) if (B) B->Center = spawnOrigin; //check and/or rewrite
}


float ATwinStickCharacter::RandomPitch() { float p = FMath::FRandRange(0.f, 360.f); return p; };
float ATwinStickCharacter::RandomYaw() { float y = FMath::FRandRange(0.f, 360.f); return y; };
float ATwinStickCharacter::RandomRoll() { float r = FMath::FRandRange(0.f, 360.f); return r; };

void ATwinStickCharacter::SpawnBoids(int Count)
{
	if (!BoidClass)
	{
		UE_LOG(LogTemp, Error, TEXT("BoidClass is NOT set on %s"), *GetName());
		return;
	}
<<<<<<< Updated upstream
=======
	
	const int32 AvailableSlots = maxFishNumber - Boids.Num();
	/*GEngine->AddOnScreenDebugMessage(
		-1,
		2.0f,
		FColor::Yellow,
		FString::Printf(TEXT("Available: %d"), AvailableSlots)
	);*/
	if (AvailableSlots <= 0)
	{
		return; // already at cap
	}
>>>>>>> Stashed changes

	UWorld* World = GetWorld();
	if (!World) return;

	spawnOrigin = GetActorLocation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;
	SpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	Boids.Reserve(Boids.Num() + Count);

	for (int32 i = 0; i < Count; ++i)
	{
		const FVector SpawnLocation =
			spawnOrigin + FVector(
				FMath::FRandRange(-radius, radius),
				FMath::FRandRange(-radius, radius),
				FMath::FRandRange(-radius/2, radius/2)
			);

		const FRotator SpawnRot(RandomPitch(),RandomYaw(), RandomRoll());

		ABoid* EachBoid = World->SpawnActor<ABoid>(BoidClass, SpawnLocation, SpawnRot, SpawnParams);
		if (!EachBoid)
		{/*
			UE_LOG(LogTemp, Warning, TEXT("Failed to spawn boid %d"), i);*/
			continue;
		}

		EachBoid->spawner = this;
		EachBoid->Center = spawnOrigin;

		Boids.Add(EachBoid);

		// Debug:
		// DrawDebugSphere(World, SpawnLocation, 25.f, 12, FColor::Green, false, 2.f);
	}
	/*
	UE_LOG(LogTemp, Warning, TEXT("Spawned %d boids. Total: %d"), Count, Boids.Num());*/
}



void ATwinStickCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// update the items count
	UpdateItems();

	//GetWorld()->GetFirstPlayerController()->GetActorLocation();

	spawnOrigin = GetActorLocation();

	//GetActorBounds(false, spawnOrigin, flyLimit);
	
	SpawnBoids(25);

	// I am desperate
	FInputModeGameAndUI Mode;
	Mode.SetHideCursorDuringCapture(false);

	////Suggested by GPT, check if needed v
	//
	//FActorSpawnParameters SpawnParams;
	//SpawnParams.Owner = this;
	//SpawnParams.SpawnCollisionHandlingOverride =
	//	ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	//
	//Boids.Reserve(25);
	//
	//
	////Suggested by GPT, check if needed ^
	//
	//for (int i = 0; i < 25; i++) {
	//	FVector spawnLocation =
	//		spawnOrigin + FVector(
	//			FMath::RandRange(-radius, radius/*-flyLimit.X, flyLimit.X*/),
	//			FMath::RandRange(-radius, radius/*-flyLimit.Y, flyLimit.Y*/),
	//			FMath::RandRange(-radius / 2, radius / 2/*-flyLimit.Z, flyLimit.Z */)
	//
	//		);
	//	//GetWorld()->SpawnActor(boid, &spawnLocation);
	//	/*
	//	ABoid* eachBoid = (ABoid*)GetWorld()->SpawnActor(
	//		boid,
	//		&spawnLocation
	//	);
	//
	//	eachBoid->spawner = this;
	//	}
	//	*/
	//
	//	//Suggested by GPT v
	//
	//	ABoid* EachBoid = GetWorld()->SpawnActor<ABoid>(
	//		BoidClass,
	//		spawnLocation,
	//		FRotator(RandomPitch(), RandomYaw(), RandomRoll()),
	//		SpawnParams
	//	);
	//	if (!BoidClass)
	//	{
	//		UE_LOG(LogTemp, Error, TEXT("BoidClass is NOT set on %s"), *GetName());
	//		return;
	//	}
	//
	//
	//	if (!EachBoid) continue;
	//
	//	EachBoid->spawner = this;
	//	EachBoid->Center = spawnOrigin; 
	//	Boids.Add(EachBoid);
	//
	//	//Suggested by GPT ^
	//

	//Background audio
	if (BackgroundMusic)
	{
		//Create the component
		MusicComponent = UGameplayStatics::SpawnSound2D(
														this,
														BackgroundMusic,
														1.0f,      // Volume
														1.0f,      // Pitch
														0.0f,      // Start time
														nullptr,   // Concurrency
														true,      // Persist
														false      // Don't auto destroy
														);
		if (MusicComponent)
			MusicComponent->Play();
	}

}

void ATwinStickCharacter::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	/** Clear the autofire timer */
	GetWorld()->GetTimerManager().ClearTimer(AutoFireTimer);
}

void ATwinStickCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// set the player controller reference
	PlayerController = Cast<APlayerController>(GetController());
}

void ATwinStickCharacter::Pause()
{
	if (isPaused)
		return;
	ShowPauseMenu();
	
}

void ATwinStickCharacter::ShowPauseMenu()
{
	// Create widget if it doesn't exist
	if (!PauseMenuWidget && PauseMenuWidgetClass)
	{
		
		if (PlayerController)
		{
			PauseMenuWidget = CreateWidget<UUserWidget>(PlayerController, PauseMenuWidgetClass);

		}
	}

	// Only proceed if widget exists and is not already in viewport
	if (PauseMenuWidget && !PauseMenuWidget->IsInViewport())
	{

		// Add to viewport ONCE
		PauseMenuWidget->AddToViewport();

		// Set input mode to UI only (or GameAndUI if you want game input too)
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(PauseMenuWidget->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		
		if (PlayerController)
		{
			UE_LOG(LogTemp, Warning, TEXT("xxxthree"));

			PlayerController->SetInputMode(InputMode);
			PlayerController->SetShowMouseCursor(true);

		}

		// Pause the game
		UGameplayStatics::SetGamePaused(GetWorld(), true);

		//Audio 
		if (PauseMenuMusic)
		{
			//Create the component
			PauseMenuMusicComponent = UGameplayStatics::SpawnSound2D(
				this,
				PauseMenuMusic,
				1.0f,      // Volume
				1.0f,      // Pitch
				0.0f,      // Start time
				nullptr,   // Concurrency
				true,      // Persist
				false      // Don't auto destroy
			);
			if (PauseMenuMusicComponent)
				MusicComponent->SetPaused(true);
				PauseMenuMusicComponent->Play();
		}




		isPaused = true;
	}
}



void ATwinStickCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

<<<<<<< Updated upstream
=======
	/*
	GEngine->AddOnScreenDebugMessage(
		-1,
		2.0f,
		FColor::Yellow,
		FString::Printf(TEXT("Doids size: %d"), Boids.Num())
	);*/
>>>>>>> Stashed changes
	// get the current rotation
	const FRotator OldRotation = GetActorRotation();

	// are we aiming with the mouse?
	if (bUsingMouse)
	{
		if (PlayerController)
		{
			// get the cursor world location
			FHitResult OutHit; 
			PlayerController->GetHitResultUnderCursorByChannel(MouseAimTraceChannel, true, OutHit);

			APlayerController* PC = Cast<APlayerController>(GetController());
			if (!PC) return;

			const FVector CursorPoint = OutHit.ImpactPoint;

			SetActorLocation(FVector(CursorPoint.X, CursorPoint.Y,GetActorLocation().Z));

			for (ABoid* BoidActor : Boids)
			{
				if (!IsValid(BoidActor)) continue;
				BoidActor->Center = CursorPoint;
			}
			

			/* We might need to edit this */
			// find the aim rotation 
			const FRotator AimRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), OutHit.Location);

			// save the aim angle
			AimAngle = AimRot.Yaw;

			

			// update the yaw, reuse the pitch and roll
			SetActorRotation(FRotator(OldRotation.Pitch, AimAngle, OldRotation.Roll));

		}

	} else {

		// use quaternion interpolation to blend between our current rotation
		// and the desired aim rotation using the shortest path
		const FRotator TargetRot = FRotator(OldRotation.Pitch, AimAngle, OldRotation.Roll);

		SetActorRotation(TargetRot);
	}
}

void ATwinStickCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// set up the enhanced input action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{

		EnhancedInputComponent->BindAction(StickAimAction, ETriggerEvent::Triggered, this, &ATwinStickCharacter::StickAim);
		EnhancedInputComponent->BindAction(MouseAimAction, ETriggerEvent::Triggered, this, &ATwinStickCharacter::MouseAim);
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Triggered, this, &ATwinStickCharacter::Dash);
		EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Triggered, this, &ATwinStickCharacter::Pause);

		//EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Triggered, this, &ATwinStickCharacter::Shoot);
		EnhancedInputComponent->BindAction(AoEAction, ETriggerEvent::Triggered, this, &ATwinStickCharacter::AoEAttack);
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Started, this, &ATwinStickCharacter::OnLeftMousePressed);
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Completed, this, &ATwinStickCharacter::OnLeftMouseReleased);

	}

}

void ATwinStickCharacter::StickAim(const FInputActionValue& Value)
{
	// get the input vector
	FVector2D InputVector = Value.Get<FVector2D>();

	// route the input
	DoAim(InputVector.X, InputVector.Y);
}

void ATwinStickCharacter::MouseAim(const FInputActionValue& Value)
{
	// raise the mouse controls flag
	bUsingMouse = true;

	// show the mouse cursor
	if (PlayerController)
	{
		PlayerController->SetShowMouseCursor(true);
	}
}

void ATwinStickCharacter::Dash(const FInputActionValue& Value)
{
	// route the input
	DoDash();
}

void ATwinStickCharacter::Shoot(const FInputActionValue& Value)
{
	// route the input
	DoShoot();
}

void ATwinStickCharacter::AoEAttack(const FInputActionValue& Value)
{
	// route the input
	DoAoEAttack();
}

void ATwinStickCharacter::DoMove(float AxisX, float AxisY)
{
	// save the input
	LastMoveInput.X = AxisX;
	LastMoveInput.Y = AxisY;

	// calculate the forward component of the input
	FRotator FlatRot = GetControlRotation();
	FlatRot.Pitch = 0.0f;

	// apply the forward input
	AddMovementInput(FlatRot.RotateVector(FVector::ForwardVector), AxisX);

	// apply the right input
	AddMovementInput(FlatRot.RotateVector(FVector::RightVector), AxisY);
}

void ATwinStickCharacter::DoAim(float AxisX, float AxisY)
{
	// calculate the aim angle from the inputs
	AimAngle = FMath::RadiansToDegrees(FMath::Atan2(AxisY, -AxisX));

	// lower the mouse controls flag
	bUsingMouse = false;

	// hide the mouse cursor
	if (PlayerController)
	{
		PlayerController->SetShowMouseCursor(true);
	}

	// are we on autofire cooldown?
	if (!bAutoFireActive)
	{
		// set ourselves on cooldown
		bAutoFireActive = true;

		// fire a projectile
		DoShoot();

		// schedule autofire cooldown reset
		GetWorld()->GetTimerManager().SetTimer(AutoFireTimer, this, &ATwinStickCharacter::ResetAutoFire, AutoFireDelay, false);
	}
}

void ATwinStickCharacter::DoDash()
{
	// calculate the launch impulse vector based on the last move input
	FVector LaunchDir = FVector::ZeroVector;

	LaunchDir.X = FMath::Clamp(LastMoveInput.X, -1.0f, 1.0f);
	LaunchDir.Y = FMath::Clamp(LastMoveInput.Y, -1.0f, 1.0f);

	// launch the character in the chosen direction
	LaunchCharacter(LaunchDir * DashImpulse, true, true);
}

void ATwinStickCharacter::OnLeftMousePressed() {
	FHitResult HitResult;

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;

	if (PC->GetHitResultUnderCursor(ECC_Visibility, false, HitResult))
	{
		//FVector ClickLocation = HitResult.Location;

		FHitResult Hit;
		const FVector ClickLocation = Hit.ImpactPoint;
		UE_LOG(LogTemp, Warning, TEXT("Mouse clicked at world location: %s"), *ClickLocation.ToString());

		Boids.RemoveAll([](ABoid* B) { return !IsValid(B); });

		for (ABoid* BoidActor : Boids)
		{
			if (!BoidActor) continue;
			BoidActor->Center = ClickLocation;
			//Make Boid circle around where the mouse is held down.
		}
	}
}

void ATwinStickCharacter::OnLeftMouseReleased() {

	Boids.RemoveAll([](ABoid* B) { return !IsValid(B); });
	
	for (ABoid* BoidActor : Boids)
	{
		if (!BoidActor) continue;
		BoidActor->Center = spawnOrigin;
			//Make Boid circle around player center.
	}
}

void ATwinStickCharacter::DoShoot()
{
		FHitResult HitResult;

		APlayerController* PC = Cast<APlayerController>(GetController());


		if (PC->GetHitResultUnderCursor(ECC_Visibility, false, HitResult))
		{
			//FVector ClickLocation = HitResult.Location;

			FHitResult Hit;
			const FVector ClickLocation = Hit.ImpactPoint;
			UE_LOG(LogTemp, Warning, TEXT("Mouse clicked at world location: %s"), *ClickLocation.ToString());

			for (ABoid* BoidActor : Boids)
			{
			if (bIsLeftMouseDown)
			{
				BoidActor->Center = ClickLocation;
				//Make Boid circle around where the mouse is held down.
			}
			else {
				BoidActor->Center = spawnOrigin;
				//Make Boid circle around player center.
			}
			}
		}

}

void ATwinStickCharacter::DoAoEAttack()
{
	// do we have enough items to do an AoE attack?
	if (Items > 0)
	{
		// get the game time
		const float GameTime = GetWorld()->GetTimeSeconds();

		// are we off AoE cooldown?
		if (GameTime - LastAoETime > AoECooldownTime)
		{
			// save the new AoE time
			LastAoETime = GameTime;
			/*
			// spawn the AoE
			ATwinStickAoEAttack* AoE = GetWorld()->SpawnActor<ATwinStickAoEAttack>(AoEAttackClass, GetActorTransform());
			*/
			//Sike, we're spawning Boids instead
			SpawnBoids(BoidsToSpawnPerClick);


			// decrease the number of items
			--Items;

			// update the items count
			UpdateItems();
		}
	}
}

void ATwinStickCharacter::HandleDamage(float Damage, const FVector& DamageDirection)
{
	// calculate the knockback vector
	FVector LaunchVector = DamageDirection;
	LaunchVector.Z = 0.0f;

	// apply knockback to the character
	LaunchCharacter(LaunchVector * KnockbackStrength, true, true);

	// pass control to BP
	BP_Damaged();
}

void ATwinStickCharacter::AddPickup()
{
	// increase the item count
	++Items;

	// update the items counter
	UpdateItems();
}

void ATwinStickCharacter::UpdateItems()
{
	// update the game mode
	if (ATwinStickGameMode* GM = Cast<ATwinStickGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GM->ItemUsed(Items);
	}
}

void ATwinStickCharacter::ResetAutoFire()
{
	// reset the autofire flag
	bAutoFireActive = false;
}

