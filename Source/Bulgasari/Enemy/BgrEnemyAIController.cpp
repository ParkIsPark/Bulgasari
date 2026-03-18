#include "BgrEnemyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/PathFollowingComponent.h"

ABgrEnemyAIController::ABgrEnemyAIController()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABgrEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	// 플레이어 폰 캐싱 (인덱스 0 = 로컬 플레이어)
	PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	// TODO: Behavior Tree 도입 시 RunBehaviorTree(BT_Enemy) 호출로 교체
	ChasePlayer();
}

void ABgrEnemyAIController::ChasePlayer()
{
	if (!PlayerPawn) return;

	// TODO: Behavior Tree 도입 시 이 직접 호출을 BTTask_ChasePlayer 로 이전할 것
	// bUsePathfinding = false: Flying 폰은 지면 NavMesh를 쓸 수 없으므로 직선 추적 사용
	// TODO: 공중 장애물 회피가 필요해지면 다음 작업 필요:
	//       1. Project Settings → Navigation → Supported Agents에 Flying Agent 추가
	//       2. NavMeshBoundsVolume을 공중 높이까지 포함하도록 확장
	//       3. ABgrEnemyBase의 GetNavAgentPropertiesRef().bCanFly = true 설정
	//       4. 이 플래그를 true로 변경
	MoveToActor(PlayerPawn, AcceptanceRadius, true, false);
}

void ABgrEnemyAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	// MoveToActor가 NavMesh 없음 등으로 동기적(같은 콜 스택)으로 즉시 완료될 경우
	// ChasePlayer()를 직접 호출하면 무한 재귀가 발생한다.
	// 타이머로 다음 프레임 이후로 지연시켜 콜 스택을 끊는다.
	// TODO: BT 도입 후 이 루프는 BTService_UpdateTarget 또는 Decorator 로 대체
	GetWorldTimerManager().SetTimer(
		ChaseTimerHandle,
		this,
		&ABgrEnemyAIController::ChasePlayer,
		ChaseRetryDelay,
		false
	);
}
