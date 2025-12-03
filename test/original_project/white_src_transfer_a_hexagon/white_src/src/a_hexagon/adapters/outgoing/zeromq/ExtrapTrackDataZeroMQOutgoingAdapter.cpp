#include "adapters/outgoing/zeromq/ExtrapTrackDataZeroMQOutgoingAdapter.hpp"
#include <iostream>
#include <sstream>

namespace domain {
namespace adapters {
namespace outgoing {

ExtrapTrackDataZeroMQOutgoingAdapter::ExtrapTrackDataZeroMQOutgoingAdapter()
    : context(1), socket(context, ZMQ_RADIO), group_name_("ExtrapTrackData") {
    loadConfiguration();
    socket.connect(endpoint);
    std::cout << "Outgoing adapter bağlandı: " << endpoint << " (protokol: " << protocol << ", grup: " << group_name_ << ")" << std::endl;
}

void ExtrapTrackDataZeroMQOutgoingAdapter::sendExtrapTrackData(const std::vector<domain::model::ExtrapTrackData>& data) {
    for (const auto& item : data) {
        // Modelin kendi binary serialization'ını kullan
        std::vector<uint8_t> binaryData = item.serialize();
        zmq::message_t message(binaryData.begin(), binaryData.end());
        
        // RADIO socket için group belirleme
        message.set_group(group_name_.c_str());
        
        socket.send(message, zmq::send_flags::none);
    }
}

void ExtrapTrackDataZeroMQOutgoingAdapter::loadConfiguration() {

        std::cerr << "Varsayılan konfigürasyon kullanılıyor..." << std::endl;
        
        // Varsayılan değerler
        protocol = "udp";
        socketType = ZMQ_RADIO;
        endpoint = "udp://udn;239.1.1.1:9001";

        //socket->set(zmq::sockopt::sndhwm, 1);       // Unlimited receive buffer
        //dish_socket_->set(zmq::sockopt::linger, 0);       // No linger on close
        //socket->set(zmq::sockopt::immediate, 1);
        socket.close();
        socket = zmq::socket_t(context, socketType);

}





// Tek veri gönderimi için yeni metod
void ExtrapTrackDataZeroMQOutgoingAdapter::sendExtrapTrackData(const domain::model::ExtrapTrackData& data) {
    std::vector<domain::model::ExtrapTrackData> singleData = {data};
    sendExtrapTrackData(singleData); // Mevcut vector metodunu kullan
}

}
}
}
