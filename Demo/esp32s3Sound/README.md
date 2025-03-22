- esp32s3SoundtoPython：烧录程序并启动py代码，可以听到电脑在播放ESP32收到的声音。

- esp32s3VoiceWakeUp：烧录程序并对其说“嗨乐鑫”或“小爱同学”监视器上会打印出识别到的模型序号。

  ```txt
  需要配置以下内容，开始对应唤醒词模型
  CONFIG_SR_WN_WN9_HILEXIN=y
  CONFIG_SR_WN_WN9_XIAOAITONGXUE=y
  # CONFIG_SR_WN_WN9_NIHAOXIAOZHI_TTS is not set
  # CONFIG_SR_WN_WN9_ALEXA is not set
  # CONFIG_SR_WN_WN9_HIESP is not set
  # CONFIG_SR_WN_WN9_JARVIS_TTS is not set
  # CONFIG_SR_WN_WN9_COMPUTER_TTS is not set
  # CONFIG_SR_WN_WN9_HEYWILLOW_TTS is not set
  # CONFIG_SR_WN_WN9_SOPHIA_TTS is not set
  # CONFIG_SR_WN_WN9_NIHAOXIAOXIN_TTS is not set
  # CONFIG_SR_WN_WN9_XIAOMEITONGXUE_TTS is not set
  # CONFIG_SR_WN_WN9_HEYPRINTER_TTS is not set
  # CONFIG_SR_WN_WN9_XIAOLONGXIAOLONG_TTS is not set
  # CONFIG_SR_WN_WN9_MIAOMIAOTONGXUE_TTS is not set
  # CONFIG_SR_WN_WN9_HEYWANDA_TTS is not set
  # CONFIG_SR_WN_WN9_HIMIAOMIAO_TTS is not set
  # CONFIG_SR_WN_WN9_MYCROFT_TTS is not set
  # CONFIG_SR_WN_WN9_HIJOY_TTS is not set
  # CONFIG_SR_WN_WN9_HILILI_TTS is not set
  # CONFIG_SR_WN_WN9_HITELLY_TTS is not set
  # CONFIG_SR_WN_WN9_XIAOBINXIAOBIN_TTS is not set
  # CONFIG_SR_WN_WN9_HAIXIAOWU_TTS is not set
  # CONFIG_SR_WN_WN9_ASTROLABE_TTS is not set
  # CONFIG_SR_WN_WN9_XIAOYAXIAOYA_TTS2 is not set
  # CONFIG_SR_WN_WN9_HIJASON_TTS2 is not set
  # CONFIG_SR_WN_WN9_LINAIBAN_TTS2 is not set
  # CONFIG_SR_WN_WN9_XIAOSUROU_TTS2 is not set
  # CONFIG_SR_WN_WN9_XIAOYUTONGXUE_TTS2 is not set
  # CONFIG_SR_WN_WN9_XIAOMINGTONGXUE_TTS2 is not set
  # end of Load Multiple Wake Words
  ```

- esp32s3VoiceWakeWord：

  ```txt
  需要配置以下内容，配置ming'ling'sh
  # CONFIG_SR_MN_CN_NONE is not set
  # CONFIG_SR_MN_CN_MULTINET5_RECOGNITION_QUANT8 is not set
  # CONFIG_SR_MN_CN_MULTINET6_QUANT is not set
  # CONFIG_SR_MN_CN_MULTINET6_AC_QUANT is not set
  # CONFIG_SR_MN_CN_MULTINET7_QUANT is not set
  CONFIG_SR_MN_CN_MULTINET7_AC_QUANT=y
  CONFIG_SR_MN_EN_NONE=y
  # CONFIG_SR_MN_EN_MULTINET5_SINGLE_RECOGNITION_QUANT8 is not set
  # CONFIG_SR_MN_EN_MULTINET6_QUANT is not set
  # CONFIG_SR_MN_EN_MULTINET7_QUANT is not set
  # end of ESP Speech Recognition
  ```

- esp32s3SenseVoice

  配合 [FunAudioLLM/SenseVoice: Multilingual Voice Understanding Model](https://github.com/FunAudioLLM/SenseVoice) 项目实现语音转文字

  或使用 [0x5446/api4sensevoice: API and websocket server for sensevoice. It has inherited some enhanced features, such as VAD detection, real-time streaming recognition, and speaker verification.](https://github.com/0x5446/api4sensevoice)

