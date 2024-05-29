# Habit Chair v1
<p align="center">
    <img src="/include/Habit%20Chair%20Logo.png" alt="logo" width="200"/>
</p>

A system to help correct the posture of a user using a series of force sensitive resistors

> [!NOTE]
> This system is just a prototype, performance may vary.

## How it works🤔
This project uses a ```ESP32-WROOM``` development board for its wifi and dual core capability

### Task seperation
```Core 1```:
- Collects sensor data and processes data.
- Determines the posture of the user
- Performs alerts using audio and a vibration motor

```Core 2```:
- Handles wifi connectivity
- Sends data over to the database

## Achievements🏆
This project has obtained several awards
- 2022 Tan Kah Kee Tan Lark Sye Young Inventors' Awards - Bronze 🥉
- IEM STEM Innovations Showcase 2022 - Consolation