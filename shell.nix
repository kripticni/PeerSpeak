{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  buildInputs = with pkgs; [
    cmake
    clang
    pkg-config
    SDL2
    sqlite
    openssl
    asio
    imgui
  ];
}

