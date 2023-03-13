<!--- Author: Matej Otčenáš --->

# X-ray Detector

X-ray Detector is an algorithm based on Python which detects contour of the third metacarpal bone and then measures the width in the narrowest part of the bone.

## Installation Linux

For installation of all dependencies please use provided script `install.sh`. This installation supports only CPU version.
First you need to install tools which can differ according your Linux distribution.

Run this command if you are using Debian like types of distributions:

```bash
sudo apt install python3-pip git
```

For Fedora like distro's run:

```bash
sudo dnf install python3-pip git
```

For Arch Linux run:

```bash
sudo pacman -S python3-pip git
```

Then run following command in terminal:

```bash
sh install.sh
```

This script installs Detectron2 library, Pytorch library and OpenCV library which are largely used in this program.

## Installation Windows 10

For Windows users, the installation can not be processed using only one installation script. Official version of Python Detectron2 library does not support Windows installation, therefore, an unofficial approach of installation must be taken. The installation is based on [this](https://ivanpp.cc/detectron2-walkthrough-windows/) blog, which can be also used as a good Windows installation guide.

- Install Python3 for example from Microsoft Store
- Install Visual Studio 2019 from [here](https://visualstudio.microsoft.com/vs/) where you need to install all Python and C++ extensions
- Install Anaconda enviroment from [here](https://www.anaconda.com/products/individual#windows) 
- Install Git tool form [here](https://git-scm.com/download/win), `select credentail helper option to None`


After neccessary installations, use Anaconda tool first, to create new enviroment. In opened Anaconda program, launch `CMD.exe Prompt` to open Anaconda base enviroment.

Then run this command:

```bash
conda create --name detectron2 python=3.7
conda activate detectron2
```

Now run these few commands in your created `detectron2` enviroment.

```bash
pip install pycocotools
conda install pytorch torchvision torchaudio cpuonly -c pytorch
git clone https://github.com/ivanpp/detectron2.git
cd detectron2
pip install -e .
pip install opencv-python
```

Now you should be ready to run this program using Windows 10 OS.


## Usage
For running the program there are several options. First command `python run.py` executes program, where the input database folder is not specified, therefore `current working directory(cwd)
` is recursively searched for [TIF](https://www.coreldraw.com/en/pages/tif-file/) formatted images.
Option `-n` specifies the image name from which you wish to start the algorithm.
Option `-i` specifies the input path including input directory name.
Option `-o` specifies the output path including output directory name.

```bash
python run.py
python run.py -n "1002_1961LP_F_upr"
python run.py -i "/path/to/input/image/directory"
python run.py -o "/path/to/program/output/directory"
```

### Clearing build folder and Cython generated files
Use provided script `clear.sh`.

Run following command in terminal:
```bash
sh clear.sh
```

## Contact

For any possbile issues please contact me on  <mato.otcenas@gmail.com>

