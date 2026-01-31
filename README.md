# AFDevice - ESP32-S3 Motor Controller

ESP32-S3 firmware for motor control device with WiFi configuration, MQTT broker, encoder reading, current sensing, and React web interface.

## Features

- **Motor Control**: PWM motor driver with configurable speed (-255 to 255)
- **Encoder Reading**: Position tracking via quadrature encoder
- **Current Sensing**: ADC-based current monitoring
- **WiFi Management**: Automatic reconnection, setup AP mode
- **MQTT Broker**: Built-in broker with telemetry publishing
- **Web Interface**: React-based configuration UI
- **Hardware Buttons**: Physical control with long-press setup mode

## Hardware Requirements

- ESP32-S3-DevKitC-1-N16R8 (16MB Flash, 8MB PSRAM)
- Motor driver (3-wire: PWM, DIR, EN)
- Quadrature encoder
- Current sensor (ADC)
- 3 push buttons

## Pinout

| Function | Pin |
|----------|-----|
| Motor PWM | 4 |
| Motor DIR | 5 |
| Motor EN | 6 |
| Encoder A | 15 |
| Encoder B | 16 |
| Button Up | 10 |
| Button Down | 11 |
| Button Setup | 12 |
| Current ADC | 7 |

## Quick Start

### 1. Initial Setup

Hold **Setup button** for 5 seconds to enter AP mode:
- Connect to WiFi: `AlexFil Developer`
- Open browser: http://192.168.4.1
- Configure your WiFi credentials

### 2. Build and Upload

```bash
# Build firmware
pio run

# Upload to device
pio run --target upload

# Upload web interface
pio run --target uploadfs

# Monitor serial output
pio device monitor --baud 115200
```

### 3. MQTT Control

```bash
# Start motor forward
mosquitto_pub -h hub.local -p 1883 -t hub/cmd/motor -m '{"action":"forward","speed":200}'

# Stop motor
mosquitto_pub -h hub.local -t hub/cmd/motor -m '{"action":"stop"}'

# Set speed via config
mosquitto_pub -h hub.local -p 1883 -t hub/cmd/config -m '{"param":"speed","value":69}'

# Subscribe to telemetry
mosquitto_sub -h hub.local -p 1883 -t hub/telemetry
```

## MQTT Topics

| Topic | Direction | Description |
|-------|-----------|-------------|
| `hub/cmd/motor` | In | Motor commands: forward/backward/stop/set |
| `hub/cmd/config` | In | Config commands: speed parameter |
| `hub/telemetry` | Out | Encoder, current, speed, WiFi status (1Hz) |
| `hub/status` | Out | Online/offline status |

## Project Structure

```
src/
├── app/App.h              # Main application coordinator
├── core/DeviceState.h     # Shared state structure
├── hardware/              # Hardware modules
│   ├── Buttons.h
│   ├── CurrentSensor.h
│   ├── EncoderReader.h
│   └── MotorController.h
└── network/               # Network services
    ├── MqttBroker.h
    ├── MqttController.h
    ├── WebServer.h
    └── WiFiManager.h
```

## Dependencies

### C++ Libraries
- WiFi (built-in)
- EncButton ^3.7.4
- GyverMotor ^4.2.2
- ESP32Encoder ^0.10.2
- ESPAsyncWebServer (GitHub)
- ArduinoJson ^7.4.2
- PicoMQTT ^1.3.0

### Frontend
- React 19 + TypeScript 5.9
- Vite 7
- Tailwind CSS 4
- shadcn/ui

## Architecture

**Event-Driven State Management:**
- All modules share `DeviceState` by reference
- Inputs (buttons, MQTT, web) → DeviceState → Outputs (motor)
- Async web server and MQTT broker
- Non-blocking main loop

**Loop Order:**
```
1. WiFi update (connection handling)
2. Web server (async, no-op)
3. MQTT broker (conditional on WiFi)
4. Buttons (input handling)
5. Encoder (sensor reading)
6. Current sensor (ADC)
7. Motor controller (output)
```

## Development

### Frontend Development

```bash
cd frontend
npm run dev        # Development server
npm run build      # Production build
npm run lint       # TypeScript linting
```

### Firmware Development

```bash
pio run            # Build
pio test           # Run unit tests
pio run --target clean  # Clean build
```

## Configuration

WiFi credentials are stored in ESP32 Preferences (flash memory). Device attempts to reconnect automatically every 5 seconds if connection is lost.

## Troubleshooting

**No serial output?**
- Use `Serial0` for ESP32-S3 with USB CDC

**MQTT not responding?**
- Check WiFi connection status
- Broker runs only when WiFi is connected

**Motor not moving?**
- Check button logic doesn't override MQTT commands
- Verify motor driver power supply

## License

MIT License - See LICENSE file for details
