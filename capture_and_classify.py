import argparse
import time
from pathlib import Path

import requests

from flame_classifier import load_flame_classifier


def capture_and_classify(host: str, port: int, fps: float, output: Path,
                         model_path: str, device: str = "cpu") -> None:
    """Capture frames from ESP32-CAM and classify them with a PyTorch model."""
    classify = load_flame_classifier(model_path, device)
    interval = 1.0 / fps
    url = f"http://{host}:{port}/capture"
    output.mkdir(parents=True, exist_ok=True)
    frame_idx = 0
    print(f"Capturing from {url} at {fps} fps. Press Ctrl+C to stop.")
    try:
        while True:
            start = time.time()
            try:
                resp = requests.get(url, timeout=5)
                resp.raise_for_status()
                timestamp = resp.headers.get("X-Timestamp", str(time.time()))
                filename = output / f"frame_{frame_idx:06d}.jpg"
                with open(filename, "wb") as f:
                    f.write(resp.content)
                label = classify(str(filename))
                print(f"Saved {filename} -> {label} (ts={timestamp})")
                frame_idx += 1
            except Exception as e:
                print(f"Error capturing frame: {e}")
            elapsed = time.time() - start
            if elapsed < interval:
                time.sleep(interval - elapsed)
    except KeyboardInterrupt:
        print("Stopping capture")


def main() -> None:
    parser = argparse.ArgumentParser(description="Capture and classify frames from ESP32-CAM")
    parser.add_argument("host", help="ESP32-CAM host or IP address")
    parser.add_argument("--port", type=int, default=80, help="HTTP port of the camera")
    parser.add_argument("--fps", type=float, default=10.0, help="capture frame rate")
    parser.add_argument("--output", type=Path, default=Path("frames"), help="directory to save images")
    parser.add_argument("--model", required=True, help="path to the PyTorch model file")
    parser.add_argument("--device", default="cpu", help="PyTorch device to run the model on")
    args = parser.parse_args()

    capture_and_classify(args.host, args.port, args.fps, args.output,
                         args.model, args.device)


if __name__ == "__main__":
    main()
