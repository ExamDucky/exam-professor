#include <WiFi.h>
#include <FFat.h>  

const char* ssid     = "test-ssid";
const char* password = "test-password";
String studentName = "Stefan Jovanovic";
String studentInfo = "2024_3802";
String macAddr;

bool examDownloaded = false;
int time_since_processes = 0;

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());

  if (!FFat.begin(true)) {
    Serial.println("FatFS Mount Failed");
    return;
  }

  uint32_t totalBytes = FFat.totalBytes();
  uint32_t usedBytes = FFat.usedBytes();
  Serial.print("Total space: ");
  Serial.println(totalBytes);
  Serial.print("Used space: ");
  Serial.println(usedBytes);
  
  macAddr = WiFi.macAddress();
  FFat.remove("/exam.zip");
  FFat.remove("/exam2.zip");
  FFat.remove("/exam3.zip");
  FFat.remove("/exam4.zip");
  FFat.remove("/exam5.zip");
  // writeToFile("exam5.zip", "abc", 3);

  sendStudentLogin();
  uploadProcessesToServer();

}

void loop() {

  if(!examDownloaded && examExists()) {
    downloadExam();
    sendExamToHost();
    examDownloaded = true;
  }
  
  if(millis() - time_since_processes >= 60*1000){
    time_since_processes = millis();
    Serial.println("PROCGET");
  }
  parse_serial();
}
