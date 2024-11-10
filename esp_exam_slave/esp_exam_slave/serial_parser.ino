#include <FFat.h>

typedef enum {
  PROCESS_READING,
  SCRT_READING,
  EXAM_SUBMISSION,
  NONE
} SerialState;

SerialState serialState = NONE;

void writeToFile(const String &filename, const char *data, size_t len) {
  uint8_t data_u[600];
  for (int i = 0; i < len; i++) {
    data_u[i] = uint8_t(data[i]);
  }
  File uploadFile = FFat.open("/" + filename, FILE_APPEND);
  uploadFile.write(data_u, len);
  uploadFile.close();

  Serial.printf("Upload Complete: %s\n", filename.c_str());
}

void sendExamToHost() {
  File file = FFat.open("/exam.zip");

  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  if (file.size() == 0) {
    Serial.println("File is empty");
    file.close();
    return;
  }

  int bytesSent = 0;
  int totalBytes = file.size();
  Serial.print("Total size: ");
  Serial.println(totalBytes);
  byte buffer[128];
  Serial.println("EXDOW");
  delay(2000);
  while (file.available()) {
    int bytesRead = file.read(buffer, sizeof(buffer));
    bytesSent += bytesRead;
    for(int i = 0; i < bytesRead; i++) {
      Serial.print(int(buffer[i]));
      delay(10);
    }
  }
  delay(2000);
  Serial.println("EXEND");

  file.close();
  Serial.println("File upload complete");
}


void parse_serial() {
  if (Serial.available() > 0) {
    String message = "";
    while (Serial.available() > 0) {
      char incomingByte = Serial.read();
      message += incomingByte;
    }

    if (message.indexOf("PSTART") >= 0) {
      serialState = PROCESS_READING;
      File uploadFile = FFat.open("/processes_2024_3802.txt", FILE_WRITE);
      uploadFile.close();
    } else if (message.indexOf("PEND") >= 0) {
      Serial.println("Sending processes...");
      serialState = NONE;
      uploadProcessesToServer();
    } else if (serialState == PROCESS_READING) {
      writeToFile("processes_2024_3802.txt", message.c_str(), message.length());
    } else if (message.indexOf("SCSTART") >= 0) {
      serialState = SCRT_READING;
    } else if (message.indexOf("SCEND") >= 0) {
      serialState = NONE;
      sendMonitoring();
    } else if (serialState == SCRT_READING) {
      writeToFile("scrt_2024_3802.png", message.c_str(), message.length());
    } else if (message.indexOf("EXSUB") >= 0) {
      serialState = EXAM_SUBMISSION;
    } else if (message.indexOf("EXFIN") >= 0) {
      serialState = NONE;
      submitExam();
    } else if (serialState == EXAM_SUBMISSION) {
      writeToFile("exam_2024_3802.zip", message.c_str(), message.length());
    }
  }
}
