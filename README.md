# 🎤 Voice-Controlled Home Automation & Security System

---

## 📌 Project Overview

This project was developed as part of the **Microprocessors (CPE-312) Laboratory**.  
The goal is to create a **voice-controlled system** that allows users to control household appliances and activate a **security alert** using a smartphone and **Bluetooth communication**.

The system uses a **Tiva C Series (TM4C123GH6PM)** microcontroller to process voice commands received from a mobile phone and convert them into real hardware actions.

---

## 🎯 Project Objectives

- Control household appliances using **voice commands**
- Implement a **security alert (Danger Mode)** for emergency situations
- Safely interface **low-power logic** with **high-current devices**
- Gain hands-on experience with:
  - Embedded C programming  
  - UART communication  
  - Relay interfacing  

---

## 🧠 System Architecture

### 🔹 Main Components

- **Microcontroller:** Tiva C Series (TM4C123GH6PM)  
- **Bluetooth Module:** HC-05  
- **Relay Module:** 5V Opto-Isolated Relay  
- **Actuator:** Fan (external load)  
- **Indicators:** Onboard RGB LED & Piezo Buzzer  
- **Power Source:** External 5V Battery (for high-current load)

---

## ⚙️ Hardware Design Concept

The system uses **dual power rails for safety**:

- **3.3V Logic Power:** Tiva C microcontroller  
- **5V External Power:** Relay coil and fan  

This design prevents **brownouts** and protects the microcontroller from the high current drawn by the fan.

---

## 🔄 Working Mechanism

### 🗣️ Voice Control Mode

1. User gives a voice command via mobile app (e.g., `"fan on"`)
2. Command is sent as a string via Bluetooth (**UART**)
3. Tiva C processes the command
4. If command matches:
   - **PA2** is set **HIGH**
   - Relay is activated
   - Fan turns **ON** using external power

---

### 🚨 Danger Mode (Security Feature)

Activated using the voice command: `"danger"`

Actions performed:

- 🔴 **Red LED (PF1)** blinks rapidly  
- 🔊 **Piezo buzzer (PA3)** beeps in sync  
- System enters a continuous alert loop to signal an emergency  

---

## 🔌 Pin Configuration

| Component       | Tiva C Pin | Function             |
|----------------|------------|----------------------|
| HC-05 RX       | PE4        | UART Receive         |
| HC-05 TX       | PE5        | UART Transmit        |
| Relay Module   | PA2        | Fan Control          |
| Buzzer         | PA3        | Audible Alert        |
| Red LED        | PF1        | Visual Alert         |
| Power          | VBUS (5V)  | Relay Coil Power     |

---

## 🛠️ Key Challenges & Solutions

- **Power Isolation:** Used an external battery to prevent system resets caused by high current draw.
- **Relay Interfacing:** Proper use of **NO (Normally Open)** and **COM** terminals for safe switching.
- **Timing Synchronization:** Software delays were carefully managed to synchronize LED blinking and buzzer beeping.

---

## 📚 Technologies Used

- Embedded C  
- TivaWare / TM4C Peripheral Libraries  
- UART Communication  
- Relay Interfacing  
- Bluetooth Communication (HC-05)

---

## 👥 Team Members

- Ayesha  
- Hadia  
- Khadijha  
- Laiba Khan  

---

## ✅ Conclusion

This project demonstrates a practical application of microprocessors and embedded systems by integrating voice commands with real-world hardware control.  
It enhanced our understanding of **hardware-software integration**, **power management**, and **teamwork** in embedded system design.

---

## 🔮 Future Improvements

- Add Wi-Fi / IoT support  
- Integrate motion or gas sensors  
- Control multiple appliances  
- Develop a dedicated mobile app  
- Implement feedback monitoring


## 👩‍💻 Author

**Layba Khan**  
Electrical Engineering Student  
Machine Learning Enthusiast

---

## ⭐ Support

If you like this project, give it a ⭐ on GitHub and feel free to fork or contribute!
