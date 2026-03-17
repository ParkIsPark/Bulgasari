#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "BgrTopDownCamera.generated.h"

UCLASS()
class BULGASARI_API ATopDownCamera : public ACameraActor
{
	GENERATED_BODY()

public:
	ATopDownCamera();

	virtual void Tick(float DeltaTime) override;

	/** 따라갈 대상 액터 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TopDown Camera")
	TObjectPtr<AActor> TargetActor;

	/** 대상 위치로부터의 카메라 오프셋 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TopDown Camera")
	FVector CameraOffset = FVector(-2753.0, 0.0f, 3705.f);

	/** 카메라 고정 Pitch (음수 = 아래 방향) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TopDown Camera")
	float CameraPitch = -51.f;

protected:
	virtual void BeginPlay() override;
};
