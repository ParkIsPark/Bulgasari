#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BgrEnemyBase.generated.h"

class UStaticMeshComponent;
class ABgrProjectile;

UCLASS(Abstract)
class BULGASARI_API ABgrEnemyBase : public ACharacter
{
	GENERATED_BODY()

public:
	ABgrEnemyBase();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// ── 비주얼 ──────────────────────────────────────────────────────────────
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	TObjectPtr<UStaticMeshComponent> BodyMesh;

	// ── 전투 ────────────────────────────────────────────────────────────────
	/** 발사할 투사체 클래스 (Blueprint에서 지정) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	TSubclassOf<ABgrProjectile> ProjectileClass;

	/** 타겟을 향해 투사체 발사 */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void Shoot(AActor* Target);

	// ── 스탯 ────────────────────────────────────────────────────────────────
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stats")
	float MaxHP = 100.f;

	UPROPERTY(BlueprintReadOnly, Category = "Stats")
	float CurrentHP;

	/** HP가 0 이하가 됐을 때 호출. Blueprint에서 사망 연출 구현 */
	UFUNCTION(BlueprintNativeEvent, Category = "Stats")
	void Die();
	virtual void Die_Implementation();

	// ── 호버링 ──────────────────────────────────────────────────────────────
	/** Sine 파형 호버링 진폭 (cm) */
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float HoverAmplitude = 15.f;

	/** Sine 파형 호버링 주기 속도 */
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float HoverSpeed = 2.f;

	/** 이동 속도 (Blueprint에서 적 종류별로 조정) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float MoveSpeed = 300.f;

private:
	float HoverBaseZ = 0.f;
	float HoverTime  = 0.f;
};
