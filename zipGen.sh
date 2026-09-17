#cd ~/Documents/GitHub/RRSS

ZIP="../RRSS-CCS2026-Artifact.zip"

zip -r "$ZIP" . \
  -x ".git/*" \
     ".venv/*" \
     "*/.venv/*" \
     "build/*" \
     "build-backup.*/*" \
     "artifact/results/reproduced/?*" \
     "cmake-build-*/*" \
     ".idea/*" \
     "__pycache__/*" \
     "*/__pycache__/*" \
     "*.before-system-python" \
     "*.pyc" \
     "*.o" \
     "*.a" \
     "*.so" \
     "*.dylib" \
     "*.log"