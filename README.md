<h2>Gerät zur Füllstandsüberwachung von TPE-Granulat in einem Einzelschnecken-Extruder</h2>
<img src="https://user-images.githubusercontent.com/148296/190693663-f1e11b35-3161-4f1b-972f-78b669e3cd6e.jpg">

<h2>Das Problem</h2>
<p>Mein Job war es, die Extruder mit Material zu versorgen. Wenn kein Material mehr in den Trichter gesaugt wurde, lief er leer und das Profil riss ab. Die Maschine musste gestoppt werden, bzw. das Profil musste neu "durchgezogen" und eingestellt werden. Doppelte Arbeit, also musste das vermieden werden.</p>

<h2>Meine Lösung</h2>
<p>Nach Feierabend und an den Wochenenden baute ich dieses portable Gerät und programmierte es so, dass es mittels eines Laser Abstandssensors den Ist-Wert mit dem Soll-Wert verglich. Wenn die Abweichung zu groß wurde, löste ein visueller Alarm aus und ich konnte die Störung beheben.</p>

<h2><b>Verwendte Hardware</b></h2>
<ul>
  <li>Arduino Nano</li>
  <li>Sensor VL53L0X Time-of-Flight (ToF) Laser Abstandssensor</li>
  <li>0,91 Zoll I2C OLED Display</li>
  <li>2 Drucktaster</li>
  <li>Kippschalter</li>
  <li>Batteriehalter 2xAA</li>
  <li>MT3608 DC-DC Boost Converter</li>
</ul>
