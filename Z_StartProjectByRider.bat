@echo off

set pwd=%cd%
for %%f in (*.sln) do (
    set file=%%f
    break
)

d:
cd "D:\Program Files\JetBrains\JetBrains Rider 2024.2.7\bin"

start "rider64" "rider64.exe" %pwd%\%file%