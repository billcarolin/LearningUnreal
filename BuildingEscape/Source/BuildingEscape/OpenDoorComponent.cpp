// Fill out your copyright notice in the Description page of Project Settings.

#include "OpenDoorComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Actor.h"

#define OUT

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

	//works for single player game
	ActorThatOpens = GetWorld()->GetFirstPlayerController()->GetPawn();

	FindPressurePlateComponent();
	FindAudioComponent();
	
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
	if(AudioComponent)
	{
		if(!IsDoorOpen)
		{
			AudioComponent->Play();
			IsDoorOpen = true;
		}
		
	}	
}

void UOpenDoorComponent::CloseDoor(float DeltaTime)
{
	
	FRotator CurrentRotation = GetOwner()->GetActorRotation();
	FRotator DoorRotation = GetOwner()->GetActorRotation();	
	DoorRotation.Yaw = FMath::FInterpTo(CurrentYaw,InitialYaw,DeltaTime,1.f);
	CurrentYaw = DoorRotation.Yaw;
	GetOwner()->SetActorRotation(DoorRotation);
	if(AudioComponent)
	{
		if(IsDoorOpen)
		{
			AudioComponent->Play();
			IsDoorOpen = false;
		}		
	}
}

float UOpenDoorComponent::TotalMassOfActors() const
{
	float TotalMass = 0.f;

	// Find All Overlapping Actors.
	TArray<AActor*> OverlappingActors;
	if(!PressurePlate) {return TotalMass;} //null pointer protection
	PressurePlate->GetOverlappingActors(OUT OverlappingActors);

	// Add Up Their Masses.

	for(AActor* Actor : OverlappingActors)
	{
		TotalMass += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
		UE_LOG(LogTemp, Warning, TEXT("%s is on the pressureplate!"), *Actor->GetName());
	}

	return TotalMass;
}

void UOpenDoorComponent::FindPressurePlateComponent()
{
	if(!PressurePlate)
	{
		FString ObjectName = GetOwner()->GetName();	
		UE_LOG(LogTemp, Error, TEXT("!! The %s object has no Pressure Plate set"), *ObjectName);
	}
}

void UOpenDoorComponent::FindAudioComponent()
{	
	AudioComponent = GetOwner()->FindComponentByClass<UAudioComponent>();
	if(!AudioComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("Audio Component Missing from %s"), *GetOwner()->GetName());
	}
}
