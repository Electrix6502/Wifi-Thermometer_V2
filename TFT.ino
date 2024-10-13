void displayGraph(){
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_YELLOW,TFT_BLACK);
  tft.setTextSize(1);

  int twindow = tmax - tmin;
  
  if(twindow < 1000){
    twindow = 1000;
  }

  int tbot = tmin - (twindow / 2);
  int ttop = tmax + (twindow / 2);
  int tteil = (ttop - tbot) / 6;
  
  tft.setCursor(0, 120, 2);
  tft.println(tbot / 100);  
  tft.setCursor(0, 98, 2);
  tft.println((tbot + tteil) / 100); 
  tft.setCursor(0, 75, 2);
  tft.println((tbot + (tteil * 2)) / 100); 
  tft.setCursor(0, 52, 2);
  tft.println((tbot + (tteil * 3)) / 100); 
  tft.setCursor(0, 29, 2);
  tft.println((tbot + (tteil * 4)) / 100); 
  tft.setCursor(0, 4, 2);
  tft.println(ttop / 100);
  
  tft.drawLine(16,134,16,0, TFT_YELLOW);
  tft.drawLine(16,134,239,134, TFT_YELLOW);

  int p = pos;
  int x = 0;
  int x1 = 0;
  int y = 0;
  int d = 1;
  int t = 0;
  int t1 = 0;

  if(sensCnt > 1){
    tempdata = "\"Punkt\",\"" + sensor1 + "\",\"" + sensor2 + "\"\n";
  } else {
    tempdata = "\"Punkt\",\"" + sensor1 + "\"\n";
  } 
  
  for(int i = 0; i < 224; i++){
    
    t = ringbuffer[p];
    if(sensCnt > 1){
      t1 = ringbuffer1[p];
    }
    
    p++;
    if(p >= 224){
      p = 0;    
    }
    if(t < tbot || t > ttop){
      continue;
    }
    
    x = map(t,tbot, ttop, 134, 0);
    tft.drawPixel(17 + y, x, colors[color]);
    if(sensCnt > 1){
      x1 = map(t1,tbot, ttop, 134, 0);
      tft.drawPixel(17 + y, x1, colors[color2]);
      tempdata += String(d++) + "," + String(t / 100.0) + "," + String(t1 / 100.0) + "\n";
    } else {
      tempdata += String(d++) + "," + String(t / 100.0) + "\n";
    }
    y ++;    
  }
}

void displayData(){  
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_YELLOW,TFT_BLACK);  
  tft.setTextSize(2);
  tft.setCursor(0, 0, 2);
  tft.println(sensor1);
  tft.setTextSize(1); 
  tft.setCursor(160, 0, 2);
  tft.println(voltage);
  
  tft.setTextSize(2);
  int xpos = tft.width() / 2; // Half the screen width
  int ypos = tft.height() - 5;
  
  tft.setTextColor(colors[color], TFT_BLACK, true);
  tft.setTextDatum(C_BASELINE);
  tft.drawString((String)sensors.getTempC(device0), xpos, ypos, 6);
}

void displayData2(){  
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_YELLOW,TFT_BLACK);  
  tft.setTextSize(2);
  tft.setCursor(0, 0, 2);
  tft.println(sensor2);
  tft.setTextSize(1); 
  tft.setCursor(160, 0, 2);
  tft.println(voltage);
  
  tft.setTextSize(2);
  int xpos = tft.width() / 2; // Half the screen width
  int ypos = tft.height() - 5;
  
  tft.setTextColor(colors[color2], TFT_BLACK, true);
  tft.setTextDatum(C_BASELINE);
  tft.drawString((String)sensors.getTempC(device1), xpos, ypos, 6);
}

void displayWifi(){
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_YELLOW,TFT_BLACK);  
  tft.setTextSize(2);
  tft.setCursor(0, 0, 2);
  if(config_done){
    tft.println("Wifi:");
    tft.println(ssid); 
    tft.setTextSize(1);
    tft.print("IP: ");
    tft.println(WiFi.localIP()); 
  } else {
    tft.println("WifiAP:");
    tft.println(apssid);
    tft.setTextSize(1);
    IPAddress myIP = WiFi.softAPIP();
    tft.print("IP: ");
    tft.println(myIP);     
  }
}