from picamera2 import Picamera2
import cv2
from pyzbar.pyzbar import decode
import time
import numpy as np
import threading

class CameraStreamer:
    def __init__(self):
        self.picam2 = Picamera2()
        self.picam2.configure(self.picam2.create_preview_configuration(main={"format": "RGB888", "size": (640, 480)}))
        self.picam2.start()
        self.frame = None
        self.lock = threading.Lock()
        self.running = True
        threading.Thread(target=self._capture_loop, daemon=True).start()
        self.qr_set = set()  # To store unique QR codes

    def _capture_loop(self):
        while self.running:
            img = self.picam2.capture_array()
            decoded_objects = decode(img)
            for obj in decoded_objects:
                qr_data = obj.data.decode('utf-8')
                print("QR Code Data:", qr_data)
                self.qr_set.add(qr_data)  # Store unique QR codes
                # Draw rectangle and label on image
                pts = obj.polygon
                if pts and len(pts) == 4:
                    pts = [(pt.x, pt.y) for pt in pts]
                    cv2.polylines(img, [np.array(pts)], isClosed=True, color=(0, 255, 0), thickness=2)
                    cv2.putText(img, qr_data, (pts[0][0], pts[0][1] - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (255,0,0), 2)
            _, buffer = cv2.imencode('.jpg', img, [int(cv2.IMWRITE_JPEG_QUALITY), 65])
            with self.lock:
                self.frame = buffer.tobytes()
            time.sleep(0.05)  # ~20 fps

    def get_jpeg_frame(self):
        with self.lock:
            return (b'--frame\r\n'
                b'Content-Type: image/jpeg\r\n\r\n' + self.frame + b'\r\n')

    def generate_frames(self):
        while True:
            frame = self.get_jpeg_frame()
            if frame:
                yield frame
            time.sleep(0.05)  # control frame rate

    def stop(self):
        self.running = False
        self.picam2.stop()

    def get_qr_count(self):
        """Returns the count of unique QR codes detected."""
        return len(self.qr_set)

    def reset_qr_set(self):
        """Resets the set of detected QR codes."""
        self.qr_set.clear()
