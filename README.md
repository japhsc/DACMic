# DACMic

## DAC mic recoder using ESP32

Record audio as raw Pulse-code modulation (PCM) and transfer via serial ``mic.ino``
⋅⋅⋅adjustable sampling rate etc in ``libraries/timer.h``
⋅⋅⋅record duration is very limited (several seconds, depends on sampling) due to limited RAM
⋅⋅⋅for 44100 Hz reduce ADC_AVG to 1 in ``libraries/recorder.h``, otherwise ESP will crash (timer interrupt takes too long)
⋅⋅⋅I used a MAX4465 microphone with fixed adjustable gain (no auto-gain), which I can NOT recommend (very noisy)

* Receive serial data via python with ``serial/serial_read.py``
* Analyse and received data and create wav file ``serial/serial_analyse.py``
* Create raw wav file via cpp ``wav_converter/main.cpp``

