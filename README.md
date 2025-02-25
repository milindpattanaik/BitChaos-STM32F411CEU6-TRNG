# **BitChaos - 9-bit True Random Number Generator (TRNG)**  

BitChaos is a **9-bit True Random Number Generator (TRNG)** implemented on an **STM32F411CEU6 (Black Pill)** using **STM32 HAL** with **CMake**. The generated random bits are transmitted over **USB Type-C** using **USB CDC (Virtual COM Port)** to a web application built with **Flask, HTML, and CSS**.

## **Hardware Overview**  
The circuit consists of:  
- **STM32F411CEU6 (Black Pill)**
- **2 × 74HC04 Hex Inverters**
- **Breadboard + Jumper Wires**
- **USB Type-C for Power & Communication**

### **Schematic** 
![TRNG Schematic 1](/readme-images/TRNGcircuitpicture.PNG) 

![TRNG Schematic 2](/readme-images/TRNGhardwarepicture.jpeg) 

## **Software Overview**  
### **Firmware (STM32)**
- Developed using **STM32 HAL**
- Built with **CMake**
- Generates 9-bit random numbers using **analog noise amplification**
- Transmits data over **USB CDC (Virtual COM Port)**  

### **Web Application**
- **Backend:** Flask (Handles serial communication)
- **Frontend:** HTML + CSS (Displays random data)
- Communicates with STM32 via **Serial (UART over USB CDC)**  
