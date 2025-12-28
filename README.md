# Gohar Xiters - Pro Cheat Panel (DirectX11 Source)
**Developed by Gohar Rehman**

This is the **Professional Graphical Version** of the Cheat Panel using DirectX11 and ImGui.

## ⚠️ Requirements
*   **Visual Studio 2022** (C++ Desktop Dev workload)
*   **BlueStacks 5** (HD-Player.exe)
*   **Dear ImGui Library** (You must download this!)

## 📦 Setting Up Dependencies (ImGui)
Since I cannot download files to your computer, you must add the ImGui library yourself:
1.  Go to [Dear ImGui GitHub](https://github.com/ocornut/imgui).
2.  Click **Code -> Download ZIP**.
3.  Extract the zip.
4.  Copy the following files into your project's `src/imgui/` folder (create it):
    *   `imgui.cpp`, `imgui.h`, `imgui_demo.cpp`, `imgui_draw.cpp`, `imgui_internal.h`, `imconfig.h`, `imgui_tables.cpp`, `imgui_widgets.cpp`
    *   **From `backends/` folder**: `imgui_impl_dx11.cpp`, `imgui_impl_dx11.h`, `imgui_impl_win32.cpp`, `imgui_impl_win32.h`

## ☁️ How to Build Online (No Download Required)
If you don't want to install Visual Studio (10GB+), you can use **GitHub Actions**:

1.  **Create a GitHub Account**.
2.  **Create a New Repository** (Name it `GoharXiters`).
3.  **Upload Files**: Upload your entire `src` folder, `README.md`, and the `.github` folder I created.
4.  **Wait**: Go to the **"Actions"** tab in your repository. You will see a "Build GoharXiters" workflow running.
5.  **Download**: Click on the workflow run, scroll down to **"Artifacts"**, and download `GoharXiters-Cheat`.

## 🛠️ How to Compile (Local Visual Studio)
1.  Open Visual Studio 2022.
2.  Click **"Create a new project"** -> **"Empty Project (C++)"**.
3.  Name it `GoharXiters` and click Create.
4.  **Add Your Files**:
    *   In the **Solution Explorer** (Right Panel), Right-Click on **Source Files** -> **Add** -> **Existing Item**.
    *   Navigate to your `src` folder.
    *   Select **ALL** `.cpp` files from:
        *   `src/` (`main.cpp`)
        *   `src/memory/` (`Memory.cpp`, `Scanner.cpp`)
        *   `src/imgui/` (`imgui.cpp`, `imgui_draw.cpp`, `imgui_tables.cpp`, `imgui_widgets.cpp`, `imgui_impl_dx11.cpp`, `imgui_impl_win32.cpp`, `imgui_demo.cpp`)
    *   Right-Click on **Header Files** -> **Add** -> **Existing Item**.
    *   Select **ALL** `.h` files from `src/memory/`, `src/gui/`, and `src/imgui/`.
5.  **Build**:
    *   Change the Debug/Release dropdown (top bar) to **Release**.
    *   Change x86/x64 dropdown to **x64**.
    *   Press **Build -> Build Solution** (or Ctrl+Shift+B).
    *   Your EXE will appear in `x64/Release/GoharXiters.exe`.

## 🟢 How to Use
1.  Open `src/memory/Offsets.h` and update your AOB Patterns (get them from Cheat Engine).
2.  Run `GoharXiters.exe` as Admin.
3.  The panel will look for BlueStacks.
4.  Use the Toggle Buttons to enable Aimbot / Scope / Quick Switch (Success Toasts will appear!).

## 🛡️ Anti-Crack Features (New)
*   **Window Randomization**: The window name changes every launch.
*   **Anti-Debug**: Protects against Cheat Engine / x64dbg.
