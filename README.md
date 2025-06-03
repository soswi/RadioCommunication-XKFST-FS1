# 🧭 Instrukcja (PL)

Ten projekt to komunikacja radiowa między dwoma mikrokontrolerami ESP32-C3 za pomocą modułów 433 MHz (FS1000A i MX-05V). Kod jest pisany w Arduino IDE, a celem jest stworzenie solidnego, ramkowego protokołu z synchronizacją, CRC, potwierdzeniami i możliwością dzielenia wiadomości na pakiety.

Zostałeś przypisany do zadania związanego z tym projektem. ChatGPT wcześniej utworzył strukturę repozytorium i opisał założenia. Twoje zadanie zostanie zaraz określone. Wszystko — kod, komentarze, dokumentacja — piszemy **po angielsku**.

---

# ESP32 433MHz Radio Communication Protocol (EN)

## 📡 Goal

Establish a robust bit-level communication protocol between two ESP32-C3 microcontrollers using 433 MHz RF modules (FS1000A transmitter + MX-05V receiver). The protocol includes frame-based transmission, CRC validation, acknowledgment handling, and message fragmentation.

## 🧱 Protocol Features

- Bit-level TX/RX over GPIO
- Synchronization and handshake at startup
- Addressed packets with source/destination fields
- Multi-packet framing for long messages (fragmentation)
- CRC validation (CRC-8 or CRC-16)
- ACK/NACK response with retransmission logic
- Finite State Machine managing all communication stages

## 📦 Frame Structure

| Field      | Size (bytes) | Description                      |
|------------|--------------|----------------------------------|
| SYNC       | 1            | Synchronization byte             |
| SRC_ADDR   | 1            | Sender address                   |
| DST_ADDR   | 1            | Receiver address                 |
| TYPE       | 1            | Frame type (DATA, ACK, etc.)     |
| PKT_ID     | 1            | Packet index in multi-part frame |
| PKT_CNT    | 1            | Total number of packets          |
| LENGTH     | 1            | Length of PAYLOAD in bytes       |
| PAYLOAD    | up to 32     | Message fragment                 |
| CRC        | 1            | CRC checksum                     |
| ACK_FLAG   | 1            | Acknowledgment bit (1=ACK, 0=NACK) |

## 🔄 Communication Flow

### Sender
1. Perform clock synchronization
2. Initiate handshake
3. Fragment and send message frames
4. Wait for ACK after each packet
5. Retry or continue depending on ACK/NACK

### Receiver
1. Perform clock synchronization
2. Acknowledge handshake
3. Receive and buffer message fragments
4. Validate CRC, send ACK/NACK
5. Assemble complete message

## 🧩 Repository Structure

RadioCommunication-XKFST-FS1/
│
├── /docs/
│ └── protocol.md # Full protocol description
├── /receiver/
│ └── receiver.ino # Receiver logic
├── /transmitter/
│ └── transmitter.ino # Transmitter logic
├── /lib/
│ ├── crc_utils.h # CRC functions
│ ├── state_machine.h # Finite state machine
│ ├── radio_utils.h # Bit-level GPIO TX/RX
│ └── packet_utils.h # Message fragmenting/assembling
├── .gitignore
├── LICENSE
└── README.md # Project overview