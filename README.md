# PolyPals Defense

## 프로젝트 개요
- 타워 디펜스 장르의 UnrealEngine5.5.4release 버전을 이용한 프로젝트입니다.
- 데디케이트 서버를 이용한 멀티 플레이를 지원하여 여러 플레이어(최대 4인)가 협동으로 스테이지를 방어할 수 있습니다.

## 주요 기능
- 3종류의 타워와 다양한 적 타입이 등장하는 웨이브 기반 전투
- 스테이지 선택 후 플레이어들이 준비를 완료하면 게임 시작
- UMG 기반 HUD 및 메뉴 시스템

## 데이터 매니저
- UTowerDataManager ('Source/PolyPalsDefense/Public/Core/Subsystems/TowerDataManager.h')
  - 월드 서브시스템으로 동작하며 게임 시작 시 타워 관련 'PrimaryDataAsset'을 불러옵니다.
  - TowerMaterialData 와 TowerPropertyData를 'PolyPalsDefenseAssetManager' 를 통해 로드하여 메모리에 캐시합니다.
  - 각 타워의 설치 비용, 업그레이드 수치, 능력 지속시간과 강도를 제공하여 다른 시스템에서 참조합니다.

## 적 시스템
- AEnemyPawn ('Source/PolyPalsDefense/Public/Enemy/EnemyPawn.h')
  - 스플라인을 따라 이동하는 'UEnemySplineMovementComponent' 와 체력 및 상태이상을 관리하는 'UEnemyStatusComponent' 로 구성됩니다.
  - 'UEnemyDataAsset' 에서 스탯, 보상, 외형 정보를 읽어 초기화하며, 사망 시 플레이어에게 골드를 지급합니다.
  - 'EnemyPoolComponent' 를 통해 객체 풀링을 사용하여 반복 생성 비용을 줄입니다.

## 타워 시스템
- APlacedTower ('Source/PolyPalsDefense/Public/Tower/PlacedTower.h')
  - 설치 후 'UTowerAttackComponent' 를 통해 적 탐지 및 공격을 수행합니다.
  - 공격 컴포넌트는 상태 머신(ETowerState)으로 동작하며 타겟 지정, 사격, 지연 등의 단계를 처리합니다.
  - 업그레이드 요청은 RPC 로 서버에서 처리하고, 결과는 'Multicast_UpdateLevelVisuals' 로 전체 동기화합니다.

## 멀티플레이어
- APolyPalsGameMode와 APolyPalsState
  - 로비에서 최대 인원 수와 스테이지 정보를 설정하고 모든 플레이어가 준비되면 게임을 시작합니다.
  - 'OnHealthChanged' 와 'OnRoundChanged' 델리게이트를 통해 HUD에 현재 생명과 라운드를 알립니다.
- APolyPalsController ('Source/PolyPalsDefense/Public/Multiplayer/PolyPalsController.h')
  - 플레이어 색상, 로비 UI, 스테이지 선택 UI 등을 관리합니다.
  - RPC 로 타워 설치 위치를 서버에 전달합니다.
- UPolyPalsGameInstance
  - 스팀 로그인 처리를 담당하며 닉네임을 취득합니다.
  - 로비 이름과 스테이지 정보를 임시로 보관합니다.

## 맵과 웨이브
- AWaveManager 와 AWaveSpawner
  - 라운드 타이머와 적 생성 스케줄을 관리합니다.
  - 'WavePlanRow' 데이터 테이블을 통해 라운드마다 등장할 적과 수량, 보스 몬스터 여부를 정의하며, 스폰은 'EnemySpawnEntry' 정보를 사용합니다.
  - 적이 목표 지점에 도달하거나 사망할 때 남은 적 수를 갱신하고 라운드 종료를 판단합니다.

## UI 시스템
- 다양한 UMG 위젯
  - 라운드 정보, 남은 생명, 골드, 다음 웨이브 시간 등을 실시간으로 표시합니다.
  - 메인화면 ,로비, 플레이어 슬롯과 스테이지 선택 화면 등도 각각 별도의 위젯 클래스로 구현되어 있습니다.
  - UPolyPalsHUD는 게임 시작 시 UGamePlayingUIWidget와 UGameResultWidget 관리하며 컨트롤러와 연동됩니다.

## 빌드 및 실행
1. Git을 통해 UnrealEngine5.5.4release 버전을 다운로드
2. 저장소 클론 후 언리얼 엔진으로 프로젝트 파일을 연 뒤 'Generate Visual Studio project files'를 실행합니다.
3. Visual Studio 2022 에서 'PolyPalsDefense' 프로젝트를 Development Editor, Development Server 순으로 빌드합니다.
4. Editor, Server로 패키징합니다.
5. Steam을 켭니다.
6. 패키징된 Server.exe를 실행 후 패키징 된 Editor.exe를 실행합니다.