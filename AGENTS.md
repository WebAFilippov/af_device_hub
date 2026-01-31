# AGENTS.md

## Project Overview

ESP32-S3 N16R8 firmware with React web interface. PlatformIO project for motor control device with WiFi configuration, encoder reading, current sensing, and MQTT broker.

## Build Commands

### Firmware (C++)
```bash
# Build firmware
pio run

# Upload firmware to device
pio run --target upload

# Upload filesystem image (LittleFS)
pio run --target uploadfs

# Clean build
pio run --target clean

# Monitor serial output
pio device monitor --baud 115200

# Run tests (PlatformIO Unity)
pio test

# Run specific test environment
pio test -e esp32-s3-devkitc-1-n16r8v
```

### Frontend (React + TypeScript)
```bash
cd frontend

# Development server
npm run dev

# Build for production
npm run build

# Lint TypeScript/React
npm run lint

# Preview production build
npm run preview
```

### Full Build (Firmware + Frontend)
```bash
# Build React app, copy to data/, build LittleFS image
pio run --target uploadfs
```

## Code Style Guidelines

### C++ (Firmware)

**File Organization:**
- Headers use `#pragma once`
- Implementation in .h files (header-only classes)
- Include order: Arduino/system libs → Third-party → Local (relative paths with `../`)

**Naming Conventions:**
- Classes: `PascalCase` (e.g., `MotorController`, `DeviceState`)
- Methods: `camelCase` (e.g., `begin()`, `update()`)
- Private members: camelCase, no underscore prefix
- Structs: `PascalCase` with public fields
- Constants: `UPPER_SNAKE_CASE` or `camelCase` for local
- Macros: `UPPER_SNAKE_CASE`

**Class Pattern:**
```cpp
#pragma once
#include <Library.h>
#include "../core/DeviceState.h"

class ComponentName
{
public:
    void begin();                    // Initialize hardware
    void update(DeviceState &state); // Main loop update

private:
    LibraryType instance{args};      // Braced initialization
};

void ComponentName::begin() { }
void ComponentName::update(DeviceState &state) { }
```

**Key Patterns:**
- All hardware classes have `begin()` and `update(DeviceState &state)` methods
- Use braced initialization: `GMotor2<DRIVER3WIRE> motor{4, 6, 5}`
- Pass `DeviceState` by reference for shared state
- Use `Serial0.println()` with `[TAG]` prefixes for logging
- Binary protocols use `#pragma pack(push, 1)` for packed structs
- Default member initialization in headers

**Error Handling:**
- Check hardware init with `if (!LittleFS.begin())` pattern
- Log errors to Serial immediately
- Use lambda callbacks for async web handlers

### TypeScript/React (Frontend)

**Naming Conventions:**
- Components: `PascalCase` files and exports
- Hooks: `camelCase` starting with `use`
- Types/Interfaces: `PascalCase` with `I` prefix optional
- API functions: `camelCase` descriptive verbs
- Utility functions: `camelCase`

**Component Pattern:**
```typescript
import { FC, useState, useEffect } from "react";
import { Component } from "@/components/ui/component";

interface ComponentProps {
  prop: string;
}

export const MyComponent: FC<ComponentProps> = ({ prop }) => {
  // Implementation
};
```

**Key Patterns:**
- Use TypeScript strict mode
- Prefer functional components with hooks
- Use `@/` path alias for imports from `src/`
- Use `cn()` utility for conditional Tailwind classes
- API calls in `src/api/` directory
- Zod schemas for form validation
- shadcn/ui component patterns (Radix + Tailwind)
- Russian UI text is acceptable (device is for Russian market)

**Styling:**
- Tailwind CSS v4 with `@tailwindcss/vite`
- Use `class-variance-authority` for component variants
- Color tokens: `bg-background`, `text-foreground`, `text-muted-foreground`
- Responsive design with container queries

**API Calls:**
- Use `AbortController` for timeouts
- Return typed promises
- Handle errors with descriptive messages
- Poll with `setInterval`, cleanup in `useEffect` return

## Project Structure

```
AFDevice/
├── src/                    # C++ firmware source
│   ├── main.cpp           # Entry point (delegates to App)
│   ├── app/               # Application coordinator
│   │   └── App.h          # Main app class, initializes all modules
│   ├── core/              # Core types and state
│   │   ├── DeviceState.h  # Shared state struct
│   │   └── Config.h       # Centralized configuration constants (pins, intervals, limits)
│   ├── hardware/          # Hardware abstraction
│   │   ├── MotorController.h   # Motor control (GyverMotor2)
│   │   ├── EncoderReader.h     # Encoder reading (ESP32Encoder)
│   │   ├── Buttons.h           # Button handling (EncButton)
│   │   └── CurrentSensor.h     # Current sensing (ADC)
│   ├── network/           # Network services
│   │   ├── WebServer.h    # HTTP server (ESPAsyncWebServer)
│   │   ├── WiFiManager.h  # WiFi configuration with auto-reconnect
│   │   ├── MqttBroker.h   # MQTT broker (PicoMQTT)
│   │   └── MqttController.h # MQTT message handler
│   └── protocol/          # Binary protocols
│       └── Packets.h      # Data packet structures
├── frontend/              # React web interface
│   ├── src/
│   │   ├── app/           # Main app component
│   │   ├── components/    # React components
│   │   │   ├── ui/       # shadcn/ui components
│   │   │   └── *.tsx     # Feature components
│   │   ├── api/           # API client functions
│   │   ├── lib/           # Utilities and schemas
│   │   └── main.tsx       # Entry point
│   ├── package.json
│   ├── vite.config.ts     # Vite + compression config
│   └── tsconfig.json
├── data/                  # LittleFS files (auto-generated)
├── test/                  # PlatformIO tests
├── platformio.ini         # PlatformIO configuration
└── extra_script.py        # Pre-build script (builds React)
```

## Hardware Target

- **Board**: ESP32-S3-DevKitC-1-N16R8 (16MB Flash, 8MB PSRAM)
- **Framework**: Arduino
- **Filesystem**: LittleFS
- **Flash Mode**: QIO 80MHz
- **Upload Speed**: 115200 baud

## Pinout

- **Motor**: Pin 4 (PWM), 5 (DIR), 6 (EN)
- **Encoder**: Pin 15 (A), 16 (B)
- **Buttons**: Pin 10 (Up), 11 (Down), 12 (Setup)
- **Current Sensor**: Pin 7 (ADC)

## Dependencies

### C++ Libraries
- `WiFi` (built-in)
- `gyverlibs/EncButton` ^3.7.4
- `gyverlibs/GyverMotor` ^4.2.2
- `madhephaestus/ESP32Encoder` ^0.10.2
- `mathieucarbou/AsyncTCP` (GitHub)
- `mathieucarbou/ESPAsyncWebServer` (GitHub)
- `bblanchon/ArduinoJson` ^7.4.2
- `mlesniew/PicoMQTT` ^1.3.0

### NPM Packages
- React 19 + TypeScript 5.9
- Vite 7 + @vitejs/plugin-react
- Tailwind CSS 4 + @tailwindcss/vite
- shadcn/ui (Radix UI + class-variance-authority)
- Zod for validation
- lucide-react for icons

## MQTT Topics

- `hub/cmd/motor` - Motor commands (forward/backward/stop/set with speed)
- `hub/cmd/config` - Configuration commands (e.g., speed parameter)
- `hub/telemetry` - Telemetry data (encoder, current, motorSpeed, wifiConnected)
- `hub/status` - Device status (online/offline)

## WiFi Modes

1. **STA Mode**: Connects to saved WiFi network, auto-reconnects every 5 seconds
2. **Setup Mode**: AP mode activated by holding Setup button for 5 seconds
   - AP Name: "AlexFil Developer"
   - IP: 192.168.4.1
   - Web interface available for WiFi configuration

## Configuration

All constants centralized in `src/core/Config.h` using namespace pattern:

```cpp
#include "../core/Config.h"

// Usage: Config::Pins::MOTOR_PWM, Config::Mqtt::PORT, etc.
```

**Namespaces:**
- `Config::Pins` - GPIO pin numbers
- `Config::Motor` - Speed limits, min duty
- `Config::WiFi` - AP/STA settings, timeouts
- `Config::Mqtt` - Broker port, topics, intervals
- `Config::Debug` - Logging settings, prefixes
- `Config::System` - Stack sizes, queue sizes

Change values in Config.h to customize without modifying module code.

## Important Notes

- The frontend is auto-built and compressed during `uploadfs` target
- Gzipped files are served with `Content-Encoding: gzip` header
- Device acts as AP (192.168.4.1) for initial configuration
- MQTT broker runs only when WiFi is connected to save CPU
- All hardware modules update via `DeviceState` reference
- Serial monitor at 115200 baud for debugging (use Serial0 for ESP32-S3)
- Russian language acceptable for UI and comments
- WiFi auto-reconnect: 15s timeout, 5s delay between retries
- MQTT telemetry publishes every 1 second
- mDNS service advertised at hub.local:1883

## Architecture Pattern

```cpp
// Main loop execution order (App.loop):
1. wifi.update(state)     // Network connection handling
2. web.update(state)      // Async web server (no-op for ESPAsyncWebServer)
3. mqtt.update(state)     // MQTT broker (conditional on WiFi)
4. buttons.update(state)  // Input handling
5. encoder.update(state)  // Sensor reading
6. current.update(state)  // ADC reading
7. motor.update(state)    // Output control
```

## Event Flow

```
[Buttons/MQTT/Web] → DeviceState → [Motor/LED/Display]
     ↓                      ↑
[Hardware Events]      [State Changes]
```

All modules read/write to shared DeviceState. Motor controller reads motorSpeed from state and applies it to hardware. Buttons and MQTT write to motorSpeed based on user input.
