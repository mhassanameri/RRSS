#cd ~/Documents/GitHub/RRSS

ZIP="../RRSS-CCS2026-Artifact.zip"

zip -r "$ZIP" . \
  -x ".git/*" \
     "build/*" \
     "cmake-build-asan/*" \
     "cmake-build-debug/*" \
     "cmake-build-release/*" \
     "cmake-build-relwithdebinfo/*" \
     "cmake-build-sanitizer/*" \
     ".idea/*" \
     "**/__pycache__/*" \
     "*.pyc" \
     "*.o" \
     "*.a" \
     "*.so" \
     "*.dylib" \
     "*.log"