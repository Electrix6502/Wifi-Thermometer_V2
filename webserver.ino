
File fsUploadFile;
 
const char redirect_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<head>
<meta charset="UTF-8">
<meta http-equiv="refresh" content="0; url=/">
<script type="text/javascript">
window.location.href = "/"
</script>
<title>Page Redirection</title>
</head>
<body>
If you are not redirected automatically, follow this <a href='/'>link</a>.
</body>
</html>
)rawliteral";

const char upload_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
<title>File Upload</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="stylesheet" href="style.css">
</head>
<body>
<div class="head">
  <div class="dropdown">
    <button class="btn dropbtn">Menu</button>
      <div class="dropdown-content">
        <a href="/">Home</a>
        <a href="/conf.htm">Konfiguration</a>
        <a href="/down.htm">Download</a>
        <a href="/upload">Upload</a>
        <a href="/update">Firmware</a>
        <a href="/reset">RESET</a>          
      </div>
   </div>
</div>
<div>
<h2>File Upload</h2>
<form method='POST' action='/upload' enctype='multipart/form-data'><input type='file' name='upload'><br><p></p><input type='submit' value='Upload' class='btn'></form>
</div>
</body>
</html>
)rawliteral";

const char down_head_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
<title>Download</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="stylesheet" href="style.css">
</head>
<body>
<div class="head">
  <div class="dropdown">
    <button class="btn dropbtn">Menu</button>
      <div class="dropdown-content">
        <a href="/">Home</a>
        <a href="/conf.htm">Konfiguration</a>
        <a href="/down.htm">Download</a>
        <a href="/upload">Upload</a>
        <a href="/update">Firmware</a>
        <a href="/reset">RESET</a>          
      </div>
   </div>
</div>
<div>
<h2>DOWNLOAD</h2>
)rawliteral";

const char reset_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
<title>RESET</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="stylesheet" href="style.css">
</head>
<body>
<div class="head">
  <div class="dropdown">
    <button class="btn dropbtn">Menu</button>
      <div class="dropdown-content">
        <a href="/">Home</a>
        <a href="/conf.htm">Konfiguration</a>
        <a href="/down.htm">Download</a>
        <a href="/upload">Upload</a>
        <a href="/update">Firmware</a>
        <a href="/reset">RESET</a>          
      </div>
   </div>
</div>
<div>
<h2>RESET</h2>
<p>Wollen Sie wirklich die Werkseinstellungen wiederherstellen?</p>
<form method='POST' action='/reset.htm' enctype='multipart/form-data'><input type='submit' name='reset' value='RESET' class='btn'></form>
</div>
</body>
</html>
)rawliteral";

const char update_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
<title>Firmware Update</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="stylesheet" href="style.css">
<script type="text/javascript">
function getData(id){
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
    document.getElementById(id).innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/data?c="+id, true);
  xhttp.send(); 
}
</script>
</head>
<body>
<div class="head">
  <div class="dropdown">
    <button class="btn dropbtn">Menu</button>
      <div class="dropdown-content">
        <a href="/">Home</a>
        <a href="/conf.htm">Konfiguration</a>
        <a href="/down.htm">Download</a>
        <a href="/upload">Upload</a>
        <a href="/update">Firmware</a>
        <a href="/reset">RESET</a>           
      </div>
   </div>
</div>
<div>
<h2>Firmware Update</h2>
<p>Aktulle Firmware: <div id='firm'></div></p>
<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><br><p></p><input type='submit' value='Update' class='btn'></form>
</div>
<script type="text/javascript">getData('firm');</script>
</body>
</html>
)rawliteral";

void wifisetup(){
  // Connect to Wi-Fi
  int timeout = 60;
  uint32_t chipId = 0;
  String id = "";

	for(int i=0; i<17; i=i+8) {
	  chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
	}

  hostname += "-";
  
  id = String(chipId, HEX);

  id.toUpperCase();

  hostname += id;

  WiFi.setHostname(hostname.c_str()); //define hostname

  WiFi.begin(ssid.c_str(), password.c_str());
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    timeout--;
    if(!timeout){
      apsetup();
      break;
    }
    Serial.println("Connecting to WiFi..");
  }
}

void apsetup(){
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(apssid.c_str(), appassword.c_str());
  IPAddress myIP = WiFi.softAPIP();
}

void handleData(){
  String val = "";
  String res = "";
  
  if(server.hasArg("c")){
    val = server.arg("c"); 
  } else {
    server.send(404, "text/plain", "Not Found");
    return;
  }
  if(val == "firm") res = fwversion;
  if(val == "ssid") res = ssid;
  if(val == "pass") res = password;
  if(val == "s1") res = sensor1;
  if(val == "s2") res = sensor2;
  if(val == "col") res = String(color);
  if(val == "col2") res = String(color2);  
  if(val == "store") res = String(storeinterval);
  if(val == "ms") res = mqttServer;
  if(val == "mu") res = mqttUser;
  if(val == "mp") res = mqttPass;
  server.send(200, "text/plain", res);
}

void handleConfig(){
  if(server.args()){
    ssid = server.arg("ssid");
    password = server.arg("pass");
    sensor1 = server.arg("s1");
    sensor2 = server.arg("s2");
    color = server.arg("col").toInt();
    color2 = server.arg("col2").toInt();  
    storeinterval = server.arg("store").toInt();
    mqttServer = server.arg("ms");
    mqttUser = server.arg("mu");
    mqttPass = server.arg("mp");  
  }
  server.sendHeader("Connection", "close");
  server.send(200, "text/plain", "Gespeichert! Neustart!");
  yield();
  prefWrite();
  delay(1000);
  ESP.restart();
}

void handleReset(){
  if(server.hasArg("reset")){
    Serial.println("RESET");
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", "RESET! Neustart!");
    yield();
    nvs_flash_erase(); // erase the NVS partition and...
    nvs_flash_init(); // initialize the NVS partition.
    delay(1000);
    ESP.restart(); 
  }
  server.sendHeader("Connection", "close");
  server.send(200, "text/html", redirect_html);
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

//format bytes
String formatBytes(size_t bytes) {
  if (bytes < 1024) {
    return String(bytes) + "B";
  } else if (bytes < (1024 * 1024)) {
    return String(bytes / 1024.0) + "KB";
  } else if (bytes < (1024 * 1024 * 1024)) {
    return String(bytes / 1024.0 / 1024.0) + "MB";
  } else {
    return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB";
  }
}

String getContentType(String filename) {
  if (server.hasArg("download")) {
    return "application/octet-stream";
  } else if (filename.endsWith(".htm")) {
    return "text/html";
  } else if (filename.endsWith(".html")) {
    return "text/html";
  } else if (filename.endsWith(".css")) {
    return "text/css";
  } else if (filename.endsWith(".js")) {
    return "application/javascript";
  } else if (filename.endsWith(".png")) {
    return "image/png";
  } else if (filename.endsWith(".gif")) {
    return "image/gif";
  } else if (filename.endsWith(".jpg")) {
    return "image/jpeg";
  } else if (filename.endsWith(".ico")) {
    return "image/x-icon";
  } else if (filename.endsWith(".xml")) {
    return "text/xml";
  } else if (filename.endsWith(".pdf")) {
    return "application/x-pdf";
  } else if (filename.endsWith(".zip")) {
    return "application/x-zip";
  } else if (filename.endsWith(".gz")) {
    return "application/x-gzip";
  }
  return "text/plain";
}

bool exists(String path){
  bool yes = false;
  File file = SPIFFS.open(path, "r");
  if(!file.isDirectory()){
    yes = true;
  }
  file.close();
  return yes;
}

bool handleFileRead(String path) {
  if (path.endsWith("/")) {
    path += "index.html"; 
  }
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  if (exists(pathWithGz) || exists(path)) {
    if (exists(pathWithGz)) {
      path += ".gz";
    }
    File file = SPIFFS.open(path, "r");
    server.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}

void handleFileUpload() {
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    String filename = upload.filename;
    if (!filename.startsWith("/")) {
      filename = "/" + filename;
    }
    fsUploadFile = SPIFFS.open(filename, "w");
    filename = String();
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (fsUploadFile) {
      fsUploadFile.write(upload.buf, upload.currentSize);
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    if (fsUploadFile) {
      fsUploadFile.close();
    }
  }
}

void handleDownload(){
  String text = "";
  String path = "/";
  
  File root = SPIFFS.open(path);
  path = String();

  if(root.isDirectory()){
      File file = root.openNextFile();
      while(file){
        text += "<a href=\"";
        if(!file.isDirectory()){
          text += String(file.path());
        }
        text += "\" download>";
        text += String(file.path()).substring(1);
        text += "</a> - -  - <a href=\"delete?file=";
        text += String(file.path());
        text += "\">LOESCHEN</a>"; 
        text += "<br>\r\n";
        file = root.openNextFile();
      }
  }
  
  text += "</div></body></html>";
  server.send(200, "text/html", down_head_html + text);
}

void handleDelete(){
  String val = "";
  String res = "";
  
  if(server.hasArg("file")){
    val = server.arg("file"); 
  }   

  SPIFFS.remove(val);

  server.sendHeader("Connection", "close");
  server.send(200, "text/html", redirect_html);  
}

void serversetup(void) {
  if(!SPIFFS.begin()){
    SPIFFS.format();
    SPIFFS.begin();
  }
  
  for(int i = 1; i < 101; i++){
    String path = "/temp_" + String(i) + ".csv";
    if(!exists(path)){
      files = i;
      if(i > 5){
        oldfile = files - 5;
      } else {
        oldfile = 1;
      }
      break;
    }
  }

  server.on("/upload", HTTP_GET, []() {
    server.send(200, "text/html", upload_html);
  });
  server.on("/upload", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", redirect_html);
  }, handleFileUpload);
  server.on("/update", HTTP_GET, []() {
    server.send(200, "text/html", update_html);
  });
  server.on("/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "ERROR NOK" : "UPDATE OK! Rebooting!");
    delay(1000);
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.setDebugOutput(true);
      Serial.printf("Update: %s\n", upload.filename.c_str());
      uint32_t maxSketchSpace = (1310720 - 0x1000) & 0xFFFFF000;
      if (!Update.begin(maxSketchSpace)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
      Serial.setDebugOutput(false);
    }
    yield();
  });
  server.on("/reset", HTTP_GET, []() {
    server.send(200, "text/html", reset_html);
  });
  server.on("/readings", HTTP_GET, []() {
    server.send(200, "text/plain", header + tempdata);
  });
  server.on("/conf.htm", HTTP_POST, handleConfig);
  server.on("/down.htm", HTTP_GET, handleDownload);
  server.on("/reset.htm", HTTP_POST, handleReset);
  server.on("/data", HTTP_GET, handleData);
  server.on("/delete", HTTP_GET, handleDelete);
  server.onNotFound([]() {
    if (!handleFileRead(server.uri())) {
      handleNotFound();
    }
  });
  server.begin();
}
