# PolyPals Defense

## 1. 프로젝트 개요 (Project Overview)
- **게임 장르·목표**: 4인 협동 카툰풍 타워디펜스  
- **핵심 특징**: 멀티플레이, 커스텀 타워 시스템, 데이터 주도 밸런싱, 카툰풍 그래픽  

## 2. 주요 기능 (Key Features)
- 멀티플레이어 모드 (최대 4명)  
- 다양한 타워(근거리·원거리) & 업그레이드 시스템  
- 라운드별 웨이브 관리 & 버프/디버프 효과  
- UMG 기반 HUD·메뉴 

## 3. 요구사항 및 환경 (Prerequisites)
- **엔진 버전**: Unreal Engine 5.5.x  
- **플랫폼**: Windows 10 이상 (게임 서버는 Dedicated Server 모드)  
- **온라인 서브시스템**: Null or Steam/EOS (`DefaultEngine.ini` 설정 참고)  
- **컴파일 도구**: Visual Studio 2022 (C++17), Windows SDK  

## 4. 설치·빌드 방법 (Setup & Build)
1. 소스코드 클론  
   ```bash
   git clone https://github.com/YourOrg/PolyPalsDefense.git
   cd PolyPalsDefense