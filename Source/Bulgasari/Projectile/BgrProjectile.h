#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BgrProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class BULGASARI_API ABgrProjectile : public AActor
{
	GENERATED_BODY()

public:
	ABgrProjectile();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Projectile")
	TObjectPtr<USphereComponent> CollisionComp;

	UPROPERTY(VisibleAnywhere, Category = "Projectile")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;
};
