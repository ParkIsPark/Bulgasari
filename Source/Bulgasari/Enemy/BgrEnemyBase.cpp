#include "BgrEnemyBase.h"
#include "BgrEnemyAIController.h"
#include "Bulgasari/Projectile/BgrProjectile.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ABgrEnemyBase::ABgrEnemyBase()
{
	PrimaryActorTick.bCanEverTick = true;

	// ── AI Controller 연결 ───────────────────────────────────────────────────
	AIControllerClass    = ABgrEnemyAIController::StaticClass();
	AutoPossessAI        = EAutoPossessAI::PlacedInWorldOrSpawned;

	// ── 이동: 플레이어(BulgasariCharacter)와 동일하게 Flying 모드 사용 ────────
	GetCharacterMovement()->GravityScale              = 0.f;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate              = FRotator(0.f, 360.f, 0.f);
	GetCharacterMovement()->MaxAcceleration           = 2048.f;
	GetCharacterMovement()->BrakingDecelerationFlying = 2048.f;

	// 기본 SkeletalMesh 숨김 (플레이어와 동일 구조)
	GetMesh()->SetVisibility(false);

	// ── 비주얼 메시 ──────────────────────────────────────────────────────────
	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(RootComponent);
}

void ABgrEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	GetCharacterMovement()->MaxFlySpeed = MoveSpeed;

	CurrentHP  = MaxHP;
	HoverBaseZ = GetActorLocation().Z;
}

void ABgrEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ── Sine 파형 호버링 ─────────────────────────────────────────────────────
	HoverTime += DeltaTime;
	FVector Loc = GetActorLocation();
	Loc.Z = HoverBaseZ + FMath::Sin(HoverTime * HoverSpeed) * HoverAmplitude;
	SetActorLocation(Loc);
}

void ABgrEnemyBase::Shoot(AActor* Target)
{
	if (!ProjectileClass || !Target) return;

	const FVector Direction     = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	const FVector SpawnLocation = GetActorLocation() + Direction * 60.f;
	const FRotator SpawnRotation = Direction.Rotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner      = this;
	SpawnParams.Instigator = this;

	GetWorld()->SpawnActor<ABgrProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);
}

void ABgrEnemyBase::Die_Implementation()
{
	Destroy();
}
