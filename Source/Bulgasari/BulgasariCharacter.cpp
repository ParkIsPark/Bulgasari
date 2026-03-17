// Copyright Epic Games, Inc. All Rights Reserved.

#include "BulgasariCharacter.h"
#include "Camera/BgrTopDownCamera.h"
#include "Projectile/BgrProjectile.h"
#include "Engine/LocalPlayer.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ABulgasariCharacter

ABulgasariCharacter::ABulgasariCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->GravityScale = 0.f;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->MaxFlySpeed = 500.f;
	GetCharacterMovement()->MaxAcceleration = 10000.f;    // 기본 2048, 높을수록 즉시 가속
	GetCharacterMovement()->BrakingDecelerationFlying = 10000.f; // 높을수록 즉시 정지

	// 비주얼 메시 (Blueprint에서 Static Mesh 지정)
	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(RootComponent);

	// 기본 SkeletalMesh 숨김
	GetMesh()->SetVisibility(false);
}

void ABulgasariCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 중력 없이 공중에 부유
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);

	// 초기 마우스 보간 위치를 캐릭터 전방으로 설정
	PrevMouseVector = GetActorLocation() + GetActorForwardVector() * 100.f;

	// 탑뷰 카메라 스폰 후 뷰 타겟으로 설정
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	TopDownCamera = GetWorld()->SpawnActor<ATopDownCamera>(ATopDownCamera::StaticClass(), GetActorLocation(), FRotator::ZeroRotator, SpawnParams);
	if (TopDownCamera)
	{
		TopDownCamera->TargetActor = this;

		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			PC->SetViewTarget(TopDownCamera);
			PC->bShowMouseCursor = true; // 커서 설정0
		}
	}
	
	
	
}

//////////////////////////////////////////////////////////////////////////
// Input

void ABulgasariCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABulgasariCharacter::Move);

		// Shooting
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Started, this, &ABulgasariCharacter::Shoot);

		// Dashing
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &ABulgasariCharacter::Dash);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ABulgasariCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 마우스 커서 방향으로 캐릭터 회전
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;

	FHitResult HitResult;
	PC->GetHitResultUnderCursor(ECC_Visibility, false, HitResult);
	if (!HitResult.bBlockingHit) return;

	// 이전 위치에서 현재 마우스 위치로 보간 → 부드러운 회전
	PrevMouseVector = FMath::VInterpTo(PrevMouseVector, HitResult.Location, DeltaTime, MouseRotationInterpSpeed);

	FVector Direction = PrevMouseVector - GetActorLocation();
	Direction.Z = 0.f;

	if (!Direction.IsNearlyZero())
	{
		SetActorRotation(Direction.Rotation());
	}

	// 쿨다운 감소
	if (DashCooldownRemaining > 0.f)
	{
		DashCooldownRemaining -= DeltaTime;
	}

	// 대쉬 이동 — Cubic Ease-Out: 시작 폭발적, 끝에서 쫙 감속
	if (bIsDashing)
	{
		DashElapsed += DeltaTime;
		const float Alpha = FMath::Clamp(DashElapsed / DashDuration, 0.f, 1.f);

		// 1 - (1 - t)^3 : 초반 빠르게 치고 나가다 끝에서 부드럽게 제동
		const float EasedAlpha = 1.f - FMath::Pow(1.f - Alpha, 3.f);

		SetActorLocation(FMath::Lerp(DashStartLocation, DashTargetLocation, EasedAlpha), true);

		if (Alpha >= 1.f)
		{
			bIsDashing = false;
			DashElapsed = 0.f;
			DashCooldownRemaining = DashCooldown;
		}
	}
}

void ABulgasariCharacter::Dash(const FInputActionValue& Value)
{
	if (bIsDashing || DashCooldownRemaining > 0.f) return;

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC) return;

	FHitResult CursorHit;
	PC->GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if (!CursorHit.bBlockingHit) return;

	// 클릭 위치를 캐릭터와 같은 Z로 맞춤
	FVector ToTarget = CursorHit.Location - GetActorLocation();
	ToTarget.Z = 0.f;

	
	ToTarget = ToTarget.GetSafeNormal() * MaxDashDistance;
	
	

	FVector RawTarget = GetActorLocation() + ToTarget;

	// 캡슐 스윕으로 막힌 지점 감지
	FHitResult SweepHit;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	FCollisionShape CapsuleShape = FCollisionShape::MakeCapsule(
		GetCapsuleComponent()->GetScaledCapsuleRadius(),
		GetCapsuleComponent()->GetScaledCapsuleHalfHeight()
	);
	bool bBlocked = GetWorld()->SweepSingleByChannel(
		SweepHit,
		GetActorLocation(),
		RawTarget,
		FQuat::Identity,
		ECC_Pawn,
		CapsuleShape,
		QueryParams  
	);

	DashStartLocation = GetActorLocation();
	DashTargetLocation = bBlocked ? SweepHit.Location : RawTarget;
	DashElapsed = 0.f;
	bIsDashing = true;
}

void ABulgasariCharacter::Shoot(const FInputActionValue& Value)
{
	if (!ProjectileClass) return;

	FVector Direction = GetActorForwardVector();
	FVector SpawnLocation = GetActorLocation() + Direction * 60.f;
	FRotator SpawnRotation = Direction.Rotation();

	GetWorld()->SpawnActor<ABgrProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
}

void ABulgasariCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		
		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

