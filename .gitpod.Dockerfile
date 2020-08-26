FROM gitpod/workspace-full

# Install custom tools, runtime, etc.
RUN sudo apt-get update \
    && sudo apt-get install -y \
    apt-utils \
    autoconf \
    automake \
    build-essential \
    ccache
    cmake \
    curl \
    cython3 \
    gfortran \
    gdb \
    git \
    jq \
    lcov \
    libblas-dev \
    libboost-dev libboost-serialization-dev libboost-mpi-dev libboost-filesystem-dev libboost-test-dev \
    libfftw3-dev \
    libgsl-dev \
    libhdf5-openmpi-dev \
    liblapack-dev \
    libopenmpi-dev \
    libtool \
    openmpi-bin \
    openssh-client \
    pkg-config \
    python3 \
    python3-coverage \
    python3-dev \
    python3-numpy \
    python3-numpydoc \
    python3-scipy \
    python3-h5py \
    python3-pip \
    python3-lxml \
    python3-requests  \
    python3-setuptools \
    python3-vtk7 \
    rsync \
    vim \
    clang-9 clang-tidy-9 clang-format-9 llvm-9 \
    doxygen \
    ffmpeg \
    gcc-8 g++-8 \
    gcc-9 g++-9 \
    graphviz \
    ipython3 jupyter-notebook jupyter-nbconvert \
    libthrust-dev \
    nvidia-cuda-toolkit \
    python3-matplotlib \
    texlive-base \
    && rm -rf /var/lib/apt/lists/*

RUN pip3 install \
  autopep8==1.3.4 \
  pycodestyle==2.3.1 \
  pylint==2.2.2 \
  astroid==2.1.0 \
  isort==4.3.4 \
  cmake-format==0.6.9 \
  pre-commit \
  'sphinx>=2.0,!=2.1.0,!=3.0.0' \
  sphinxcontrib-bibtex \
  'MDAnalysis>=0.18' \
  'pint>=0.9'
