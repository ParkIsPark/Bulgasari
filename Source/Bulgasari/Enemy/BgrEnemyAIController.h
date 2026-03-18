#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BgrEnemyAIController.generated.h"

UCLASS()
class BULGASARI_API ABgrEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	ABgrEnemyAIController();

protected:
	virtual void BeginPlay() override;

	// OnMoveCompleted: 이동 완료/실패 시 Chase 루프 재개
	// TODO: Behavior Tree 도입 시 이 루프는 BTService 또는 Decorator로 이전할 것
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

private:
	// TODO: 추후 Behavior Tree 도입 시 아래 Chase 로직을 BTTask_ChasePlayer 로 이전할 것
	//       현재는 MoveToActor() 직접 호출로 프로토타입 Chase 구현
	void ChasePlayer();

	UPROPERTY()
	TObjectPtr<APawn> PlayerPawn;

	/** 목표 도달 허용 반경 — 이 거리 안에 들어오면 MoveToActor 완료로 처리됨 */
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float AcceptanceRadius = 150.f;

	/** Chase 재시도 딜레이 (초) — MoveToActor 즉시 실패 시 재귀 방지용 */
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float ChaseRetryDelay = 0.3f;

	FTimerHandle ChaseTimerHandle;
};
