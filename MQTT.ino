void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
}

void mqttLoop(){
  if(mqttEnable){
    client.loop();
    yield();
  }  
}

void mqttPub(){
  if(mqttEnable){
    String topic1 = hostname + "/temperatur/sensor1";
    String topic2 = hostname + "/temperatur/sensor2";
    if(mqttUser.length() > 3){
      if (client.connect(hostname.c_str(), mqttUser.c_str(), mqttPass.c_str())) {
        client.publish(topic1.c_str(), String(sensors.getTempC(device0)).c_str());
        if(sensCnt > 1){
          client.publish(topic2.c_str(), String(sensors.getTempC(device1)).c_str());
        }
      }          
    } else {
      if (client.connect(hostname.c_str())) {
        client.publish(topic1.c_str(), String(sensors.getTempC(device0)).c_str());
        if(sensCnt > 1){
          client.publish(topic2.c_str(), String(sensors.getTempC(device1)).c_str());
        }
      }          
    }
  }  
}
