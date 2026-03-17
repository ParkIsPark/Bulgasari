// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "BulgasariCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class UStaticMeshComponent;
class ATopDownCamera; // Camera/BgrTopDownCamera.h
class ABgrProjectile; // Projectile/BgrProjectile.h
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ABulgasariCharacter : public ACharacter
{
	GENERATED_BODY()

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Shoot Input Action (좌클릭) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ShootAction;

	/** Dash Input Action (우클릭) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DashAction;

	/** 발사할 투사체 클래스 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ABgrProjectile> ProjectileClass;

	/** 비주얼 메시 (Blueprint에서 교체 가능) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> BodyMesh;

	/** 스폰된 탑뷰 카메라 레퍼런스 */
	UPROPERTY()
	TObjectPtr<ATopDownCamera> TopDownCamera;

	/** 이전 프레임의 보간된 마우스 월드 위치 */
	FVector PrevMouseVector;

	/** 마우스 회전 보간 속도 (높을수록 빠르게 반응) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float MouseRotationInterpSpeed = 10.f;

	/** 대쉬 최대 사거리 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float MaxDashDistance = 600.f;

	/** 대쉬 총 소요 시간 (초) — 짧을수록 폭발적인 느낌 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float DashDuration = 0.18f;

	/** 대쉬 쿨다운 (초) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float DashCooldown = 0.6f;

	bool bIsDashing = false;
	FVector DashStartLocation;
	FVector DashTargetLocation;
	float DashElapsed = 0.f;
	float DashCooldownRemaining = 0.f;

public:
	ABulgasariCharacter();

protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for shoot input (좌클릭) */
	void Shoot(const FInputActionValue& Value);

	/** Called for dash input (우클릭) */
	void Dash(const FInputActionValue& Value);

	virtual void Tick(float DeltaTime) override;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void BeginPlay() override;
};

