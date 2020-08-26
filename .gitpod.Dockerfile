FROM gitpod/workspace-full

RUN sudo apt-get update \
    && sudo apt-get install -y \
        apt-utils \
        autoconf \
        automake \
        build-essential \
        ccache \
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
        python3-requests \
        python3-setuptools \
        python3-vtk7 \
        rsync \
        vim \
    && rm -rf /var/lib/apt/lists/*