# CG2

Repo: `https://github.com/Yunzhuo-Dong/CG2/`

## Setup

### MacOS

To use CG2 under MacOS, build the application inside docker and set screen forwarding.

1. Build docker image:
```
docker build --platform linux/amd64 -t cg2 .
```

2. Strart the container:
```
docker run -it --rm \
--platform linux/amd64 \
-p 5900:5900 \
-v .:/workspace \
cg2 bash
```

3. Inside a container, run Xvfb
```
Xvfb :1 -screen 0 1280x800x24 +extension GLX &
sleep 1
cd /workspace/solution
DISPLAY=:1 LIBGL_ALWAYS_SOFTWARE=1 bash run_exercise0.sh &
sleep 3
x11vnc -display :1 -nopw -listen 0.0.0.0 -forever
```

4. Install VNC Viewer (`brew install --cask vnc-viewer`) and open it with `127.0.0.1:5900`
