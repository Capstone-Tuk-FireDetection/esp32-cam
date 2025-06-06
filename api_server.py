import os
from flask import Flask, jsonify, Response
import requests

# Base URL of the ESP32-CAM web server
ESP32_HOST = os.environ.get("ESP32_HOST", "http://192.168.4.1")

app = Flask(__name__)

@app.after_request
def add_cors_headers(resp):
    resp.headers['Access-Control-Allow-Origin'] = '*'
    return resp


def fetch_json(path):
    url = f"{ESP32_HOST}{path}"
    try:
        r = requests.get(url, timeout=5)
        r.raise_for_status()
        return r.json(), 200
    except Exception as e:
        return {"error": str(e)}, 502


@app.route('/flame', methods=['GET'])
def flame():
    data, status = fetch_json('/flame')
    return jsonify(data), status


@app.route('/dht', methods=['GET'])
def dht():
    data, status = fetch_json('/dht')
    return jsonify(data), status


@app.route('/ai', methods=['GET'])
def ai_capture():
    """Fetch a JPEG image from the ESP32 and return it."""
    url = f"{ESP32_HOST}/capture"
    try:
        r = requests.get(url, timeout=5)
        r.raise_for_status()
        return Response(r.content, content_type='image/jpeg')
    except Exception as e:
        return jsonify({"error": str(e)}), 502


if __name__ == '__main__':
    port = int(os.environ.get('PORT', 5000))
    app.run(host='0.0.0.0', port=port)
