#include <HTTPClient.h>
#include <FFat.h>

String serverURL = "http://192.168.4.1";

String urlEncode(const String &str) {
  String encoded = "";
  for (int i = 0; i < str.length(); i++) {
    char c = str.charAt(i);
    if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
      encoded += c;
    } 
    else if (c == ' ') {
      encoded += '+';
    } 
    else {
      encoded += '%';
      encoded += hexChar(c >> 4);
      encoded += hexChar(c & 0x0F);
    }
  }
  return encoded;
}

char hexChar(uint8_t nibble) {
  if (nibble < 10) return '0' + nibble;
  return 'A' + (nibble - 10);
}

void sendStudentLogin() {
  HTTPClient http;
  String serverPath = serverURL + "/login?s_id=" + urlEncode(studentInfo) + "&s_name=" + urlEncode(studentName) + "&mac=" + urlEncode(macAddr);
  Serial.println(serverPath);
  http.begin(serverPath.c_str());
  int httpResponseCode = http.GET();

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    String payload = http.getString();
    Serial.println(payload);
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

void uploadProcessesToServer() {
  File file = FFat.open("/processes_2024_3802.txt");
  String serverPath = serverURL + "/processes";

  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  if (file.size() == 0) {
    Serial.println("File is empty");
    file.close();
    return;
  }

  HTTPClient http;

  http.begin(serverPath);

  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  int bytesSent = 0;
  int totalBytes = file.size();
  byte buffer[1024];

  while (file.available()) {
    int bytesRead = file.read(buffer, sizeof(buffer));
    bytesSent += bytesRead;
    char code[1024] = {0};
    for(int i = 0; i < bytesRead; i++) {
      code[i] = (char)buffer[i];
    }
    String codeS = code;
    Serial.println(codeS);
    codeS.replace('\n', ';');
    codeS.replace(' ', '+');
    Serial.println(codeS);
    String s = "proc="+codeS+"&s_id=2024_3802";
    const char* postReq = s.c_str();
    uint8_t arr[1024] = {0};
    for(int i = 0; i < 1024; i++) {
      arr[i] = (uint8_t) postReq[i];
    }
    Serial.println(s);
    int httpResponseCode = http.POST(arr, s.length());
    Serial.print("Upload process response code: ");
    Serial.println(httpResponseCode);
  }

  file.close();
  http.end();

  Serial.println("File upload complete");
}

void uploadScreenshotToServer() {
  File file = FFat.open("/scrt_2024_3802.png");
  String serverPath = serverURL + "/upload";

  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  if (file.size() == 0) {
    Serial.println("File is empty");
    file.close();
    return;
  }

  HTTPClient http;

  http.begin(serverURL);

  http.addHeader("Content-Type", "application/octet-stream");

  int bytesSent = 0;
  int totalBytes = file.size();
  byte buffer[128];

  while (file.available()) {
    int bytesRead = file.read(buffer, sizeof(buffer));
    bytesSent += bytesRead;

    int httpResponseCode = http.POST(buffer, bytesRead);

    if (httpResponseCode > 0) {
      Serial.printf("Sent %d/%d bytes, Response code: %d\n", bytesSent, totalBytes, httpResponseCode);
    } else {
      Serial.printf("Failed to send data, HTTP code: %d\n", httpResponseCode);
    }
  }

  file.close();
  http.end();

  Serial.println("File upload complete");
}

void sendProcConfirm() {
  HTTPClient http;
  String serverPath = serverURL + "/proc?s_id=" + urlEncode(studentInfo);

  http.begin(serverPath.c_str());
  int httpResponseCode = http.GET();

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    String payload = http.getString();
    Serial.println(payload);
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

void sendMonitoring() {
  uploadProcessesToServer();
  uploadScreenshotToServer();
  sendProcConfirm();
}

void downloadExam() {
  HTTPClient http;
  String serverPath = serverURL + "/exam.zip";
  http.begin(serverPath);

  int httpCode = http.GET();

  if (httpCode > 0) {
    Serial.printf("HTTP GET request successful, code: %d\n", httpCode);

    int contentLength = http.getSize();
    if (contentLength > 0) {
      Serial.printf("File size: %d bytes\n", contentLength);

      File file = FFat.open("/exam.zip", "w");
      if (!file) {
        Serial.println("Failed to open file for writing");
        http.end();
        return;
      }

      WiFiClient *stream = http.getStreamPtr();
      byte buffer[128];
      int bytesRead = 0;

      while (http.connected() && (contentLength > 0 || contentLength == -1)) {
        int len = stream->available();
        if (len > 0) {
          bytesRead = stream->readBytes(buffer, sizeof(buffer));
          file.write(buffer, bytesRead);
          contentLength -= bytesRead;
          Serial.print(".");
        }
      }

      file.close();
      Serial.println("\nFile downloaded and saved to ffat card!");
    } else {
      Serial.println("Failed to get file size.");
    }
  } else {
    Serial.printf("HTTP GET request failed, code: %d\n", httpCode);
  }

  http.end();
}

void uploadExam() {
  File file = FFat.open("/exam_2024_3802.zip");
  String serverPath = serverURL + "/upload";

  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  if (file.size() == 0) {
    Serial.println("File is empty");
    file.close();
    return;
  }

  HTTPClient http;

  http.begin(serverURL);

  http.addHeader("Content-Type", "application/octet-stream");

  int bytesSent = 0;
  int totalBytes = file.size();
  byte buffer[128];

  while (file.available()) {
    int bytesRead = file.read(buffer, sizeof(buffer));
    bytesSent += bytesRead;

    int httpResponseCode = http.POST(buffer, bytesRead);

    if (httpResponseCode > 0) {
      Serial.printf("Sent %d/%d bytes, Response code: %d\n", bytesSent, totalBytes, httpResponseCode);
    } else {
      Serial.printf("Failed to send data, HTTP code: %d\n", httpResponseCode);
    }
  }

  file.close();
  http.end();

  Serial.println("File upload complete");
}

void confirmSubmission() {
  HTTPClient http;
  String serverPath = serverURL + "/submit?s_id=" + urlEncode(studentInfo);

  http.begin(serverPath.c_str());
  int httpResponseCode = http.GET();

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    String payload = http.getString();
    Serial.println(payload);
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

void submitExam() {
  uploadExam();
  confirmSubmission();
}

bool examExists() {
  HTTPClient http;
  String serverPath = serverURL + "/exists?file=" + "exam.zip";

  http.begin(serverPath.c_str());
  int httpResponseCode = http.GET();

  if (httpResponseCode == 200) {
    return true;
  } else {
    return false;
  }

  http.end();
}