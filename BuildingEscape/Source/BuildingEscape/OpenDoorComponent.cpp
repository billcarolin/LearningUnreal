// Fill out your copyright notice in the Description page of Project Settings.

#include "OpenDoorComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Actor.h"

// Sets default values for this component's properties
UOpenDoorComponent::UOpenDoorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UOpenDoorComponent::BeginPlay()
{
	Super::BeginPlay();
	InitialYaw = GetOwner()->GetActorRotation().Yaw;
	CurrentYaw = InitialYaw;
	//set by Editor - TargetYaw = InitialYaw + 90.f;
	TargetYaw += InitialYaw;

	if(!PressurePlate)
	{
		FString ObjectName = GetOwner()->GetName();	
		UE_LOG(LogTemp, Error, TEXT("!! The %s object has no Pressure Plate set"), *ObjectName);
	}

	//works for single player game
	ActorThatOpens = GetWorld()->GetFirstPlayerController()->GetPawn();
	
}


// Called every frame
void UOpenDoorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(PressurePlate && PressurePlate->IsOverlappingActor(ActorThatOpens))
	{
		OpenDoor(DeltaTime);
		DoorLastOpened = GetWorld()->GetTimeSeconds(); 
	}
	else
	{
		if(GetWorld()->GetTimeSeconds() - DoorLastOpened > DoorCloseDelay)
		{
			CloseDoor(DeltaTime);
		}		
	}

}

void UOpenDoorComponent::OpenDoor(float DeltaTime)
{
	//Debug Code UE_LOG(LogTemp, Warning, TEXT("...The OpenDoor Yaw is:  %f"), GetOwner()->GetActorRotation().Yaw);
	// //float MyFloat = 90.f;  //10 int implicit converts to float  10.0 BigD converts to float.  adding f, no conversion
	// //Yaw for Open Door Rotation
	FRotator CurrentRotation = GetOwner()->GetActorRotation();
	// //CurrentRotation.Yaw = MyFloat;

	//float CurrentYaw = GetOwner()->GetActorRotation().Yaw;
	FRotator DoorRotation = GetOwner()->GetActorRotation();
	/* This form of lerp uses exponential interpolation, Linear Interpolation takes into account DeltaTime 
	OpenDoor.Yaw = FMath::Lerp(CurrentYaw, TargetYaw, 0.03f);  !!Multiply alpha by DeltaTime to insure frame rate independent calculation*/
	//This uses Unreals Interp
	//Linear   FMath::FInterpConstantTo 
	//Exponential Interpolation
	DoorRotation.Yaw = FMath::FInterpTo(CurrentYaw,TargetYaw,DeltaTime,4.f);
	CurrentYaw = DoorRotation.Yaw;

	GetOwner()->SetActorRotation(DoorRotation);
}

void UOpenDoorComponent::CloseDoor(float DeltaTime)
{
	
	FRotator CurrentRotation = GetOwner()->GetActorRotation();
	FRotator DoorRotation = GetOwner()->GetActorRotation();	
	DoorRotation.Yaw = FMath::FInterpTo(CurrentYaw,InitialYaw,DeltaTime,1.f);
	CurrentYaw = DoorRotation.Yaw;
	GetOwner()->SetActorRotation(DoorRotation);
}
