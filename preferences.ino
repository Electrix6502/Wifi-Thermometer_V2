void prefClear(){
  pref.begin("config", false);
  pref.clear();
  pref.end();
}

void prefRead(){
  pref.begin("config", false);
  apssid = pref.getString("apssid", "Wifi_Thermometer");
  appassword = pref.getString("appass", "12345678");
  ssid = pref.getString("ssid");
  password = pref.getString("pass");
  sensor1 = pref.getString("s1", "Vorlauf");
  sensor2 = pref.getString("s2", "Ruecklauf");
  color = pref.getInt("col", 0);
  color2 = pref.getInt("col2", 0);
  storeinterval = pref.getInt("store", 6);
  config_done = pref.getInt("done", 0); 
  mqttServer = pref.getString("mqttServer", "");
  mqttUser = pref.getString("mqttUser", "");
  mqttPass = pref.getString("mqttPass", "");
  pref.end();
  yield();
}

void prefWrite(){
  pref.begin("config", false);
  pref.putString("apssid", apssid);
  pref.putString("appass", appassword);
  pref.putString("ssid", ssid);
  pref.putString("pass", password);
  pref.putString("s1", sensor1);
  pref.putString("s2", sensor2);
  pref.putInt("col", color);
  pref.putInt("col2", color2);
  pref.putInt("store", storeinterval);
  if(ssid.length() < 4 || password.length() < 8){
    pref.putInt("done", 0);
  } else {
    pref.putInt("done", 1);
  }
  pref.putString("mqttServer", mqttServer);
  pref.putString("mqttUser", mqttUser);
  pref.putString("mqttPass", mqttPass);
  pref.end();
}
