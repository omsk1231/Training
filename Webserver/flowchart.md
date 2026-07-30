## System Flowchart

```mermaid
flowchart TD

    A[ESP32 Power ON / Reset] --> B[Start app_main]

    B --> C[Initialize NVS Storage]

    C --> D[Read Saved Relay States]
    D --> E[Read Relay ON Counters]
    E --> F[Restore Relay and LED States]

    F --> G[Initialize GPIO]

    G --> H[GPIO5 and GPIO18 as Inputs]
    G --> I[GPIO14 and GPIO13 as Relay Outputs]
    G --> J[GPIO2 and GPIO15 as LED Outputs]

    H --> K[Initialize SCD41 CO2 Sensor]
    I --> K
    J --> K

    K --> L[Initialize I2C]
    L --> M[Start SCD41 Periodic Measurement]

    M --> N[Read Wi-Fi Credentials from NVS]

    N --> O{Saved Wi-Fi Available?}

    O -- Yes --> P[Use Saved SSID and Password]
    O -- No --> Q[Use Default Wi-Fi Credentials]
    Q --> R[Save Default Credentials to NVS]

    P --> S[Connect ESP32 to Wi-Fi]
    R --> S

    S --> T{Wi-Fi Connected?}

    T -- No --> U[Retry Wi-Fi Connection]
    U --> T

    T -- Yes --> V[ESP32 Gets IP Address]

    V --> W[Start HTTP Webserver]

    W --> X[Start GPIO Monitoring Task]

    X --> Y[System Running]

    Y --> Z1[Read CO2 Sensor]
    Y --> Z2[Monitor Physical GPIO Inputs]
    Y --> Z3[Handle Dashboard Requests]

    %% CO2 PATH

    Z1 --> A1{Valid CO2 Reading?}

    A1 -- Yes --> A2[Store Latest Valid CO2 Value]
    A1 -- No --> A3[Keep Previous Valid CO2 Value]

    A2 --> A4[Send CO2 Value to Dashboard]
    A3 --> A4

    A4 --> Y

    %% PHYSICAL GPIO PATH

    Z2 --> B1{GPIO5 HIGH to LOW?}

    B1 -- Yes --> B2[Toggle Control 1]
    B1 -- No --> B3{GPIO18 HIGH to LOW?}

    B2 --> B4{Control 1 State}

    B4 -- ON --> B5[GPIO14 LOW = Relay 1 ON]
    B4 -- ON --> B6[GPIO2 HIGH = LED 1 ON]

    B4 -- OFF --> B7[GPIO14 HIGH = Relay 1 OFF]
    B4 -- OFF --> B8[GPIO2 LOW = LED 1 OFF]

    B5 --> B9[Save Control 1 State in NVS]
    B6 --> B9
    B7 --> B9
    B8 --> B9

    B9 --> B10{OFF to ON Transition?}

    B10 -- Yes --> B11[Increment Relay 1 ON Counter]
    B10 -- No --> Y

    B11 --> B12[Store Counter in NVS]
    B12 --> Y

    B3 -- Yes --> C1[Toggle Control 2]
    B3 -- No --> Y

    C1 --> C2{Control 2 State}

    C2 -- ON --> C3[GPIO13 LOW = Relay 2 ON]
    C2 -- ON --> C4[GPIO15 HIGH = LED 2 ON]

    C2 -- OFF --> C5[GPIO13 HIGH = Relay 2 OFF]
    C2 -- OFF --> C6[GPIO15 LOW = LED 2 OFF]

    C3 --> C7[Save Control 2 State in NVS]
    C4 --> C7
    C5 --> C7
    C6 --> C7

    C7 --> C8{OFF to ON Transition?}

    C8 -- Yes --> C9[Increment Relay 2 ON Counter]
    C8 -- No --> Y

    C9 --> C10[Store Counter in NVS]
    C10 --> Y

    %% DASHBOARD PATH

    Z3 --> D1{Dashboard Request}

    D1 -- Status --> D2[Read CO2, States and Counters]
    D2 --> D3[Return JSON Status]
    D3 --> D4[Update Dashboard]
    D4 --> Y

    D1 -- Control 1 ON/OFF --> D5[Set Logical Control 1 State]
    D5 --> B4

    D1 -- Control 2 ON/OFF --> D6[Set Logical Control 2 State]
    D6 --> C2

    D1 -- New Wi-Fi --> E1[Receive New SSID and Password]
    E1 --> E2[Validate Credentials]
    E2 --> E3[Save Wi-Fi Credentials in NVS]
    E3 --> E4[Disconnect Current Wi-Fi]
    E4 --> E5[Apply New Wi-Fi Configuration]
    E5 --> E6[Connect to New Wi-Fi]
    E6 --> E7[Receive New IP Address]