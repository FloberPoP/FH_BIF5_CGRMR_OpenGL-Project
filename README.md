# FH_BIF5_CGRMR_OpenGL Project Setup

Follow these instructions to set up and configure the OpenGL project on your system.

## Step 1: Clone the vcpkg Repository

1. Open **Visual Studio**.
2. Navigate to `Git` -> `Clone Repository...`.
3. Use the following details to clone:
   - **URL:** `https://github.com/microsoft/vcpkg.git`
   - **Path:** `...\FH_BIF5_CGRMR_OpenGL-Project\OpenGL-SnakeGame\vcpkg`

## Step 2: Install Dependencies with vcpkg

1. Open a **Command Prompt** and navigate to:
   ```
   ...\FH_BIF5_CGRMR_OpenGL-Project\OpenGL-SnakeGame\vcpkg
   ```
2. Run the following commands:
   ```cmd
   ./bootstrap-vcpkg.bat
   .\vcpkg install glfw3 glad glm
   .\vcpkg integrate install
   ```

## Step 3: Configure Your Project in Visual Studio

1. In **Solution Explorer**, right-click your project and select `Properties`.
2. Under `VC++ Directories`:
   - Add the **include path** to `Include Directories`.
   - Add the **lib path** to `Library Directories`.

## Step 4: Run the Project

After completing the above steps, the project should now be runnable.
