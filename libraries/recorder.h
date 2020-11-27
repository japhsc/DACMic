#ifndef RECORDER_H
#define  RECORDER_H

#include <driver/adc.h>

#include <soc/sens_reg.h>
#include <soc/sens_struct.h>

/*
    240000000 Hz / 44100 Hz = 5442 count cycles
    Max ADC sampling: 200 ksps = 200 kHz
*/

#define ADC_AVG 4

int IRAM_ATTR local_adc1_read(const int channel) {
    uint16_t adc_value;
    SENS.sar_meas_start1.sar1_en_pad = (1 << channel); // only one channel is selected
    while (SENS.sar_slave_addr1.meas_status != 0);
    SENS.sar_meas_start1.meas1_start_sar = 0;
    SENS.sar_meas_start1.meas1_start_sar = 1;
    while (SENS.sar_meas_start1.meas1_done_sar == 0);
    adc_value = SENS.sar_meas_start1.meas1_data_sar;
    return adc_value;
};

class Recorder {
    public:
        virtual bool begin(double duration) = 0;
        virtual void end() = 0;
        virtual bool clock() = 0;
        virtual bool ready() = 0;
};

template <class T>
class ByteRecorder : public Recorder {
    private:
        volatile int i = 0;
        volatile long N = 0;
        volatile long dur;
        
        const adc1_channel_t _adc_channel;
        const long _sampling;
        const int _offset;
        const adc_bits_width_t _adc_depth;
        
        T* data = NULL;

    public:
        ByteRecorder(
                    const adc1_channel_t adc_channel,
                    const long sampling,
                    const int offset,
                    const adc_bits_width_t adc_depth) : 
                    _adc_channel(adc_channel),
                    _sampling(sampling),
                    _offset(offset),
                    _adc_depth(adc_depth)
        {
            adc1_config_width(_adc_depth);
            //adc1_config_channel_atten(_adc_channel, ADC_ATTEN_MAX); //ADC_ATTEN_DB_0
        };

        bool begin(double duration) {
            if (!data) {
                i = 0;
                N = long(_sampling*duration);
                data = (T*) malloc(N*sizeof(T));
                if (data) {
                    adc1_get_raw(_adc_channel);
                    dur = millis();
                    return true;
                }
            }
            N = 0;
            return false;
        };

        void end() {
            ++i;
            if (data) {
                free(data);
                data = NULL;
            }
        };
        
        T clamp(const int &value) {
            return (value>255?255:(value<0?0:value));
        }
        
        T measure() {
            #if ADC_AVG
            int value = 0;
            for (int i=0; i<ADC_AVG; ++i) 
                value += local_adc1_read(_adc_channel);
            value /= ADC_AVG;
            value += _offset;
            return clamp(value);
            #elif
            return clamp(local_adc1_read(_adc_channel)+_offset);
            #endif
        }

        bool clock() {
            if (i<N) {
                data[i++] = measure();
                return false;
            } else {
                dur = millis()-dur;
                return true;
            }
        };

        bool ready() {
            return (i==N && data);
        };
        
        long duration() {
            return dur;
        };
        
        size_t size() {
            return N;
        };
        
        T get(size_t i) {
            return data[i];
        };
};

class StreamRecorder : public Recorder {
    private:
        volatile int i = 0;
        volatile long N = 0;
        volatile long dur;
        
        const adc1_channel_t _adc_channel;
        const long _sampling;
        const int _offset;
        const adc_bits_width_t _adc_depth;

    public:
    
        StreamRecorder(
                    const adc1_channel_t adc_channel,
                    const long sampling,
                    const int offset,
                    const adc_bits_width_t adc_depth) : 
                    _adc_channel(adc_channel),
                    _sampling(sampling),
                    _offset(offset),
                    _adc_depth(adc_depth)
        {
            adc1_config_width(_adc_depth);
            //adc1_config_channel_atten(_adc_channel, ADC_ATTEN_MAX); //ADC_ATTEN_DB_0
        };

        bool begin(double duration) {
            i = 0;
            N = long(_sampling*duration);
            adc1_get_raw(_adc_channel);
            dur = millis();
            return true;
        };

        void end() {
            ++i;
        };

        bool clock() {
            if (i<N) {
                uint8_t value = (local_adc1_read(_adc_channel)+_offset);
                Serial.println(value);
                return false;
            } else {
                dur = millis()-dur;
                return true;
            }
        };

        bool ready() {
            return (i==N);
        };
        
        long duration() {
            return dur;
        };
        
        size_t size() {
            return N;
        };
        
        uint8_t get(size_t i) {
            return 0;
        };
};

#endif
