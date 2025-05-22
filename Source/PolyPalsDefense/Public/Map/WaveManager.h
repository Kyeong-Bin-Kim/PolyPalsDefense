#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PolyPalsState.h"
#include "WaveManager.generated.h"

class AWaveSpawner;
class AEnemyPawn;

UCLASS()
class POLYPALSDEFENSE_API AWaveManager : public AActor
{
    GENERATED_BODY()

public:
    AWaveManager();

protected:
    virtual void BeginPlay() override;

    // 플레이어 준비 완료 시 호출
    UFUNCTION()
    void OnPlayersReady();

    // 게임 오버 이벤트 핸들러
    UFUNCTION()
    void HandleGameOver();

public:
    // 라운드를 시작하는 함수
    UFUNCTION(BlueprintCallable, Category = "Wave")
    void StartRound(int32 RoundIndex);

    // 적이 골인했을 때 생명 감소 처리
    UFUNCTION()
    void HandleEnemyReachedGoal(AEnemyPawn* Enemy);

private:
    // 첫 웨이브 시작 지연용
    UFUNCTION()
    void StartFirstRound();

    // 라운드 종료 시 호출
    void EndRound();

    // 현재 라운드 인덱스
    int32 CurrentRoundIndex = 1;
    FTimerHandle RoundTimerHandle;

protected:
    // 에디터에서 지정하거나 월드에서 자동 검색될 단일 스포너
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave")
    AWaveSpawner* WaveSpawner;

    // 사전 준비 시간
    UPROPERTY(EditAnywhere, Category = "Wave")
    float PreparationTime = 30.f;

    // 라운드 지속 시간
    UPROPERTY(EditAnywhere, Category = "Wave")
    float RoundDuration = 60.f;

    // 플레이어 생명 및 데미지 설정
    UPROPERTY(EditAnywhere, Category = "Game")
    int32 PlayerLife = 50;

    UPROPERTY(EditAnywhere, Category = "Game")
    int32 BasicDamageToLife = 1;

    UPROPERTY(EditAnywhere, Category = "Game")
    int32 BossDamageToLife = 10;
};