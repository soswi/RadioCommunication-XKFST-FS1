ESP32-C3 Custom 433MHz Radio Communication Protocol
📡 Goal
This project aims to establish a robust, low-level radio communication protocol between two ESP32-C3 microcontrollers using simple 433 MHz ASK/OOK RF modules (FS1000A transmitter + MX-05V receiver). The primary focus is on creating a reliable synchronization and data framing mechanism from scratch, without relying on high-level libraries like VirtualWire or RadioHead.

🏛️ Core Architecture
The project is built upon a flexible and powerful event-driven architecture centered around a generic, template-based Finite State Machine (FSM).

Generic FSM: The StateMachine class is a reusable template that can manage any set of states defined by an enum class. This allows for creating multiple, independent FSMs within the project.

Nested State Machines: To manage complexity, the system uses a nested FSM approach. A main FSM (MasterStates) handles the top-level application flow (Idle, Sync), while the SyncState itself contains a dedicated sub-FSM (SyncStates) to manage the intricate steps of the synchronization protocol.

Task-Based Transitions: State transitions are event-driven, primarily by hardware interrupts. A transition can be accompanied by a "task" payload (std::any), which instructs the new state on how to initialize itself (e.g., whether to act as a synchronization INITIATOR or REQUESTER).

🤝 Custom Synchronization Protocol
A custom, two-way handshake protocol has been implemented to ensure both devices are precisely synchronized before any data is exchanged.

The protocol flow is as follows:

Initiation (by Initiator):

The Initiator (e.g., triggered by a button press) sends a long (15-20ms) initial pulse to "wake up" any listening devices.

Preamble for Clock Discovery (by Initiator):

Immediately following the initial pulse, the Initiator sends a burst of short, fixed-width pulses.

Measurement (by Receiver):

The Receiver, woken from its Idle state by an interrupt, detects the initial pulse.

It then measures the duration of the incoming preamble pulses to calculate the average pulse width, effectively discovering the Initiator's transmission speed.

Confirmation (by Receiver):

If the measurement is successful, the Receiver sends back its own long (20-25ms) confirmation pulse, signaling "I have your timing".

Final Trigger (by Initiator):

The Initiator detects the confirmation pulse and sends one final, short trigger pulse.

Synchronized Action:

Both the Initiator and Receiver receive the final trigger pulse. This pulse acts as a shared "starting gun".

Upon receiving this trigger, both devices start a precise, non-blocking hardware timer (esp_timer). When the timer fires, they execute a synchronized action (blinking an LED), visually confirming that their clocks are aligned.

🚀 How to Test
The system is designed to be triggered by real-world events:

Become a Receiver: An interrupt on RX_PIN (GPIO 4) is triggered by incoming radio signals, which automatically transitions the device into the Sync state with a REQUEST task.

Become an Initiator: An interrupt on BUTTON_PIN (GPIO 3) is triggered by a button press, which transitions the device into the Sync state with an INITIATE task.

To test, upload the code to two ESP32-C3 devices and press the button on one. You should see a sequence of log messages on the serial monitors, culminating in a simultaneous LED blink on both devices.

📁 Project Structure
src/: Main application source (.ino).

src/state/: Core, reusable FSM classes (StateMachine.h, State.h).

src/states/: Definitions for all concrete states and sub-states.

StateIds.h: Contains all enum definitions for state identifiers.

sync/SyncState.cpp: A consolidated file containing the logic for the Sync state and all its synchronization sub-states.

🔮 Future Work
With the synchronization protocol successfully implemented, the next step is to build out the TxState and RxState to handle the transmission and reception of actual data packets, including payload framing, CRC checksums, and an ACK/NACK mechanism.