#!/bin/bash

function download_github_tar_gz_release {
    name=$1
    version=$2
    url="https://github.com/$3/archive/refs/tags/v$version.tar.gz"
    tar_gz="$1.tar.gz"

    rm -rf "$name"
    curl -L -f "$url" --output "$tar_gz"
    tar -xzf "$tar_gz"
    rm -rf "$tar_gz"
    mv "$name-$version" "$name"
}

pushd lib
    rm -rf portaudiocpp
    download_github_tar_gz_release "portaudio" "19.7.0" "PortAudio/portaudio"
    mv portaudio/bindings/cpp ./portaudiocpp
    rm -rf portaudio
popd