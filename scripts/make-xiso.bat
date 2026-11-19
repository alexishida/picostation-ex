@echo off
if not defined PICOSTATION_VARIANT set "PICOSTATION_VARIANT=picostation_pico2"
cmake -S "%~dp0.." -B "%~dp0..\build\%PICOSTATION_VARIANT%" -DPICOSTATION_VARIANT=%PICOSTATION_VARIANT%
if errorlevel 1 exit /b %errorlevel%
cmake --build "%~dp0..\build\%PICOSTATION_VARIANT%"
