@echo off
echo ========================================
echo ZMQ Test Uygulamalari Calistirma Rehberi
echo ========================================
echo.
echo Bu uygulamalar birbirleriyle iletisim kuran 3 farkli programdir:
echo.
echo App1 (Kaynak):     Kullanicidan mesaj alir ve yayinlar
echo App2 (Isleyici):   App1'den mesaj alir, isler ve yeniden yayinlar  
echo App3 (Dinleyici):  Hem App1'den hem App2'den mesajlari dinler
echo.
echo ONEMLI: Uygulamalari asagidaki sirada calistirin:
echo.
echo 1. ONCE: app3.exe (Dinleyici) - 3 ayri terminal penceresinde
echo 2. SONRA: app2.exe (Isleyici)
echo 3. EN SON: app1.exe (Kaynak)
echo.
echo Ornek kullanim:
echo.
echo Terminal 1: app3.exe
echo Terminal 2: app2.exe  
echo Terminal 3: app1.exe
echo   Mesaj yaz: "Merhaba Dunya"
echo   Cikis: "exit"
echo.
echo App3'te hem dogrudan App1'den hem de App2'den gelen
echo mesajlari goreceksiniz. Gecikme surelerini karsilastirebilirsiniz.
echo.
pause
