// 사용할 명령어   사용할 옵션			원본 파일이 있는 위치			사본을 저장할 위치


set SRC=.\Editor\CONVERT\Binary
set DST=.\Client\Bin\Resources\Models

rem 파일들을 하나씩 순회
for %%F in (.\Editor\CONVERT\Binary\*.*) do (
    set "FNAME=%%~nF"
    setlocal enabledelayedexpansion

    rem 목적지 폴더: DST\파일이름
    set "TARGET=!DST!\!FNAME!"

    rem 폴더가 없으면 생성
    if not exist "!TARGET!" (
        mkdir "!TARGET!"
    )

    rem 파일 복사
    xcopy /y "%%F" "!TARGET!\"
    endlocal
)