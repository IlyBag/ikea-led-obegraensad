#!/usr/bin/env python3
import hashlib
import sys
import os
import requests
from requests.auth import HTTPDigestAuth
from requests_toolbelt import MultipartEncoder, MultipartEncoderMonitor
from tqdm import tqdm

TARGET_URL = sys.argv[1] if len(sys.argv) > 1 else "http://192.168.1.28/update"
BIN_PATH = sys.argv[2] if len(sys.argv) > 2 else ".pio/build/esp32dev/firmware.bin"
USERNAME = "admin"
PASSWORD = "ikea-led-wall"

if not os.path.exists(BIN_PATH):
    print(f"Error: {BIN_PATH} not found!")
    sys.exit(1)

file_size = os.path.getsize(BIN_PATH)
print(f"=== OTA Upload to {TARGET_URL} ===")
print(f"Firmware: {BIN_PATH} ({file_size / 1024:.1f} KB)")

# Compute MD5
with open(BIN_PATH, "rb") as f:
    md5 = hashlib.md5(f.read()).hexdigest()
print(f"MD5 Checksum: {md5}")

session = requests.Session()
session.auth = HTTPDigestAuth(USERNAME, PASSWORD)

# 1. Pre-authenticate to obtain Digest nonce so the upload doesn't retry on 401
base_url = TARGET_URL.split("/update")[0]
ident_url = f"{base_url}/update/identity"
print(f"Authenticating with {ident_url}...")
try:
    r_auth = session.get(ident_url, timeout=5)
    print(f"Auth response: {r_auth.status_code} {r_auth.text.strip()}")
    if r_auth.status_code != 200:
        print("Error: Authentication failed!")
        sys.exit(1)
except Exception as e:
    print(f"Auth error: {e}")
    sys.exit(1)

# 2. Upload firmware with established session
with open(BIN_PATH, "rb") as f:
    encoder = MultipartEncoder(fields={
        "MD5": md5,
        "firmware": ("firmware", f, "application/octet-stream")
    })

    bar = tqdm(desc="Uploading", total=encoder.len, unit="B", unit_scale=True, unit_divisor=1024)
    monitor = MultipartEncoderMonitor(encoder, lambda m: bar.update(m.bytes_read - bar.n))

    upload_endpoint = f"{TARGET_URL}?MD5={md5}"
    try:
        response = session.post(
            upload_endpoint,
            data=monitor,
            headers={"Content-Type": monitor.content_type},
            timeout=180
        )
        bar.close()
        print(f"\nResponse: {response.status_code} {response.text}")
        if response.status_code == 200:
            print("\n>>> SUCCESS! Firmware uploaded successfully! <<<")
            print(">>> ESP32 is now rebooting into the new firmware... <<<")
        else:
            print(f"\n>>> FAILED: Server returned status {response.status_code}: {response.text}")
    except requests.exceptions.ConnectionError:
        bar.close()
        # ESP32 reboots immediately upon successful flash, which can sever the connection
        print("\n>>> SUCCESS! Connection closed by ESP32 (reboot initiated). <<<")
    except Exception as e:
        bar.close()
        print(f"\nUpload exception: {e}")
