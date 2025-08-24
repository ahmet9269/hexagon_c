@echo off
echo ========================================
echo ZMQ DLL Bagimliliklarini Kopyalama
echo ========================================
echo.

echo Gerekli DLL dosyalari kopyalaniyor...

if exist "C:\msys64\mingw64\bin\libzmq.dll" (
    copy "C:\msys64\mingw64\bin\libzmq.dll" .
    echo [OK] libzmq.dll kopyalandi
) else (
    echo [HATA] libzmq.dll bulunamadi
)

if exist "C:\msys64\mingw64\bin\libwinpthread-1.dll" (
    copy "C:\msys64\mingw64\bin\libwinpthread-1.dll" .
    echo [OK] libwinpthread-1.dll kopyalandi
) else (
    echo [HATA] libwinpthread-1.dll bulunamadi
)

if exist "C:\msys64\mingw64\bin\libgcc_s_seh-1.dll" (
    copy "C:\msys64\mingw64\bin\libgcc_s_seh-1.dll" .
    echo [OK] libgcc_s_seh-1.dll kopyalandi
) else (
    echo [HATA] libgcc_s_seh-1.dll bulunamadi
)

if exist "C:\msys64\mingw64\bin\libstdc++-6.dll" (
    copy "C:\msys64\mingw64\bin\libstdc++-6.dll" .
    echo [OK] libstdc++-6.dll kopyalandi
) else (
    echo [HATA] libstdc++-6.dll bulunamadi
)

if exist "C:\msys64\mingw64\bin\libsodium-23.dll" (
    copy "C:\msys64\mingw64\bin\libsodium-23.dll" .
    echo [OK] libsodium-23.dll kopyalandi (opsiyonel)
)

echo.
echo ========================================
echo DLL Kopyalama Tamamlandi!
echo ========================================
echo.
echo Artik uygulamalari calistirabilirsiniz:
echo 1. app3.exe (Dinleyici)
echo 2. app2.exe (Isleyici)
echo 3. app1.exe (Kaynak)
echo.
pause
