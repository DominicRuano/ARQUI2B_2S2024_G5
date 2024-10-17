#### Suscripciones ( arduino -> mqtt -> frontend(graficas) )
> `F3G5/humedad`
> `F3G5/temperatura`
> `F3G5/proximidad`
> `F3G5/calidadAire`
> `F3G5/luminosidad`
> `F3G5/Accesos`

#### Publicaciones ( frontend(Monitoreo) -> mqtt -> arduino )
> `F3G5/Control`
```json
{"ControlServo":90}
```

> `F3G5/LedControl`
```json
{"LedState":1}
{"LedState":0}
```

> `F3G5/RFID`  
#### ( frontend(Dashboard) -> mqtt -> arduino , arduino RFID)
```json
{"NUID":"0x39,0x09,0xFB,0xB3"}
{"NUID":"0x19,0x7D,0xD1,0xB1"}
```

####   ( frontend(Pronostico) -> mqtt -> backend)
> `F3G5/Pronostico`
```json
{"dia":"1","sensor":"mq"}
{"dia":"3","sensor":"dht"}
{"dia":"4","sensor":"ldr"}
```

#### ( backend py -> mqtt -> frontend(Pronostico) )
> `F3G5/Pronostico/Resultado`
```json
{
  "sensor": "mq",
  "valor": { "gas": 150 }
}

{
  "sensor": "dht",
  "valor": { "temperatura": 28, "humedad": 60 }
}

{
  "sensor": "ldr",
  "valor": { "luminosidad": 600 }
}
```


> FRONTEND
> proyecto > frontend > app >npm i> npm run dev

> BACKEND ( Falta la parte del backend del pronostico py )
> proyecto > backend > npm i > node server.js
