# pio_builder.py
Import("env")
import shutil
import os

def after_build(source, target, env):
    firmware_path = env.subst("$BUILD_DIR/${PROGNAME}.bin")
    dist_dir = env.subst("$PROJECT_DIR/dist")
    os.makedirs(dist_dir, exist_ok=True)
    shutil.copy(firmware_path, os.path.join(dist_dir, "bara_toolkit.bin"))
    print("\n🩸 BARA FIRMWARE READY!")
    print(f"✅ BIN saved to: {dist_dir}/bara_toolkit.bin")
    print("🔥 Upload to ESP32 via esptool or Arduino IDE.\n")

env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", after_build)
