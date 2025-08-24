@echo off
echo ========================================
echo ZMQ Test Uygulamalari Derleme Scripti
echo ========================================
echo.

echo App1 (Kaynak) derleniyor...
g++ -std=c++17 -IC:/msys64/mingw64/include -LC:/msys64/mingw64/lib -o app1.exe app1_compatible.cpp -lzmq -lws2_32
if %ERRORLEVEL% EQU 0 (
    echo [OK] App1 basariyla derlendi
) else (
    echo [HATA] App1 derlenemedi
    goto :error
)

echo App2 (Isleyici) derleniyor...
g++ -std=c++17 -IC:/msys64/mingw64/include -LC:/msys64/mingw64/lib -o app2.exe app2_compatible.cpp -lzmq -lws2_32
if %ERRORLEVEL% EQU 0 (
    echo [OK] App2 basariyla derlendi
) else (
    echo [HATA] App2 derlenemedi
    goto :error
)

echo App3 (Dinleyici) derleniyor...
g++ -std=c++17 -IC:/msys64/mingw64/include -LC:/msys64/mingw64/lib -o app3.exe app3_compatible.cpp -lzmq -lws2_32
if %ERRORLEVEL% EQU 0 (
    echo [OK] App3 basariyla derlendi
) else (
    echo [HATA] App3 derlenemedi
    goto :error
)

echo.
echo ========================================
echo TUM UYGULAMALAR BASARIYLA DERLENDI!
echo ========================================
echo.
echo Calistirma sirasi:
echo 1. Yeni bir terminal: app3.exe (Dinleyici)
echo 2. Yeni bir terminal: app2.exe (Isleyici) 
echo 3. Yeni bir terminal: app1.exe (Kaynak)
echo.
pause
goto :end

:error
echo.
echo DERLEME HATASI! Lutfen hata mesajlarini kontrol edin.
pause

:end
