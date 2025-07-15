@REM @echo off
@REM rem Execute para ler valores seriais pela porta UDP 
@REM rem Se for do Simulador Workwi "inindkit%kitId%.local" por "localhost"
@REM for /f "tokens=1,2 delims==" %%a in (../platformio.ini) do (if %%a==kitId set kitId=%%b)
@REM com2tcp-rfc2217.bat \\.\CNCA1%kitId% inindkit%kitId%.local 400%kitId%

@echo off
SETLOCAL ENABLEDELAYEDEXPANSION

rem — Ajuste aqui para o seu utilitário com0com (installc.exe, setupc.exe, com0com.exe, etc)
set "COM0COMCLI=installc.exe"

rem 1. Extrai kitId do ../platformio.ini
for /f "tokens=1,2 delims==" %%A in (..\platformio.ini) do (
  if /I "%%A"=="kitId" set "kitId=%%B"
)

if not defined kitId (
  echo ERRO: nao foi possivel encontrar kitId em platformio.ini
  goto :EOF
)

rem 2. Cria o par virtual — assume que 'install' sem parametros gera CNCAx/CNCBx
for /f "tokens=3" %%P in ('%COM0COMCLI% install') do (
  set "CNCA_PORT=%%P"
)

echo Par virtual criado: !CNCA_PORT!

rem 3. Extrai o indice (o número apos "CNCA")
set "INDEX=!CNCA_PORT:~4!"

rem 4. Chama seu com2tcp passando a porta dinamica
rem    — ajusta o caminho para o seu .bat, se necessario
"%~dp0com2tcp-rfc2217.bat" \\.\!CNCA_PORT! inindkit%kitId%.local 400%kitId%

rem 5. Aguarda o usuario antes de remover o par
echo.
echo Pressione qualquer tecla para remover o par virtual...
pause >nul

rem 6. Remove o par que criamos
%COM0COMCLI% remove %INDEX%

echo Par virtual %INDEX% removido.
ENDLOCAL
