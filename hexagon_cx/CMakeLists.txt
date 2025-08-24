set(CMAKE_BUILD_TYPE Debug)

cmake_minimum_required(VERSION 3.10)
project(zmq_radio_dish_c_example CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Yerel Kütüphane ve Başlık Dosyası Yollarını Ayarlama ---
# Proje kök dizinindeki 'include' klasörünü başlık dosyası yolu olarak ekliyoruz.
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
# Proje kök dizinini kütüphane yolu olarak ekliyoruz.
link_directories(${CMAKE_CURRENT_SOURCE_DIR})

# --- Derleyici Ayarları ---
# Tüm hedefler için DRAFT API'leri (ZMQ_RADIO, ZMQ_DISH dahil) aktive ediyoruz
add_compile_definitions(ZMQ_BUILD_DRAFT_API)

# --- Hedefleri Tanımlama ---
# App1, App2, App3 uygulamalarını derliyoruz.
# add_executable(app1 app1_source.cpp)
# add_executable(app2 app2_processor.cpp)
# add_executable(app3 app3_listener.cpp)

# Radio/Dish C API test uygulamaları
add_executable(c_radio_sender c_radio_sender.cpp)
add_executable(c_dish_receiver c_dish_receiver.cpp)

# --- Kütüphaneleri ve Yolları Tüm Hedeflere Bağlama ---
# foreach(TARGET app1 app2 app3 c_radio_sender c_dish_receiver)
foreach(TARGET c_radio_sender c_dish_receiver)
    # Gerekli kütüphaneleri bağlıyoruz (linkliyoruz).
    # Burada doğrudan 'zmq' olarak belirtiyoruz, çünkü link_directories ile yolunu belirttik.
    target_link_libraries(${TARGET} PRIVATE zmq)
endforeach()
