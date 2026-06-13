# DataStructure Project

응급실 환자 관리 시스템을 C와 raylib/raygui로 구현한 GUI 프로젝트입니다.

환자 접수, 대기열, 진료중 상태, 병상 현황, 처리 이력, 통계 화면을 한 화면 안에서 확인하고 조작할 수 있도록 구성되어 있습니다.

## 주요 기능

- 응급 환자와 일반 환자 등록
- 우선순위 기반 대기열 관리
- 진료중 환자 및 병상 현황 표시
- 처리 이력 확인
- 대기/진료중/완료 통계 표시
- raylib 기반 데스크톱 GUI

## 기술 스택

- C11
- CMake
- raylib
- raygui

## 프로젝트 구조

- `src/` - 애플리케이션 소스 코드
- `include/` - 프로젝트 헤더 파일
- `assets/` - 폰트, 이미지 등 리소스
- `external/` - 외부 헤더 및 서드파티 코드
- `build/` - CMake 빌드 디렉터리
- `bin/` - 실행 파일과 실행 시 복사되는 리소스

## 빌드 환경

현재 `CMakeLists.txt`는 로컬에 설치된 raylib 경로를 사용하도록 설정되어 있습니다.

```cmake
set(RAYLIB_ROOT "C:/Users/twent/scoop/apps/raylib-mingw/6.0")
```

raylib가 다른 위치에 설치되어 있다면 이 경로를 본인 환경에 맞게 수정해야 합니다.

## 빌드 방법

```bash
cmake -S . -B build
cmake --build build
```

빌드가 완료되면 실행 파일은 `bin/` 아래에 생성됩니다.

## 실행

```bash
bin/app.exe
```

## 참고

- 폰트 파일 `assets/fonts/malgun.ttf`가 필요합니다.
- 빌드 후 `assets/`는 실행 폴더로 자동 복사됩니다.
- 테스트용 환자 데이터가 시작 시점에 일부 등록됩니다.

## 라이선스

이 저장소의 라이선스가 아직 정해지지 않았다면, 추후 `LICENSE` 파일을 추가하는 것을 권장합니다.
