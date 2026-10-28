#!/usr/bin/env python3
"""
Matrix Cluster & Transit Bridge for IKEA OBEGRÄNSAD
Periodically polls node health and pushes metrics to the matrix display at 192.168.1.28.
Can run as a systemd service or cron job on securegw (192.168.1.40).
"""

import time
import requests
import subprocess
import json

MATRIX_URL = "http://192.168.1.28"

NODES = [
    {"id": 0, "name": "securegw", "host": "192.168.1.40", "type": "ping"},
    {"id": 1, "name": "plantlix", "host": "192.168.1.1", "type": "ping"},
    {"id": 2, "name": "vps-runner", "host": "status.brabros.de", "type": "http"},
    {"id": 3, "name": "storage-vps", "host": "100.64.0.1", "type": "ping"},
    {"id": 4, "name": "ha-core", "host": "192.168.1.170", "port": 8123, "type": "http"}
]

def check_ping(host):
    try:
        res = subprocess.run(["ping", "-c", "1", "-W", "1", host], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        return res.returncode == 0
    except Exception:
        return False

def check_http(url):
    try:
        r = requests.get(url, timeout=2)
        return r.status_code < 500
    except Exception:
        return False

def update_cluster():
    for node in NODES:
        online = False
        if node["type"] == "ping":
            online = check_ping(node["host"])
        elif node["type"] == "http":
            url = f"http://{node['host']}:{node.get('port', 80)}" if not node['host'].startswith("http") else node['host']
            online = check_http(url)

        # Send status to Matrix
        try:
            requests.get(f"{MATRIX_URL}/api/cluster", params={
                "node": node["id"],
                "load": 25 if online else 0,
                "online": 1 if online else 0
            }, timeout=3)
        except Exception as e:
            print(f"Error updating node {node['name']}: {e}")

def main():
    print(f"Matrix Cluster Bridge started. Target: {MATRIX_URL}")
    while True:
        try:
            update_cluster()
        except Exception as e:
            print(f"Error in update cycle: {e}")
        time.sleep(30)

if __name__ == "__main__":
    main()
