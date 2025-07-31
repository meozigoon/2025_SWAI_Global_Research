```mermaid
%%{init: { "flowchart": { "rankDir": "TB", "nodeSpacing": 12, "rankSpacing": 30 } }}%%
flowchart TB
    %% ---------- 기본 노드 ----------
    Start["전원 ON / 리셋"]
    Setup["setup() 실행"]
    A1["Serial.begin(9600)"]
    A2["IrReceiver.begin(recvPin)"]
    A3["pinMode(12, OUTPUT)<br/>pinMode(13, OUTPUT)"]
    A4["MR.begin()<br/>ML.begin()"]
    A5["srv.attach(8)<br/>srv.write(0)"]
    Loop{{"loop() 반복"}}
    CheckIR{"IrReceiver.decode()?"}
    ReadIR["decodedRawData 읽기"]
    TogglePower{"Power 버튼?<br/>0xBA45FF00"}
    TP["isOn 값 토글"]
    AfterPower["다음 단계"]
    IsOn{"isOn == true?"}
    LEDoff["digitalWrite(12, LOW)"]
    LEDon["digitalWrite(12, HIGH)"]
    Switch{{"switch(decodedRawData)"}}
    Resume["IrReceiver.resume()"]

    %% ---------- 명령 처리 ----------
    subgraph Commands["명령 처리"]
        direction TB
        Case1["▶▶<br/>MR.forward(255)<br/>ML.forward(255)"]
        Case2["▶||<br/>MR.stop()<br/>ML.stop()"]
        Case3["◀◀<br/>MR.backward(255)<br/>ML.backward(255)"]
        Case4["▲<br/>MR.forward(255)<br/>ML.stop()"]
        Case5["▼<br/>MR.stop()<br/>ML.forward(255)"]
        Case6["EQ<br/>srv.write(130)<br/>digitalWrite(13, HIGH)"]
        Case7["ST/REPT<br/>srv.write(0)<br/>digitalWrite(13, LOW)"]
        Default["기타 입력: 동작 없음"]
    end

    %% ---------- 메인 흐름 ----------
    Start --> Setup --> A1 --> A2 --> A3 --> A4 --> A5 --> Loop
    Loop --> CheckIR
    CheckIR -- 아니오 --> Loop
    CheckIR -- 예 --> ReadIR --> TogglePower
    TogglePower -- 예 --> TP --> AfterPower
    TogglePower -- 아니오 --> AfterPower
    AfterPower --> IsOn
    IsOn -- 아니오 --> LEDoff --> Resume
    IsOn -- 예 --> LEDon --> Switch

    %% Switch 분기
    Switch --> Case1
    Switch --> Case2
    Switch --> Case3
    Switch --> Case4
    Switch --> Case5
    Switch --> Case6
    Switch --> Case7
    Switch --> Default

    %% 케이스 종료 → Resume → Loop
    Case1 --> Resume
    Case2 --> Resume
    Case3 --> Resume
    Case4 --> Resume
    Case5 --> Resume
    Case6 --> Resume
    Case7 --> Resume
    Default --> Resume

    Resume --> Loop
```
