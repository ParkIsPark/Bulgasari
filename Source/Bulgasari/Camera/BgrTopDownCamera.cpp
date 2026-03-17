#include "BgrTopDownCamera.h"

ATopDownCamera::ATopDownCamera()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATopDownCamera::BeginPlay()
{
	Super::BeginPlay();
	SetActorRotation(FRotator(CameraPitch, 0.f, 0.f));
}

void ATopDownCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (TargetActor)
	{
		SetActorLocation(TargetActor->GetActorLocation() + CameraOffset);
	}
}
