@echo off
REM Description: Copies files and folders from specified paths to target directories
REM Author: SPAN-Dev
REM Date: [Date]

REM Get the current directory path
for %%F in ("%CD%") do set CURRENT_DIR=%%~F
set DLLS_SOURCE_DIR=%CURRENT_DIR%\dlls
set DLLS_DEST_DIR=C:\Windows\VisionStudioAide
set AI_SOURCE_DIR=%CURRENT_DIR%\ai_files
set AI_DEST_DIR=C:\Program Files\SPAN Vision Studio
set GPT_SOURCE_DIR=%CURRENT_DIR%\gpt_files\python_file
set GPT_DEST_DIR=C:\Program Files\SPAN Vision Studio
set CHROMA_DB_SOURCE_DIR=%CURRENT_DIR%\gpt_files\database
set CHROMA_DB_DEST_DIR=C:\Program Files\SPAN Vision Studio\DATA

REM Ensure script is run as administrator
net session >nul 2>&1
if %errorLevel% neq 0 (
    echo This script requires administrative privileges. Please run as administrator.
    pause
    exit /b 1
)

REM Check if DLLs source directory exists
if not exist "%DLLS_SOURCE_DIR%" (
    echo Source directory "%DLLS_SOURCE_DIR%" does not exist. Exiting.
    exit /b 1
)

REM Check if DLLs destination directory exists
if not exist "%DLLS_DEST_DIR%" (
    echo Destination directory "%DLLS_DEST_DIR%" does not exist. Creating it...
    mkdir "%DLLS_DEST_DIR%"
)

REM Copy DLL files only
echo Copying DLL files from "%DLLS_SOURCE_DIR%" to "%DLLS_DEST_DIR%"...
for %%F in ("%DLLS_SOURCE_DIR%\*.dll") do copy "%%F" "%DLLS_DEST_DIR%" /Y

REM Check if AI source directory exists
if not exist "%AI_SOURCE_DIR%" (
    echo Source directory "%AI_SOURCE_DIR%" does not exist. Exiting.
    exit /b 1
)

REM Copy AI files only
echo Copying files from "%AI_SOURCE_DIR%" to "%AI_DEST_DIR%"...
for %%F in ("%AI_SOURCE_DIR%\*") do copy "%%F" "%AI_DEST_DIR%" /Y

REM Check if GPT source directory exists
if not exist "%GPT_SOURCE_DIR%" (
    echo Source directory "%GPT_SOURCE_DIR%" does not exist. Exiting.
    exit /b 1
)

REM Copy GPT files only
echo Copying files from "%GPT_SOURCE_DIR%" to "%GPT_DEST_DIR%"...
for %%F in ("%GPT_SOURCE_DIR%\*") do copy "%%F" "%GPT_DEST_DIR%" /Y

REM Check if CHROMA_DB source directory exists
if not exist "%CHROMA_DB_SOURCE_DIR%" (
    echo Source directory "%CHROMA_DB_SOURCE_DIR%" does not exist. Exiting.
    exit /b 1
)

REM Check if CHROMA_DB destination directory exists
if not exist "%CHROMA_DB_DEST_DIR%" (
    echo Destination directory "%CHROMA_DB_DEST_DIR%" does not exist. Creating it...
    mkdir "%CHROMA_DB_DEST_DIR%"
)

REM Copy CHROMA_DB folder (entire folder, including subfolders and files)
echo Copying folder "%CHROMA_DB_SOURCE_DIR%" to "%CHROMA_DB_DEST_DIR%"...
xcopy "%CHROMA_DB_SOURCE_DIR%" "%CHROMA_DB_DEST_DIR%\" /E /I /Y

REM Set the system environment variable
setx GROQ_API_KEY "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" /M

echo GROQ_API_KEY set successfully.
pip install -r requirements.txt

echo All files and folders copied successfully!
pause
