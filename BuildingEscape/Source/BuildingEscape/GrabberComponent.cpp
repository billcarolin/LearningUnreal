// Fill out your copyright notice in the Description page of Project Settings.

#include "GrabberComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"

#define OUT

// Sets default values for this component's properties
UGrabberComponent::UGrabberComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UGrabberComponent::BeginPlay()
{
	Super::BeginPlay();

	//UE_LOG(LogTemp, Warning, TEXT("....Grabber Is really loaded!!!!!"));
	CheckDependenciesAreLoaded();
	SetupInputComponent();
}

void UGrabberComponent::CheckDependenciesAreLoaded()
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if(!PhysicsHandle)
	{
		UE_LOG(LogTemp, Error, TEXT("Grabber is missing UPhysicsHandleComponent on %s!!!"), *GetOwner()->GetName());
	}
}

void UGrabberComponent::SetupInputComponent()
{
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if(InputComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("InputComponent Found. %s is good to go!!"), *GetOwner()->GetName());
		//Uses Function Pointer to send events
		InputComponent->BindAction("GrabAction", IE_Pressed, this, &UGrabberComponent::Grab);
		InputComponent->BindAction("GrabAction", IE_Released, this, &UGrabberComponent::Release);
	}
	//Debug code
	// else
	// {
	// 	UE_LOG(LogTemp, Error, TEXT("InputComponent MISSING on %s!!"), *GetOwner()->GetName());
	// }

}


void UGrabberComponent::Grab()
{
	UE_LOG(LogTemp, Warning, TEXT("GrabAction, IE_Pressed event Received!!"));
	// Get the player's viewpoint
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation, 
		OUT PlayerViewPointRotation
	);

	// UE_LOG(LogTemp, Warning, TEXT("Player Viewpoint... Location=%s  Rotation=%s"),
	// 	 *PlayerViewPointLocation.ToString(),
	// 	 *PlayerViewPointRotation.ToString()
	// );	
	FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;

	RayCastDebug(PlayerViewPointLocation, LineTraceEnd);	

	//Ray-Cast out
	FHitResult HitResult;
	//1. TagName = no, 2.  Complex Collision?  = false   3.  Actor to ignore = us
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());
	GetWorld()->LineTraceSingleByObjectType(
		OUT HitResult,
		PlayerViewPointLocation,
		LineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParams
	);

	//See what it hits..
	AActor* HitMan = HitResult.GetActor();
	UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent();

	if(HitMan)
	{
		FString WhoWasHit = HitMan->GetName();
		UE_LOG(LogTemp, Warning, TEXT("Player Hit Actor name=%s"), *WhoWasHit);

		/////////if hit something, grab attach the Physics Handle
		PhysicsHandle->GrabComponentAtLocation
		(
			ComponentToGrab,
			NAME_None,
			LineTraceEnd
		);
	}

}

void UGrabberComponent::Release()
{
	UE_LOG(LogTemp, Warning, TEXT("GrabAction, IE_Released event Received!!!"));
	PhysicsHandle->ReleaseComponent();
}


// Called every frame
void UGrabberComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Get the player's viewpoint
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation, 
		OUT PlayerViewPointRotation
	);
	FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;

	//Set the location of the grabbed component to end of Line Trace (show "picked up")
	if(PhysicsHandle->GrabbedComponent)
	{
		PhysicsHandle->SetTargetLocation(LineTraceEnd);
	}
}


//Debug Routine
void UGrabberComponent::RayCastDebug(FVector PlayerViewPointLocation, FVector LineTraceEnd)
{
	DrawDebugLine(
		GetWorld(),
		PlayerViewPointLocation,
		LineTraceEnd,
		FColor(0,255,0), //Green Line
		false,
		0.f,
		0,
		5.f
	);
}

