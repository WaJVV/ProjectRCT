
# Proyecto Reloj y Alarma con Arduino Mega

**Grupo 6** – Sistema de reloj en tiempo real (RTC) y alarma, programado en Arduino IDE (C++) y simulado en Proteus.

---

## Descripción

Este proyecto implementa un reloj digital con función de alarma en una placa Arduino Mega. Mediante una simulación en Proteus, se integran los siguientes componentes: Arduino Mega, pulsadores, resistencias, pantalla LCD 16×2 y buzzer piezoeléctrico. Permite:

- Visualizar y contar segundos, minutos y horas en tiempo real.
- Editar la fecha (día, mes, año) y hora actual con pulsadores.
- Configurar una alarma especificando día, mes, año, hora, minutos y segundos.
- Seleccionar diferentes tonos para el buzzer.
- Detener la alarma con un botón dedicado.

---

## Características principales

1. **Modo Reloj**  
   - Tiempo real mostrado en formato `HH:MM:SS`.  
   - Actualización continua mediante `millis()`.

2. **Modo Edición**  
   - Botón IZQUIERDA (`LEFT`) para entrar/salir de edición.  
   - Botones ARRIBA (`UP`)/ABAJO (`DOWN`) para ajustar valores.

3. **Configuración de Alarma**  
   - Definición completa de fecha y hora de la alarma.  
   - Indicador LED en pin A7 cuando la alarma está activa.

4. **Selector de Tonos**  
   - Botón `SELECT TONE` para recorrer melodías predeterminadas.

5. **Detención de Alarma**  
   - Botón `STOP ALARM` para silenciar el buzzer y apagar el LED.

6. **Interfaz de Usuario**  
   - Pantalla LCD 16×2 conectada a los pines 23, 22, 24, 25, 26 y 27.  
   - Mensajes en pantalla que indican el modo y los valores editados.

---

## Diagrama de flujo y simulación

- **Diagrama de flujo**: `DiagramaDeFlujo.drawio` / `DiagramaDeFlujo.drawio.png`  
- **Proyecto Proteus**: `ProyectoFinal.pdsprj`

---

## Estructura de archivos

```plaintext
/Grupo6_ProyectoFinal
│
├─ Grupo6_ProyectoFinal.docx       # Documentación técnica
├─ ProyectoFinal.pdsprj             # Proyecto Proteus
├─ DiagramaDeFlujo.drawio          # Diagrama de flujo editable
├─ DiagramaDeFlujo.drawio.png      # Imagen del diagrama de flujo
└─ ProyectoParadigmasCode/
   ├─ ProyectoParadigmasCode.ino    # Código fuente Arduino IDE
   └─ build/                        # Binarios compilados (.hex, .elf)
```

---

## Requisitos

- **Hardware**:
  - Arduino Mega
  - LCD 16×2
  - Pulsadores (UP, DOWN, LEFT, RIGHT, CENTER, SELECT TONE, STOP ALARM)
  - Buzzer piezoeléctrico
  - Resistencias

- **Software**:
  - Arduino IDE (versión ≥ 1.8.0)
  - Proteus (versión ≥ 8.0)

- **Librerías Arduino**:
  - `LiquidCrystal`
  - `Keypad`

---

## Instalación y uso

1. Clonar o descargar este repositorio.
2. Abrir `ProyectoParadigmasCode.ino` en Arduino IDE.
3. Verificar y ajustar la asignación de pines si cambia la conexión de hardware.
4. Subir el sketch a la placa Arduino Mega.
5. En Proteus, abrir `ProyectoFinal.pdsprj` y enlazar el archivo `.hex` generado.
6. Ejecutar la simulación y probar:
   - Pulsar `CENTER` para alternar vista Reloj/Alarma.
   - Pulsar `LEFT` para entrar/salir de modo edición.
   - Pulsar `UP`/`DOWN` para modificar valores.
   - Pulsar `SELECT TONE` para cambiar melodía.
   - Pulsar `STOP ALARM` para silenciar la alarma.

---

## Autores y Créditos

- **Equipo**: Grupo 6  
- **Desarrollador principal**: Wayner Jimenez Vega

---

```

