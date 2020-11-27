#ifndef TIMER_H
#define  TIMER_H

#include "recorder.h"

#define ADC_CHANNEL ADC1_CHANNEL_0      // GPIO36
#define ADC_DEPTH   ADC_WIDTH_BIT_12    // ADC_WIDTH_BIT_9..12

#define SAMPL_FREQY 8000                // Target: 44100, 8000
#define TIMER_DIVDR 10
#define TIMER_CLOCK 80000000
#define TIMER_COUNT int(round(TIMER_CLOCK/(SAMPL_FREQY*TIMER_DIVDR)))

#define SAMPLING    TIMER_CLOCK/(TIMER_COUNT*TIMER_DIVDR)
#define NCHANNEL    1
#define BITDEPTH    8
#define BITRATE     SAMPLING*BITDEPTH*NCHANNEL

hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

ByteRecorder<uint8_t> rec(ADC_CHANNEL, SAMPLING, -1882+127, ADC_DEPTH);
//StreamRecorder rec(ADC_CHANNEL, SAMPLING, -1837+127, ADC_DEPTH);

void IRAM_ATTR finish_timer() {
    timerAlarmDisable(timer);
    timerEnd(timer);
    timer = NULL;
}

void IRAM_ATTR onTimer() {
    portENTER_CRITICAL_ISR(&timerMux);
    if (rec.clock()) {
        finish_timer();
    }
    portEXIT_CRITICAL_ISR(&timerMux);
}

bool init_timer() {
    if (!timer) {
        timer = timerBegin(0, TIMER_DIVDR, true);
        timerAttachInterrupt(timer, &onTimer, true);
        timerAlarmWrite(timer, TIMER_COUNT, true);
        timerAlarmEnable(timer);
        return true;
    }
    return false;
}

#endif
