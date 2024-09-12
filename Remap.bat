:Display
echo off
cls
echo [96m=========================================================================================================
echo [95m                                              Keyboard Remapper
echo [96m=========================================================================================================
echo [91m            R: (Remaps)                        D: (Deletes)                        Q: (Quits)
echo.
echo [93mEnter data in hexadecimal, singal digits with a zero in front
echo Remeber the null requires a byte (eight bits)
echo Do not enter null terminator or any spaces
echo [97m

:Choice
choice /c rdq /n /m "What do you want to do? "
if ERRORLEVEL 255 (
    goto Choice
) else if ERRORLEVEL 3 (
    :Cleanup
    echo [0m
    exit /b
) else if ERRORLEVEL 2 (
    :Delete
    choice /c yn /m "Are you sure you want to delete the ScanCode Map? "
    if ERRORLEVEl 255 (
        goto Delete
    ) else if ERRORLEVEL 2 (
        goto Choice
    ) else if ERRORLEVEL 1 (
        reg delete "HKLM\SYSTEM\CurrentControlSet\Control\Keyboard Layout" /v "ScanCode Map" /f
    )
) else if ERRORLEVEL 1 (
    goto Input
)

:Input
set /p bytes=How many bytes do you need?
set /p map=Enter the map: 
echo.
echo Double-check your inputs before proceding!!

:Check
choice /c yn /m "Are there spaces, and do singal digits have a zero in front? "
if ERRORLEVEL 255 (
    goto Check
) else if ERRORLEVEL 2 (
    reg add "HKLM\SYSTEM\CurrentControlSet\Control\Keyboard Layout" /v "ScanCode Map" /t REG_BINARY /d 0000000000000000%bytes%000000%map%00000000 /f
) else if ERRORLEVEL 1 (
    echo.
    goto Input
)