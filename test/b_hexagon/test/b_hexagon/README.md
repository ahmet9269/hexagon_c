# b_hexagon Unit Tests

## Genel Bakış

Bu dizin, b_hexagon projesinin SOLID prensipleriyle uyumlu unit test altyapısını içerir.

## Dizin Yapısı

```
test/b_hexagon/
├── Makefile                    # Test derleme ve çalıştırma
├── README.md                   # Bu dosya
├── main_test.cpp               # GTest giriş noktası
├── mocks/                      # GMock-tabanlı mock sınıfları
│   ├── MockMessageSocket.hpp   # IMessageSocket mock'u
│   ├── MockCalculatorService.hpp # ICalculatorService mock'u
│   ├── MockIncomingPort.hpp    # IExtrapTrackDataIncomingPort mock'u
│   └── MockOutgoingPort.hpp    # IDelayCalcTrackDataOutgoingPort mock'u
├── adapters/
│   ├── common/
│   │   └── AdapterManagerTest.cpp      # Pipeline yönetimi testleri
│   ├── incoming/
│   │   └── ExtrapTrackDataZeroMQIncomingAdapterTest.cpp
│   └── outgoing/
│       └── DelayCalcTrackDataZeroMQOutgoingAdapterTest.cpp
├── domain/
│   ├── logic/
│   │   ├── ProcessTrackUseCaseTest.cpp  # Use case testleri
│   │   └── CalculatorServiceTest.cpp    # Calculator testleri
│   └── model/
│       ├── ExtrapTrackDataTest.cpp      # Giriş model testleri
│       └── DelayCalcTrackDataTest.cpp   # Çıkış model testleri
└── utils/
    └── (logger testleri eklenecek)
```

## Test Stratejisi

### SOLID Tabanlı Tasarım

Testler, projenin Dependency Inversion Principle (DIP) uygulamasından yararlanır:

1. **IMessageSocket** → MockMessageSocket ile ağ bağımlılığı kaldırılır
2. **ICalculatorService** → MockCalculatorService ile hesaplama kontrol edilir
3. **IIncomingPort/IOutgoingPort** → Mock portlar ile veri akışı izlenir

### Test Kategorileri

| Kategori | Açıklama |
|----------|----------|
| Unit Tests | Her sınıfı izole olarak test eder |
| Integration Tests | Bileşenler arası etkileşimi test eder |
| Thread Safety | Eşzamanlı erişim senaryolarını doğrular |

## Kullanım

### Testleri Derle ve Çalıştır

```bash
# b_hexagon dizininden
make test

# veya test dizininden
cd test/b_hexagon
make
make test
```

### Tek Test Dosyası

```bash
./build/b_hexagon_tests --gtest_filter=ProcessTrackUseCaseTest.*
```

### Ayrıntılı Çıktı

```bash
./build/b_hexagon_tests --gtest_output=xml:test_results.xml
```

## Bağımlılıklar

- GTest 1.14+
- GMock 1.14+
- ZeroMQ 4.3+ (Draft API)
- spdlog 1.12

## Mock Kullanımı

### MockMessageSocket Örneği

```cpp
#include "mocks/MockMessageSocket.hpp"

TEST_F(AdapterTest, SendData_CallsSocketSend) {
    auto mockSocket = std::make_unique<mocks::MockMessageSocket>();
    
    EXPECT_CALL(*mockSocket, connect(_)).WillOnce(Return(true));
    EXPECT_CALL(*mockSocket, send(_, _)).WillOnce(Return(true));
    
    MyAdapter adapter(std::move(mockSocket));
    adapter.start();
    adapter.send(testData);
    adapter.stop();
}
```

### MockCalculatorService Örneği

```cpp
#include "mocks/MockCalculatorService.hpp"

TEST_F(UseCaseTest, ProcessData_CalculatesDelay) {
    auto mockCalc = std::make_unique<mocks::MockCalculatorService>();
    
    EXPECT_CALL(*mockCalc, calculateDelay(_))
        .WillOnce(Return(42));
    
    ProcessTrackUseCase useCase(std::move(mockCalc), mockPort);
    useCase.onDataReceived(testData);
}
```

## Kod Kapsamı Hedefleri

- Domain Logic: %95+
- Adapters: %90+
- Models: %100

## Katkıda Bulunma

1. Yeni testler `[Component]Test.cpp` formatında adlandırılmalı
2. Her test fixture'ı `[Component]Test` olarak adlandırılmalı
3. Test metodları `[Method]_[Scenario]_[ExpectedResult]` pattern'ini takip etmeli
