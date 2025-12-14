{
  description = "Qt 6 Development Environment";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

  outputs = { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = nixpkgs.legacyPackages.${system};
    in
    {
      devShells.${system}.default = pkgs.mkShell {
 
        nativeBuildInputs = with pkgs; [
          cmake
          ninja

          gcc
          gdb

          llvmPackages_19.clang-tools

          lldb
          vscode-extensions.vadimcn.vscode-lldb

          qt6.wrapQtAppsHook
        ];
        buildInputs = with pkgs; [

          zlib
 
          qt6.qtbase
          qt6.qtserialport
          qt6.qtcharts
        ];

        shellHook = ''
          echo "🎨 Qt 6 Dev Environment Activated!"

          rm -f .clangd

          echo "CompileFlags:" >> .clangd
          echo "  Add:" >> .clangd

          echo "⚙️  Extracting GCC C++ include paths..."
          echo | ${pkgs.gcc}/bin/g++ -Wp,-v -x c++ - -fsyntax-only 2>&1 \
            | grep '^ /nix/store' | sed 's/^ //' | while read -r path; do
              echo "    - \"-isystem$path\"" >> .clangd
          done

          echo "⚙️  Extracting GCC C include paths..."
          echo | ${pkgs.gcc}/bin/gcc -Wp,-v -x c - -fsyntax-only 2>&1 \
            | grep '^ /nix/store' | sed 's/^ //' | while read -r path; do
              echo "    - \"-isystem$path\"" >> .clangd
          done

          echo "✅ .clangd configuration generated successfully."

          export SAD_ADAPTER_PATH="${pkgs.vscode-extensions.vadimcn.vscode-lldb}/share/vscode/extensions/vadimcn.vscode-lldb/adapter/codelldb"

          export PATH="$PATH:$(dirname $SAD_ADAPTER_PATH)"

          echo "🐞 Codelldb path injected."
        '';
      };
    };
}
