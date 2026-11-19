@echo off
python -m venv "%~dp0..\env"
if errorlevel 1 exit /b %errorlevel%
"%~dp0..\env\Scripts\python.exe" -m pip install --upgrade pip
if errorlevel 1 exit /b %errorlevel%
"%~dp0..\env\Scripts\python.exe" -m pip install -r "%~dp0..\third_party\ps1-bare-metal\tools\requirements.txt"
