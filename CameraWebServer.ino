// 카메라 관련 기능을 사용하기 위한 헤더 파일 포함
#include "esp_camera.h"

// WiFi 기능을 사용하기 위한 헤더 파일 포함
#include <WiFi.h>

// AI Thinker 모델에 해당하는 핀 설정을 적용 (PSRAM 포함)
#define CAMERA_MODEL_AI_THINKER
#include "camera_pins.h"

// ===========================
// WiFi 접속 정보 입력 (자신의 네트워크 정보로 변경)
// ===========================
const char *ssid = "**********";      // WiFi 네트워크 이름(SSID)
const char *password = "**********";  // WiFi 비밀번호

// 카메라 서버 시작 함수 선언 (구현은 별도)
void startCameraServer();

// LED 플래시 제어를 위한 초기화 함수 선언 (구현은 별도)
void setupLedFlash(int pin);

void setup() {
  // 시리얼 통신 시작 - 디버깅 메시지 출력을 위해 115200 baudrate 사용
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  // 카메라 설정을 위한 구조체 변수 생성
  camera_config_t config;
  // PWM 제어를 위한 채널 및 타이머 설정
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  
  // 카메라 데이터 핀 설정 (camera_pins.h에 정의된 핀 번호 사용)
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  // 전원 제어 핀 설정 (카메라의 전원 및 리셋 제어)
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  
  // XCLK 주파수 설정 (20MHz)
  config.xclk_freq_hz = 20000000;
  // 기본 프레임 크기를 VGA로 설정
  config.frame_size = FRAMESIZE_VGA;
  // 스트리밍용 JPEG 포맷 사용 (얼굴 인식 등 다른 기능은 RGB565 사용 가능)
  config.pixel_format = PIXFORMAT_JPEG;  // 스트리밍 전용
  // config.pixel_format = PIXFORMAT_RGB565; // 얼굴 검출/인식을 위한 설정 예시
  
  // 프레임 버퍼 획득 방식 설정: 빈 버퍼가 있을 때 가져옴
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  // 프레임 버퍼를 PSRAM에 할당 (PSRAM이 없는 경우 DRAM 사용 고려)
  config.fb_location = CAMERA_FB_IN_PSRAM;
  // JPEG 압축 품질 (수치가 낮을수록 고화질)
  config.jpeg_quality = 12;
  // 프레임 버퍼 개수
  config.fb_count = 1;

  // 만약 PSRAM이 있다면, 해상도와 JPEG 품질을 조정하여 프레임 버퍼 용량을 증가시킴
  if (config.pixel_format == PIXFORMAT_JPEG) {
    if (psramFound()) {
      config.jpeg_quality = 10;      // JPEG 품질 향상
      config.fb_count = 2;           // 프레임 버퍼 2개 사용
      config.grab_mode = CAMERA_GRAB_LATEST;  // 최신 프레임 우선
    } else {
      // PSRAM이 없을 경우 프레임 크기를 제한하고 DRAM에 프레임 버퍼 할당
      config.frame_size = FRAMESIZE_SVGA;
      config.fb_location = CAMERA_FB_IN_DRAM;
    }
  } else {
    // 얼굴 인식용으로 사용할 경우 최적의 설정 (해상도를 240x240으로 설정)
    config.frame_size = FRAMESIZE_240X240;
#if CONFIG_IDF_TARGET_ESP32S3
    config.fb_count = 2;
#endif
  }

  // ESP-EYE 모델인 경우, 특정 핀의 내부 풀업(Pull-up) 설정
#if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
#endif

  // 카메라 초기화
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    // 초기화 실패 시 에러 코드 출력 후 함수 종료
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  // 카메라 센서 정보 획득
  sensor_t *s = esp_camera_sensor_get();
  
  // JPEG 포맷일 경우 초기 프레임 속도를 높이기 위해 해상도를 QVGA(320x240)로 낮춤
  if (config.pixel_format == PIXFORMAT_JPEG) {
    s->set_framesize(s, FRAMESIZE_QVGA);
  }

  // camera_pins.h에 LED 관련 핀 번호가 정의되어 있다면 LED 플래시 설정 함수 호출
#if defined(LED_GPIO_NUM)
  setupLedFlash(LED_GPIO_NUM);
#endif

  // WiFi 연결 시작
  WiFi.begin(ssid, password);
  // WiFi 슬립 모드 비활성화 (연결 안정성 향상)
  WiFi.setSleep(false);

  // WiFi 연결 상태를 확인하며 연결될 때까지 대기
  Serial.print("WiFi connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // 카메라 서버 실행 함수 호출 (웹 인터페이스 등)
  startCameraServer();

  // 연결된 IP 주소를 출력하여 접속 방법 안내
  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");
}

void loop() {
  // 메인 루프에서는 아무 작업도 하지 않음.
  // 모든 기능은 웹 서버에서 다른 태스크로 처리됨
  delay(10000);
}
