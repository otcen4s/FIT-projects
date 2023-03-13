#!/bin/sh

pip3 install torch==1.8.1+cpu torchvision==0.9.1+cpu torchaudio==0.8.1 -f https://download.pytorch.org/whl/torch_stable.html
git clone https://github.com/facebookresearch/detectron2.git
sudo python -m pip install -e detectron2
sudo pip install opencv-python
