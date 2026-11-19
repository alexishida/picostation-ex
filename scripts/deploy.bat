@echo off
if not defined PICOSTATION_VARIANT set "PICOSTATION_VARIANT=picostation_pico2"
"%~dp0..\tools\menu\nops.exe" /exe "%~dp0..\build\%PICOSTATION_VARIANT%\menu\picostation-menu.psexe" /m COM12
