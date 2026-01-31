// Fill out your copyright notice in the Description page of Project Settings.


#include "MouseClick.h"
#include "TwinStickPlayerController.h"

// Sets default values
AMouseClick::AMouseClick()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMouseClick::BeginPlay()
{
	Super::BeginPlay();
	BindToInput();
	GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);
	
}

// Called every frame
void AMouseClick::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMouseClick::BindToInput()
{
	InputComponent = NewObject<UInputComponent>(this);
	InputComponent->RegisterComponent();
	if (InputComponent) {
		FInputActionBinding& NewBinding = InputComponent->BindAction("Left_Mouse_Click", IE_Pressed, this, &AMouseClick::LeftMouseClickFunction);
		NewBinding.bConsumeInput = false;
	}
	if (InputComponent) {
		FInputActionBinding& NewBinding = InputComponent->BindAction("Right_Mouse_Click", IE_Pressed, this, &AMouseClick::RightMouseClickFunction);
		NewBinding.bConsumeInput = false;
	}
}

void AMouseClick::LeftMouseClickFunction()
{
	ATwinStickPlayerController* controller = Cast<ATwinStickPlayerController>(GetWorld()->GetFirstPlayerController());
	controller->GetMousePosition(mouseX, mouseY);
}

void AMouseClick::RightMouseClickFunction()
{
	ATwinStickPlayerController* controller = Cast<ATwinStickPlayerController>(GetWorld()->GetFirstPlayerController());
	controller->GetMousePosition(mouseX, mouseY);
}

