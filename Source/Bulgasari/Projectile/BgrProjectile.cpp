#include "BgrProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

ABgrProjectile::ABgrProjectile()
{
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(10.f);
	CollisionComp->SetCollisionProfileName(TEXT("Projectile"));
	RootComponent = CollisionComp;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->ProjectileGravityScale = 0.f; // 탑뷰 — 중력 없음

	InitialLifeSpan = 3.f;
}
