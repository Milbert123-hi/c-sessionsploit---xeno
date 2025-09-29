name: Build + Publish Windows Release

on:
  push:
    tags:
      - 'v*'            # run when you create a tag like v1.0.0
  workflow_dispatch:   # also allow manual run from Actions UI

jobs:
  build-windows:
    runs-on: windows-latest
    steps:
      - name: Checkout repo
        uses: actions/checkout@v4

      - name: Setup MSYS2 (mingw-w64 toolchain)
        uses: msys2/setup-msys2@v2
        with:
          update: true
          install: >
            mingw-w64-x86_64-gcc
            mingw-w64-x86_64-zip

      - name: Build static Windows executable (x64)
        shell: bash
        run: |
          # Change this filename if your C++ source has a different name
          SRC="create_rojo_script.cpp"
          EXE="myprog.exe"
          # compile statically to avoid missing DLLs on target machines
          /mingw64/bin/g++ -std=c++17 "${SRC}" -o "${EXE}" -static -static-libgcc -static-libstdc++ -municode || (echo "Build failed"; exit 1)
          ls -lh "${EXE}"

      - name: Zip binary
        shell: bash
        run: |
          EXE="myprog.exe"
          ZIPNAME="myprog-windows-x64.zip"
          /mingw64/bin/zip -r "${ZIPNAME}" "${EXE}"
          sha256sum "${ZIPNAME}" > "${ZIPNAME}.sha256"

      - name: Create GitHub Release (if tag present) or use "manual" fallback
        id: create_release
        uses: actions/create-release@v1
        with:
          tag_name: ${{ github.ref_name != '' && startsWith(github.ref, 'refs/tags/') && github.ref_name || github.sha }}
          release_name: ${{ github.ref_name != '' && startsWith(github.ref, 'refs/tags/') && github.ref_name || 'auto-' + github.sha }}
          body: |
            Automated build: static Windows x64 executable and ZIP.
            Built on: ${{ runner.os }} (GitHub Actions)
          draft: false
          prerelease: false
        env:
          GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}

      - name: Upload ZIP to release
        uses: actions/upload-release-asset@v1
        with:
          upload_url: ${{ steps.create_release.outputs.upload_url }}
          asset_path: ./myprog-windows-x64.zip
          asset_name: myprog-windows-x64.zip
          asset_content_type: application/zip
        env:
          GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}

      - name: Upload checksum file to release
        uses: actions/upload-release-asset@v1
        with:
          upload_url: ${{ steps.create_release.outputs.upload_url }}
          asset_path: ./myprog-windows-x64.zip.sha256
          asset_name: myprog-windows-x64.zip.sha256
          asset_content_type: text/plain
        env:
          GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
