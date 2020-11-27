#include <Arduino.h>

#include "libraries/timer.h"

#include "esp32-hal-cpu.h"

bool poll_timer() {
    if (rec.ready()) {
        Serial.print("DATA ");
        Serial.println(rec.duration());
      
        for(int j=0; j<rec.size(); ++j) {
            Serial.println(rec.get(j));
        }
        Serial.println("EOF");

        rec.end();
        return true;
    }
    return false;
}

void setup() {
    Serial.begin(115200);
    while (!Serial) {
        delay(10);
    }
    Serial.println("Boot!");
    Serial.print("Start in 2sec... ");
    delay(2000);
    Serial.println("now!");
    
    Serial.println("BEGIN");
    Serial.println(TIMER_COUNT);
    Serial.println(SAMPLING);
    Serial.println(BITRATE);
    Serial.println(NCHANNEL);
    Serial.println(BITDEPTH);
    Serial.println(getCpuFrequencyMhz());
    
    if (rec.begin(4.)) {
        Serial.println(rec.size());
        
        if (init_timer()) {
            Serial.println("START");
        } else {
            Serial.println("ERROR");
            Serial.println("TIMER RUNNING");
            Serial.println("EOF");
        }
    } else {
        Serial.println("ERROR");
        Serial.println("NOT ENOUGH MEMORY");
        Serial.println("EOF");
    }
}

long ts = 0;
void loop() {
    if ((millis()-ts)>2000) {
        poll_timer();
        ts = millis();
    }
    delay(100);
}
