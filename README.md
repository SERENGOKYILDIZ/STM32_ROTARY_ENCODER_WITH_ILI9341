# STM32 Rotary Encoder + ILI9341 TFT Display Project

This project is built on an STM32 microcontroller (tested with STM32F407VG), where a rotary encoder is used to control a counter. The counter value is displayed on an ILI9341 TFT LCD screen via SPI. The screen is updated periodically using TIM2 interrupts.

## 🧩 Features

- 🌀 **Rotary Encoder Input**: Read using GPIO polling method.
- 🖥️ **ILI9341 TFT LCD**: Controlled via SPI1.
- ⏱️ **Timer (TIM2)**: Generates periodic interrupt every 300ms.
- 🔢 **Counter Range**: 0 to 250 (adjustable).
- ⚙️ **STM32 HAL-based Project**: Written using STM32Cube HAL APIs.

## 📷 Hardware Connections

| Component        | STM32F4 Pin        |
|------------------|--------------------|
| Encoder A        | PA0                |
| Encoder B        | PA1                |

> **Note:** [For ILI9341 Configuration](https://github.com/SERENGOKYILDIZ/STM32_ILI9341_TFT_LCD/raw/main/docs/ILI9341.PDF)

## 🔧 Peripherals Used

- **SPI1** – Communication with ILI9341 display
- **TIM2** – Periodic update via interrupt
- **GPIO** – Rotary encoder input
- **HAL Delay / GetTick** – Basic timing and delay handling

## 📁 Folder Structure

```
/Core
│
├── ILI9341/
│       ├── Inc/
│       │   └──ILI9341.h
│       │
│       └── Src/
│           └──ILI9341.c
│
└── RotaryEncoder/
        ├── Inc/
        │   └──encoder.h
        │
        └── Src/
            └──encoder.c
```

## ⏱️ Timer Configuration

- **Prescaler**: 8399
- **Period**: 2999
- **Interrupt Rate**: ~300 ms

```c
HAL_TIM_Base_Start_IT(&htim2);
```

## 📟 Display Update Logic

The counter value is printed periodically via TIM2 callback:

```c
ILI9341_Printf(&tft_lcd, 10, 154, "count: %04d", count);
```

You can optionally clear the area before drawing:

```c
ILI9341_FillRect(&tft_lcd, 10, 154, 120, 24, ILI9341_YELLOW);
```

To reduce SPI workload, update only if the value has changed.

---
## 🔄 Encoder Logic

This project uses a mechanical incremental rotary encoder with two signal outputs: **Channel A** and **Channel B**. These outputs produce square wave signals with a phase offset (known as quadrature), which allows detection of both the **rotation direction** and **steps**.

### How It Works

- When rotating **clockwise**, Channel A leads Channel B.
- When rotating **counter-clockwise**, Channel B leads Channel A.
- By monitoring both channels and detecting their state transitions, the software can determine the rotation direction and adjust the count.

### Quadrature Transition Table (Simplified)

| Previous A | Previous B | Current A | Current B | Direction        |
|------------|------------|-----------|-----------|------------------|
| 0          | 0          | 1         | 0         | Clockwise (+1)   |
| 1          | 0          | 1         | 1         | Clockwise (+1)   |
| 1          | 1          | 0         | 1         | Clockwise (+1)   |
| 0          | 1          | 0         | 0         | Clockwise (+1)   |
| ...        | ...        | ...       | ...       | ...              |

> The reverse pattern corresponds to **counter-clockwise (-1)** rotation.

### Software Implementation

- Encoder signals (A and B) are connected to `PA0` and `PA1`.
- The encoder is read using a **polling method** in the main loop with 1ms delay.
- A custom `Rotary_Encoder` structure handles the logic, limiting the count between `0` and `250`.

### Example Code Snippet

```c
Encoder_Update(&encoder);
count = encoder.count;
```
---

## ⚠️ Notes

- Avoid writing to the screen too frequently; it may interfere with encoder polling performance.
- Ensure your display can handle 42 MHz SPI. If not, reduce to `SPI_BAUDRATEPRESCALER_4`.

## 📦 Development Environment

- STM32CubeIDE 1.x
- STM32 HAL Libraries
- STM32F407VG or compatible board
- ILI9341 SPI TFT display
- Rotary Encoder (without button)

## 👨‍💻 Author

**Semi Eren Gökyıldız**  
📍 Pamukkale University, Mechatronics Engineering  
🔗 [LinkedIn](https://www.linkedin.com/in/semi-eren-gokyildiz/)  
🐙 [GitHub: SERENGOKYILDIZ](https://github.com/SERENGOKYILDIZ)

---

## 🪛 To Do

- [ ] SPI DMA support for display performance
- [ ] Push-button encoder support
- [ ] Bar graph visualization of the count

---

## 📝 License

This project is licensed under the MIT License. See the `LICENSE` file for more information.
