# log_imu.py
import paho.mqtt.client as mqtt
import json
import csv

CSV_FILE = "imu_log.csv"
BROKER_HOST = "localhost"
TOPIC = "esp32/accel"

with open(CSV_FILE, "a", newline="") as f:
    writer = csv.writer(f)
    if f.tell() == 0:
        writer.writerow(["device_t_ms", "ax", "ay", "az", "gx", "gy", "gz"])

def on_connect(client, userdata, flags, rc):
    print(f"Connected with result code {rc}")
    client.subscribe(TOPIC)

def on_message(client, userdata, msg):
    try:
        data = json.loads(msg.payload.decode())
        with open(CSV_FILE, "a", newline="") as f:
            writer = csv.writer(f)
            writer.writerow([
                data.get("t"),
                data.get("ax"), data.get("ay"), data.get("az"),
                data.get("gx"), data.get("gy"), data.get("gz"),
            ])
        print(data)
    except Exception as e:
        print("Error parsing message:", e)

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect(BROKER_HOST, 1883, 60)

print(f"Logging IMU data to {CSV_FILE} — Ctrl+C to stop")
client.loop_forever()