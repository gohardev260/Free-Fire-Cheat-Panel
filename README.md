# Gohar Xiters - Pro Cheat Panel (DirectX11 Source)
**Developed by Gohar Rehman**

This is the **Professional Graphical Version** of the Cheat Panel using DirectX11 and ImGui.

## ⚠️ Requirements
*   **Visual Studio 2022** (C++ Desktop Dev workload)
*   **BlueStacks 5** (HD-Player.exe)
*   **Dear ImGui Library** (You must download this!)

## 📦 Dependencies
All necessary libraries (including ImGui and DirectX headers) are included in this repository. You do not need to download anything extra!


## ☁️ How to Build Online (No Download Required)
If you don't want to install Visual Studio (10GB+), you can use **GitHub Actions**:

1.  **Create a GitHub Account**.
2.  **Create a New Repository** (Name it `GoharXiters`).
3.  **Upload Files**: Upload your entire `src` folder, `README.md`, and the `.github` folder I created.
4.  **Wait**: Go to the **"Actions"** tab in your repository. You will see a "Build GoharXiters" workflow running.
5.  **Download**: Click on the workflow run, scroll down to **"Artifacts"**, and download `GoharXiters-Cheat`.

## 🛠️ How to Compile (Local Visual Studio)
1.  Open Visual Studio 2022.
2.  Click **"Open a local folder"** and select this project folder.
3.  Visual Studio should detect the `CMakeLists.txt` automatically.
4.  Select your configuration (e.g., `x64-Release`) from the toolbar.
5.  Click **Build -> Build All**.

**Alternative: Legacy Solution Method**
1.  Create a "New Empty Project (C++)".
2.  Add all files from `src`, `src/gui`, `src/imgui`, `src/memory`, `src/security` to the project.
3.  Build as **Release x64**.


## 🟢 How to Use
1.  Open `src/memory/Offsets.h` and update your AOB Patterns (get them from Cheat Engine).
2.  Run `GoharXiters.exe` as Admin.
3.  The panel will look for BlueStacks.
4.  Use the Toggle Buttons to enable Aimbot / Scope / Quick Switch (Success Toasts will appear!).

## 🛡️ Anti-Crack Features (New)
*   **Window Randomization**: The window name changes every launch.
*   **Anti-Debug**: Protects against Cheat Engine / x64dbg.
