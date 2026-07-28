# ESP32 Modbus RTU Voltmeter – Program Flowchart

```mermaid
flowchart TD

    A([ESP32 Start]) --> B[Create Modbus Data Structure]

    B --> C[Initialize Modbus Communication]

    C --> C1[Configure UART2]
    C1 --> C2[Set Baud Rate: 9600]
    C2 --> C3[Set Format: 8N1]
    C3 --> C4[Set TX GPIO17 and RX GPIO16]
    C4 --> D[Start Main Monitoring Loop]

    D --> E[Create Modbus RTU Request]

    E --> E1[Set Slave ID = 2]
    E1 --> E2[Set Function Code = 04]
    E2 --> E3[Set Start Address = 0]
    E3 --> E4[Request 16 Input Registers]
    E4 --> E5[Calculate and Add CRC16]

    E5 --> F[Send Request Through UART]

    F --> G[Onboard RS485 Transceiver]

    G --> H[Transmit Request Through A/B Lines]

    H --> I[Voltmeter Receives Modbus Request]

    I --> J[Voltmeter Sends Modbus Response]

    J --> K[ESP32 Receives Response]

    K --> L{Valid Response Length?}

    L -- No --> X[Report Modbus Read Failure]
    L -- Yes --> M{Correct Slave ID?}

    M -- No --> X
    M -- Yes --> N{Correct Function Code?}

    N -- No --> X
    N -- Yes --> O{Correct Byte Count?}

    O -- No --> X
    O -- Yes --> P{CRC Valid?}

    P -- No --> X
    P -- Yes --> Q[Extract 16-bit Registers]

    Q --> R[Store Registers 0 to 15]

    R --> S[Decode Register Pairs]

    S --> S1[Registers 0 + 1 → Value 1]
    S1 --> S2[Registers 2 + 3 → Value 2]
    S2 --> S3[Registers 4 + 5 → Value 3]
    S3 --> S4[Continue for Remaining Pairs]

    S4 --> T[Combine Two 16-bit Words]

    T --> U[Create 32-bit Bit Pattern]

    U --> V[Interpret 32-bit Pattern as IEEE-754 Float]

    V --> W[Print Raw Registers and Decoded Values]

    W --> Y[Wait 1 Second]

    X --> Y

    Y --> D
```

## Simplified Working Flow

```mermaid
flowchart LR

    A[ESP32] -->|UART TX/RX| B[Onboard RS485 Transceiver]

    B -->|A/B RS485| C[Voltmeter<br/>Slave ID 2]

    C -->|Modbus Response| B

    B --> A

    A --> D[Raw 16-bit Registers]

    D --> E[Combine Register Pairs]

    E --> F[32-bit Data]

    F --> G[Decode as Float]

    G --> H[Display Measurements]
```

## Register Decoding Flow

```mermaid
flowchart TD

    A[16 Input Registers] --> B[Register Pairing]

    B --> C1[Reg 0 + Reg 1]
    B --> C2[Reg 2 + Reg 3]
    B --> C3[Reg 4 + Reg 5]
    B --> C4[Reg 6 + Reg 7]
    B --> C5[Reg 8 + Reg 9]
    B --> C6[Reg 10 + Reg 11]
    B --> C7[Reg 12 + Reg 13]
    B --> C8[Reg 14 + Reg 15]

    C1 --> D1[Value 1]
    C2 --> D2[Value 2]
    C3 --> D3[Value 3]
    C4 --> D4[Value 4]
    C5 --> D5[Value 5]
    C6 --> D6[Value 6]
    C7 --> D7[Value 7]
    C8 --> D8[Value 8]
```

## Data Conversion

Each measurement is currently interpreted using two consecutive 16-bit registers:

```text
Register N        Register N+1
  16 bits            16 bits
     │                   │
     └────────┬──────────┘
              ↓
         Combine Words
              ↓
          32-bit Data
              ↓
       IEEE-754 Float
              ↓
        Decoded Value
```

The current implementation assumes the first register is the low word and the second register is the high word. This will be verified during hardware testing.